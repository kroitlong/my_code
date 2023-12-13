/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */
#include <sstream>
#include <iomanip>
#include <set>
#include "execution_manager.h"

#include "executor_delete.h"
#include "executor_index_scan.h"
#include "executor_insert.h"
#include "executor_nestedloop_join.h"
#include "executor_projection.h"
#include "executor_seq_scan.h"
#include "executor_update.h"
#include "index/ix.h"
#include "record_printer.h"
#include "parser/ast.h"

const char *help_info = "Supported SQL syntax:\n"
                        "  command ;\n"
                        "command:\n"
                        "  CREATE TABLE table_name (column_name type [, column_name type ...])\n"
                        "  DROP TABLE table_name\n"
                        "  CREATE INDEX table_name (column_name)\n"
                        "  DROP INDEX table_name (column_name)\n"
                        "  INSERT INTO table_name VALUES (value [, value ...])\n"
                        "  DELETE FROM table_name [WHERE where_clause]\n"
                        "  UPDATE table_name SET column_name = value [, column_name = value ...] [WHERE where_clause]\n"
                        "  SELECT selector FROM table_name [WHERE where_clause]\n"
                        "type:\n"
                        "  {INT | FLOAT | CHAR(n)}\n"
                        "where_clause:\n"
                        "  condition [AND condition ...]\n"
                        "condition:\n"
                        "  column op {column | value}\n"
                        "column:\n"
                        "  [table_name.]column_name\n"
                        "op:\n"
                        "  {= | <> | < | > | <= | >=}\n"
                        "selector:\n"
                        "  {* | column [, column ...]}\n";

// 主要负责执行DDL语句
void QlManager::run_mutli_query(std::shared_ptr<Plan> plan, Context *context) {
    if (auto x = std::dynamic_pointer_cast<DDLPlan>(plan)) {
        switch (x->tag) {
        case T_CreateTable: {
            sm_manager_->create_table(x->tab_name_, x->cols_, context);
            break;
        }
        case T_DropTable: {
            //在删除表前，将内存中关于该表的内容清除
            RmFileHandle *fh_ = sm_manager_->fhs_.at(x->tab_name_).get();
            std::unique_ptr<RecScan> scan_ = std::make_unique<RmScan>(fh_);
            while (!scan_->is_end()) {
                fh_->delete_record(scan_->rid(), context);
                std::cout << "删除的rid<page_no, slot_no>: " << scan_->rid().page_no << " " << scan_->rid().slot_no << std::endl;
                scan_->next();
            }
            //删除表
            sm_manager_->drop_table(x->tab_name_, context);
            break;
        }
        case T_CreateIndex: {
            sm_manager_->create_index(x->tab_name_, x->tab_col_names_, context);
            break;
        }
        case T_DropIndex: {
            sm_manager_->drop_index(x->tab_name_, x->tab_col_names_, context);
            break;
        }
        default:
            throw InternalError("Unexpected field type");
            break;
        }
    }
}

// 执行help; show tables; desc table; begin; commit; abort;语句
void QlManager::run_cmd_utility(std::shared_ptr<Plan> plan, txn_id_t *txn_id, Context *context) {
    if (auto x = std::dynamic_pointer_cast<OtherPlan>(plan)) {
        switch (x->tag) {
        case T_Help: {
            memcpy(context->data_send_ + * (context->offset_), help_info, strlen(help_info));
            *(context->offset_) = strlen(help_info);
            break;
        }
        case T_ShowTable: {
            sm_manager_->show_tables(context);
            break;
        }
        case T_ShowIndex: {
            sm_manager_->show_index(x->tab_name_, context);
            break;
        }
        case T_DescTable: {
            sm_manager_->desc_table(x->tab_name_, context);
            break;
        }
        case T_Transaction_begin: {
            // 显示开启一个事务
            context->txn_->set_txn_mode(true);
            break;
        }
        case T_Transaction_commit: {
            context->txn_ = txn_mgr_->get_transaction(*txn_id);
            txn_mgr_->commit(context->txn_, context->log_mgr_);
            break;
        }
        case T_Transaction_rollback: {
            context->txn_ = txn_mgr_->get_transaction(*txn_id);
            txn_mgr_->abort(context->txn_, context->log_mgr_);
            break;
        }
        case T_Transaction_abort: {
            context->txn_ = txn_mgr_->get_transaction(*txn_id);
            txn_mgr_->abort(context->txn_, context->log_mgr_);
            break;
        }
        default:
            throw InternalError("Unexpected field type");
            break;
        }

    }
}

