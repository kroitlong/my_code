/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "analyze.h"

/**
 * @description: 分析器，进行语义分析和查询重写，需要检查不符合语义规定的部分
 * @param {shared_ptr<ast::TreeNode>} parse parser生成的结果集
 * @return {shared_ptr<Query>} Query
 */
std::shared_ptr<Query> Analyze::do_analyze(std::shared_ptr<ast::TreeNode> parse) {
    std::shared_ptr<Query> query = std::make_shared<Query>();
    if (auto x = std::dynamic_pointer_cast<ast::SelectStmt>(parse)) {
        // 处理表名
        query->tables = std::move(x->tabs);
        /** TODO: 检查表是否存在 */
        for (auto &table : query->tables) {
            bool is_table = sm_manager_->db_.is_table(table);
            if (!is_table) {
                throw TableNotFoundError(table);
            }
        }
        // 处理target list，在target list中添加上表名，例如 a.id
        for (auto &sv_sel_col : x->cols) {
            TabCol sel_col = {.tab_name = sv_sel_col->tab_name,
                              .col_name = sv_sel_col->col_name,
                              .as_name = sv_sel_col->as_name,
                              .aggregator = sv_sel_col->aggregator
                             };
            query->cols.push_back(std::move(sel_col));

        }

        std::vector<ColMeta> all_cols;
        get_all_cols(query->tables, all_cols);
        if (query->cols.size() == 0) {
            // select all columns
            for (auto &col : all_cols) {
                TabCol sel_col = {.tab_name = col.tab_name,
                                  .col_name = col.name,
                                  .as_name = "",
                                  .aggregator = ast::EMPTY,
                                 };
                query->cols.push_back(sel_col);
            }
        } else {
            // infer table name from column name
            for (auto &sel_col : query->cols) {
                sel_col = check_column(all_cols, sel_col);  // 列元数据校验
            }
        }

        //处理where条件
        get_clause(x->conds, query->conds);
        check_clause(query->tables, query->conds);
    } else if (auto x = std::dynamic_pointer_cast<ast::UpdateStmt>(parse)) {
        /** TODO: */
        std::string table_name = x->tab_name;
        //检查表是否存在
        bool exist = sm_manager_->db_.is_table(table_name);
        if (!exist) {
            throw TableNotFoundError(table_name);
        }
        auto table_meta = sm_manager_->db_.get_table(table_name);

        // 获取query->set_clauses
        for (auto &clause : x->set_clauses) {
            SetClause st_cls;
            // 1. 获取st_cls.sel_col
            TabCol sel_col = {.tab_name = table_name,
                              .col_name = clause->col_name,
                              .as_name = "",
                              .aggregator = ast::EMPTY,
                             };
            st_cls.lhs = sel_col;
            // 2. 获取st_cls.value
            ColMeta col = *(table_meta.get_col(sel_col.col_name));
            auto value = convert_sv_value(clause->val);
            if (col.type == TYPE_BIGINT && value.type == TYPE_INT) {
                value.type = TYPE_BIGINT;
                value.set_bigint(static_cast<int64_t>(value.int_val), 1);
            }
            st_cls.rhs = value;
            switch (st_cls.rhs.type) {
            case TYPE_INT:
                st_cls.rhs.init_raw(sizeof(int));
                break;
            case TYPE_FLOAT:
                st_cls.rhs.init_raw(sizeof(float));
                break;
            case TYPE_STRING:
                st_cls.rhs.init_raw(sizeof(st_cls.rhs.str_val));
                break;
            case TYPE_DATETIME:
                st_cls.rhs.init_raw(sizeof(st_cls.rhs.datetime_val));
                break;
            case TYPE_BIGINT:
                st_cls.rhs.init_raw(sizeof(st_cls.rhs.bigint_val));
            }
            //std::cout << st_cls.rhs.raw->data << std::endl;
            query->set_clauses.push_back(st_cls);
        }
        // 处理where条件
        get_clause(x->conds, query->conds);
        check_clause({x->tab_name}, query->conds);
    } else if (auto x = std::dynamic_pointer_cast<ast::DeleteStmt>(parse)) {
        //处理where条件
        get_clause(x->conds, query->conds);
        check_clause({x->tab_name}, query->conds);
    } else if (auto x = std::dynamic_pointer_cast<ast::InsertStmt>(parse)) {
        // 处理insert 的values值
        for (auto &sv_val : x->vals) {
            query->values.push_back(convert_sv_value(sv_val));
        }
    } else {
        // do nothing
    }
    query->parse = std::move(parse);
    return query;
}


