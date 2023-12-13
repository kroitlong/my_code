/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */
#pragma once
#include <sstream>
#include <iomanip>
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "system/sm.h"

class IndexScanExecutor : public AbstractExecutor {
private:
    std::string tab_name_;                      // 表名称
    TabMeta tab_;                               // 表的元数据
    std::vector<Condition> conds_;              // 扫描条件
    RmFileHandle *fh_;                          // 表的数据文件句柄
    std::vector<ColMeta> cols_;                 // 需要读取的字段
    size_t len_;                                // 选取出来的一条记录的长度
    std::vector<Condition> fed_conds_;          // 扫描条件，和conds_字段相同

    std::vector<std::string> index_col_names_;  // index scan涉及到的索引包含的字段
    IndexMeta index_meta_;                      // index scan涉及到的索引元数据

    Rid rid_;
    std::unique_ptr<RecScan> scan_;

    SmManager *sm_manager_;

    char *key_start;
    char *key_end;
    bool start_initialized = false;
    bool end_initialized = false;
    Iid start_Iid;
    Iid end_Iid;

    IxManager *ix_manager;
    IxIndexHandle *ix_index_handle;
public:
    IndexScanExecutor(SmManager *sm_manager, std::string tab_name, std::vector<Condition> conds, std::vector<std::string> index_col_names,
                      Context *context) {
        sm_manager_ = sm_manager;
        context_ = context;
        tab_name_ = std::move(tab_name);
        tab_ = sm_manager_->db_.get_table(tab_name_);
        conds_ = std::move(conds);
        index_col_names_ = index_col_names;
        index_meta_ = *(tab_.get_index_meta(index_col_names_));
        fh_ = sm_manager_->fhs_.at(tab_name_).get();
        cols_ = tab_.cols;
        len_ = cols_.back().offset + cols_.back().len;
        // 定义一个映射以交换比较运算符
        std::map<CompOp, CompOp> swap_op = {
            {OP_EQ, OP_EQ}, {OP_NE, OP_NE}, {OP_LT, OP_GT}, {OP_GT, OP_LT}, {OP_LE, OP_GE}, {OP_GE, OP_LE},
        };

        // 分配内存并初始化key_start和key_end缓冲区
        key_start = new char[index_meta_.col_tot_len];
        key_end = new char[index_meta_.col_tot_len];
        memset(key_start, 0, index_meta_.col_tot_len);
        memset(key_end, 0, index_meta_.col_tot_len);

        // 获取索引管理器和索引句柄
        ix_manager = sm_manager_->get_ix_manager();
        ix_index_handle = sm_manager_->ihs_.at(ix_manager->get_index_name(tab_name_, index_col_names_)).get();
        // 如果左侧列（lhs）不来自当前表，则交换条件
        for (auto &cond : conds_) {
            if (cond.lhs_col.tab_name != tab_name_) {
                // lhs列来自另一个表，rhs必须来自当前表
                assert(!cond.is_rhs_val && cond.rhs_col.tab_name == tab_name_);
                // 交换lhs和rhs使得他们指向正确的位置
                std::swap(cond.lhs_col, cond.rhs_col);
                cond.op = swap_op.at(cond.op);
            }
        }
        // 找到索引查询的key上下界
        int offset = 0;
        for (std::string index_col_name : index_col_names) {
            Value rhs_value;
            Condition find_cond;
            for (auto &cond : conds_) {
                if (cond.is_rhs_val && cond.lhs_col.tab_name == tab_name_ && cond.lhs_col.col_name == index_col_name) {
                    rhs_value = cond.rhs_val;
                    find_cond = cond;
                    break;
                }
            }
            const char *buf;
            switch (rhs_value.type) {
            case TYPE_INT:
                buf = (char *)&rhs_value.int_val;
                break;
            case TYPE_FLOAT:
                buf = (char *)&rhs_value.float_val;
                break;
            case TYPE_STRING:
                buf = rhs_value.str_val.c_str();
                break;
            case TYPE_DATETIME: {
                std::memcpy(const_cast<char *>(buf), rhs_value.datetime_val.time, sizeof(rhs_value.datetime_val.time));
            }
            case TYPE_BIGINT:
                buf = (char *)&rhs_value.bigint_val;
            default:
                break;
            }
            ColMeta index_col = *get_col(index_meta_.cols, find_cond.lhs_col);
            switch (find_cond.op) {
            case OP_EQ: {
                memcpy(key_start + offset, buf, index_col.len);
                memcpy(key_end + offset, buf, index_col.len);
                start_initialized = true;
                end_initialized = true;
                break;
            }
            default:
                break;
            }
            offset += index_col.len;
        }
        // 根据键的下界和上界设置start_Iid和end_Iid
        if (!start_initialized) {
            start_Iid = ix_index_handle->leaf_begin();
        } else {
            start_Iid = ix_index_handle->lower_bound(key_start);
        }
        if (!end_initialized) {
            end_Iid = ix_index_handle->leaf_end();
        } else {
            end_Iid = ix_index_handle->upper_bound(key_end);
        }
        // 将过滤后的条件设置为与原始条件相同
        fed_conds_ = conds_;

    }

    void beginTuple() override {
        // 创建IxScan对象，并设置起始和结束迭代器
        scan_ = std::make_unique<IxScan>(ix_index_handle,
                                         start_Iid,
                                         end_Iid,
                                         sm_manager_->get_bpm());
        // 在扫描范围内循环，直到找到满足条件的记录或扫描结束
        while (!scan_->is_end()) {
            rid_ = scan_->rid();
            try {
                // 获取记录并检查是否满足条件
                auto rec = fh_->get_record(rid_, context_);
            } catch (RecordNotFoundError &e) {
                std::cerr << e.what() << std::endl;
            }
            // 移动到下一个记录
            scan_->next();
        }
    }

    void nextTuple() override {
        assert(!is_end());
        // 移动到下一个记录并检查是否满足条件
        for (scan_->next(); !scan_->is_end(); scan_->next()) {
            rid_ = scan_->rid();
            try {
                auto rec = fh_->get_record(rid_, context_);
            } catch (RecordNotFoundError &e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }

    std::unique_ptr<RmRecord> Next() override {
        // 获取当前记录并返回
        return fh_->get_record(scan_->rid(), context_);
    }

    bool is_end() const override {
        // 检查扫描是否结束
        return scan_->is_end();
    }

    Rid &rid() override {
        return rid_;
    }
};