// 执行select语句，select语句的输出除了需要返回客户端外，还需要写入output.txt文件中
void QlManager::select_from(std::unique_ptr<AbstractExecutor> executorTreeRoot, std::vector<TabCol> sel_cols,
                            Context *context) {
    std::vector<std::string> captions;
    captions.reserve(sel_cols.size());
    //用于记录使用聚合函数时该列的位置
    std::vector<size_t> positions;
    size_t position;
    for (auto &sel_col : sel_cols) {
        if (sel_col.aggregator == ast::EMPTY) {
            captions.push_back(sel_col.col_name);
        } else {
            captions.push_back(sel_col.as_name);
            position = captions.size() - 1;
            positions.push_back(position);
        }
    }
    // Print header into buffer
    RecordPrinter rec_printer(sel_cols.size());
    rec_printer.print_separator(context);
    rec_printer.print_record(captions, context);
    rec_printer.print_separator(context);
    //print header into file
    std::fstream outfile;
    outfile.open("output.txt", std::ios::out | std::ios::app);
    outfile << "|";
    for (int i = 0; i < captions.size(); ++i) {
        outfile << " " << captions[i] << " |";
    }
    outfile << "\n";
    outfile.close();

    // Print records
    size_t record_num = 0;
    int sum_int = 0; float sum_float = 0.0;
    std::vector<std::vector<std::string>> table;
    //执行聚合函数时聚合函数的类型
    ast::SvAggre type;
    std::vector<TabCol>::iterator it;
    // 执行query_plan
    for (executorTreeRoot->beginTuple(); !executorTreeRoot->is_end(); executorTreeRoot->nextTuple()) {
        auto Tuple = executorTreeRoot->Next();
        std::vector<std::string> columns;
        //表示是否执行聚合函数
        bool flag = false;
        it = sel_cols.begin();
        for (auto &col : executorTreeRoot->cols()) {
            std::string col_str;
            char *rec_buf = Tuple->data + col.offset;
            //判断是否需要执行聚合函数
            for (; it != sel_cols.end(); it++) {
                if (it->col_name == col.name) {
                    if (it->aggregator != ast::EMPTY) {
                        type = it->aggregator;
                        it->is_count_all = false;
                        flag = true;
                        break;
                    } else {
                        continue;
                    }
                } else if (it->col_name == "*" && it->is_count_all != true) {
                    it->is_count_all = true;
                    break;
                }
            }
            if (it->is_count_all == true) {
                type = ast::SV_COUNT;
                flag = true;
            }
            //提前对字符串进行处理
            if (col.type == TYPE_INT) {
                col_str = std::to_string(*(int *)rec_buf);
            } else if (col.type == TYPE_FLOAT) {
                col_str = std::to_string(*(float *)rec_buf);
            } else if (col.type == TYPE_STRING) {
                col_str = std::string((char *)rec_buf, col.len);
                col_str.resize(strlen(col_str.c_str()));
            }
            //表示不执行聚合函数，只是普通查询
            if (flag == false) {
                if (col.type == TYPE_DATETIME) {
                    std::ostringstream oss;
                    if (static_cast<int>(rec_buf[7]) == 0)
                        throw InvalidValueError();
                    oss << std::setw(2) << std::setfill('0') << static_cast<int>(rec_buf[0])
                        << std::setw(2) << std::setfill('0') << static_cast<int>(rec_buf[1]) << "-"
                        << std::setw(2) << std::setfill('0') << static_cast<int>(rec_buf[2]) << "-"
                        << std::setw(2) << std::setfill('0') << static_cast<int>(rec_buf[3]) << " "
                        << std::setw(2) << std::setfill('0') << static_cast<int>(rec_buf[4]) << ":"
                        << std::setw(2) << std::setfill('0') << static_cast<int>(rec_buf[5]) << ":"
                        << std::setw(2) << std::setfill('0') << static_cast<int>(rec_buf[6]);

                    col_str = oss.str();
                    col_str.resize(strlen(col_str.c_str()));
                } else if (col.type == TYPE_BIGINT) {
                    if (*(int64_t *)rec_buf == static_cast<int64_t>(-1)) {
                        throw InvalidValueError();
                    }
                    col_str = std::to_string(*(int64_t *)rec_buf);
                }
                col.a_type = ast::EMPTY;
                columns.push_back(col_str);
            } else { //根据聚合函数类型执行聚合函数
                if (type == ast::SV_COUNT || type == ast::SV_MAX || type == ast::SV_MIN) {
                    if (col.type != TYPE_INT && col.type != TYPE_FLOAT && col.type != TYPE_STRING) {
                        throw InvalidTypeError();
                    }
                    columns.push_back(col_str);
                } else if (type == ast::SV_SUM) {
                    if (col.type != TYPE_INT && col.type != TYPE_FLOAT) {
                        throw InvalidTypeError();
                    }
                    columns.push_back(col_str);
                }
                flag = false;
            }
        }
        table.push_back(std::move(columns));
        columns.clear();
        ++record_num;
    }
    //select count(id) as count_id,count(id) as new_count_id,count(name) as count_name,count(name) as count_new_name,count(val) as count_val,count(val) as count_new_val, co
    //unt(*) as count_all from aggregate;
    //根据是否使用聚合函数以及使用聚合函数的类型，对表进行一定的处理
    for (auto i = 0; i < positions.size(); ++i) {
        if (sel_cols[positions[i]].aggregator == ast::SV_COUNT) {
            size_t count = 0;
            //注意，这两者没有本质区别，本来想的是指定列的统计要去重，后来发现好像不去重
            if (sel_cols[positions[i]].is_count_all == true) {
                count = table.size();
                sel_cols[positions[i]].is_count_all = false;
            } else {
                for (auto j = 0; j < table.size(); j++) {
                    if (table[j][positions[i]] != "")
                        count++;
                }
            }
            //修改表每列的值
            for (auto j = 0; j < table.size(); ++j) {
                table[j][positions[i]] = std::to_string(count);
            }
        } else if (sel_cols[positions[i]].aggregator == ast::SV_MAX) {
            //查找每列的极值
            std::string temp = table[0][positions[i]];
            if (executorTreeRoot->cols()[positions[i]].type == TYPE_INT) {
                for (auto j = 1; j < table.size(); ++j) {
                    if (stoi(temp) < stoi(table[j][positions[i]]))
                        temp = table[j][positions[i]];
                }
            } else if (executorTreeRoot->cols()[positions[i]].type == TYPE_FLOAT) {
                for (auto j = 1; j < table.size(); ++j) {
                    if (stof(temp) < stof(table[j][positions[i]]))
                        temp = table[j][positions[i]];
                }
            } else if (executorTreeRoot->cols()[positions[i]].type == TYPE_STRING) {
                for (auto j = 1; j < table.size(); ++j) {
                    if (temp < table[j][positions[i]])
                        temp = table[j][positions[i]];
                }
            }
            //修改表每列的值
            for (auto j = 0; j < table.size(); ++j) {
                table[j][positions[i]] = temp;
            }

        } else if (sel_cols[positions[i]].aggregator == ast::SV_MIN) {
            //查找每列的极值
            std::string temp = table[0][positions[i]];
            if (executorTreeRoot->cols()[positions[i]].type == TYPE_INT) {
                for (auto j = 1; j < table.size(); ++j) {
                    if (stoi(temp) > stoi(table[j][positions[i]]))
                        temp = table[j][positions[i]];
                }
            } else if (executorTreeRoot->cols()[positions[i]].type == TYPE_FLOAT) {
                for (auto j = 1; j < table.size(); ++j) {
                    if (stof(temp) > stof(table[j][positions[i]]))
                        temp = table[j][positions[i]];
                }
            } else if (executorTreeRoot->cols()[positions[i]].type == TYPE_STRING) {
                for (auto j = 1; j < table.size(); ++j) {
                    if (temp > table[j][positions[i]])
                        temp = table[j][positions[i]];
                }
            }
            //修改表每列的值
            for (auto j = 0; j < table.size(); ++j) {
                table[j][positions[i]] = temp;
            }

        } else if (sel_cols[positions[i]].aggregator == ast::SV_SUM) {
            std::string temp;
            if (executorTreeRoot->cols()[positions[i]].type == TYPE_INT) {
                for (auto j = 0; j < table.size(); ++j ) {
                    sum_int += stoi(table[j][positions[i]]);
                }
                temp = std::to_string(sum_int);
            } else {
                for (auto j = 0; j < table.size(); ++j ) {
                    sum_float += stof(table[j][positions[i]]);
                }
                temp = std::to_string(sum_float);
            }
            //修改表每列的值
            for (auto j = 0; j < table.size(); ++j) {
                table[j][positions[i]] = temp;
            }
            sum_int = 0;
            sum_float = 0.0;
        }
    }

    //若查询的列全是聚合函数构成的列，则只有1行
    if (positions.size() == sel_cols.size() && table.size() != 1) {
        if (!table.empty()) {
            table.erase(table.begin() + 1, table.end());
        }
    }
    std::vector<std::vector<std::string>> new_table;
    std::set<std::vector<std::string>> temp;
    bool is_newtable = false;
    //若查询的列不全是聚合函数，则需要删去重复的行
    if (!positions.empty() && positions.size() < sel_cols.size()) {
        for (auto i = 0; i < table.size(); i++) {
            if (temp.find(table[i]) == temp.end()) {
                temp.insert(table[i]);
                new_table.push_back(table[i]);
            }
        }
        is_newtable = true;
    }
    //打印整张表
    if (!is_newtable) {
        outfile.open("output.txt", std::ios::out | std::ios::app);
        for (auto i = 0; i < table.size(); i++) {
            // print record into buffer
            rec_printer.print_record(table[i], context);
            // print record into file
            outfile << "|";
            for (int j = 0; j < table[i].size(); ++j) {
                outfile << " " << table[i][j] << " |";
            }
            outfile << "\n";
        }
    } else {
        outfile.open("output.txt", std::ios::out | std::ios::app);
        for (auto i = 0; i < new_table.size(); i++) {
            // print record into buffer
            rec_printer.print_record(new_table[i], context);
            // print record into file
            outfile << "|";
            for (int j = 0; j < new_table[i].size(); ++j) {
                outfile << " " << new_table[i][j] << " |";
            }
            outfile << "\n";
        }
    }
    outfile.close();
    // Print footer into buffer
    rec_printer.print_separator(context);
    // Print record count into buffer
    RecordPrinter::print_record_count(table.size(), context);
    for (auto &inner_vector : table) {
        // 释放内层的column
        inner_vector.clear();
    }
    // 释放外层的table
    table.clear();
}

// 执行DML语句
void QlManager::run_dml(std::unique_ptr<AbstractExecutor> exec) {
    exec->Next();
}