TabCol Analyze::check_column(const std::vector<ColMeta> &all_cols, TabCol target) {
    if (target.tab_name.empty()) {
        // Table name not specified, infer table name from column name
        std::string tab_name;
        for (auto &col : all_cols) {
            if (col.name == target.col_name) {
                if (!tab_name.empty()) {
                    throw AmbiguousColumnError(target.col_name);
                }
                tab_name = col.tab_name;
            }
        }
        if (target.col_name == "*") {
            tab_name = "ALL_TABLE";
        }
        if (tab_name.empty()) {
            throw ColumnNotFoundError(target.col_name);
        }
        target.tab_name = tab_name;
    } else {
        /** TODO: Make sure target column exists */
        //------------------------------------------------------
        bool col_found = false;
        for (auto &col : all_cols) {
            if ((col.tab_name == target.tab_name && col.name == target.col_name) ||
                    col.tab_name == target.tab_name && target.col_name == "*" ||
                    target.tab_name == "" && target.col_name == ".*"
               ) {
                col_found = true;
            }
        }
        if (col_found == false) {
            throw ColumnNotFoundError(target.col_name);
        }
        //-------------------------------------------------------
    }
    return target;
}

void Analyze::get_all_cols(const std::vector<std::string> &tab_names, std::vector<ColMeta> &all_cols) {
    for (auto &sel_tab_name : tab_names) {
        // 这里db_不能写成get_db(), 注意要传指针
        const auto &sel_tab_cols = sm_manager_->db_.get_table(sel_tab_name).cols;
        all_cols.insert(all_cols.end(), sel_tab_cols.begin(), sel_tab_cols.end());
    }
}

void Analyze::get_clause(const std::vector<std::shared_ptr<ast::BinaryExpr>> &sv_conds, std::vector<Condition> &conds) {
    conds.clear();
    for (auto &expr : sv_conds) {
        Condition cond;
        cond.lhs_col = {.tab_name = expr->lhs->tab_name, .col_name = expr->lhs->col_name};
        cond.op = convert_sv_comp_op(expr->op);
        if (auto rhs_val = std::dynamic_pointer_cast<ast::Value>(expr->rhs)) {
            cond.is_rhs_val = true;
            cond.rhs_val = convert_sv_value(rhs_val);
        } else if (auto rhs_col = std::dynamic_pointer_cast<ast::Col>(expr->rhs)) {
            cond.is_rhs_val = false;
            cond.rhs_col = {.tab_name = rhs_col->tab_name, .col_name = rhs_col->col_name};
        }
        conds.push_back(cond);
    }
}

void Analyze::check_clause(const std::vector<std::string> &tab_names, std::vector<Condition> &conds) {
    // auto all_cols = get_all_cols(tab_names);
    std::vector<ColMeta> all_cols;
    get_all_cols(tab_names, all_cols);
    // Get raw values in where clause
    for (auto &cond : conds) {
        // Infer table name from column name
        cond.lhs_col = check_column(all_cols, cond.lhs_col);
        if (!cond.is_rhs_val) {
            cond.rhs_col = check_column(all_cols, cond.rhs_col);
        }
        TabMeta &lhs_tab = sm_manager_->db_.get_table(cond.lhs_col.tab_name);
        auto lhs_col = lhs_tab.get_col(cond.lhs_col.col_name);
        ColType lhs_type = lhs_col->type;
        ColType rhs_type;
        if (cond.is_rhs_val) {
            rhs_type = cond.rhs_val.type;
            if (lhs_type == TYPE_BIGINT && rhs_type == TYPE_INT) {
                rhs_type = TYPE_BIGINT;
                cond.rhs_val.type = TYPE_BIGINT;
                cond.rhs_val.set_bigint(static_cast<int64_t>(cond.rhs_val.int_val), 1);
            }
            cond.rhs_val.init_raw(lhs_col->len);
        } else {
            TabMeta &rhs_tab = sm_manager_->db_.get_table(cond.rhs_col.tab_name);
            auto rhs_col = rhs_tab.get_col(cond.rhs_col.col_name);
            rhs_type = rhs_col->type;
        }
        if (lhs_type != rhs_type) {
            throw IncompatibleTypeError(coltype2str(lhs_type), coltype2str(rhs_type));
        }
    }
}


Value Analyze::convert_sv_value(const std::shared_ptr<ast::Value> &sv_val) {
    Value val;
    if (auto int_lit = std::dynamic_pointer_cast<ast::IntLit>(sv_val)) {
        val.set_int(int_lit->val);
    } else if (auto float_lit = std::dynamic_pointer_cast<ast::FloatLit>(sv_val)) {
        val.set_float(float_lit->val);
    } else if (auto str_lit = std::dynamic_pointer_cast<ast::StringLit>(sv_val)) {
        val.set_str(str_lit->val);
    } else if (auto datetime_lit = std::dynamic_pointer_cast<ast::datetime>(sv_val)) {
        val.set_datetime(datetime_lit->time);
    } else if (auto bigint_lit = std::dynamic_pointer_cast<ast::bigint>(sv_val)) {
        val.set_bigint(bigint_lit->val, bigint_lit->is_legal);
    } else {
        throw InternalError("Unexpected sv value type");
    }
    return val;
}


CompOp Analyze::convert_sv_comp_op(ast::SvCompOp op) {
    std::map<ast::SvCompOp, CompOp> m = {
        {ast::SV_OP_EQ, OP_EQ}, {ast::SV_OP_NE, OP_NE}, {ast::SV_OP_LT, OP_LT},
        {ast::SV_OP_GT, OP_GT}, {ast::SV_OP_LE, OP_LE}, {ast::SV_OP_GE, OP_GE},
    };
    return m.at(op);
}
