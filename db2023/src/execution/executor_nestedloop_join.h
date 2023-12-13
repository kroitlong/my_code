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
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "system/sm.h"

class NestedLoopJoinExecutor : public AbstractExecutor {
private:
    std::unique_ptr<AbstractExecutor> left_;    // 左儿子节点（需要join的表）
    std::unique_ptr<AbstractExecutor> right_;   // 右儿子节点（需要join的表）
    size_t len_;                                // join后获得的每条记录的长度
    std::vector<ColMeta> cols_;                 // join后获得的记录的字段

    std::vector<Condition> fed_conds_;          // join条件
    bool isend;

public:
    NestedLoopJoinExecutor(std::unique_ptr<AbstractExecutor> left, std::unique_ptr<AbstractExecutor> right,
                           std::vector<Condition> conds) {
        left_ = std::move(left);
        right_ = std::move(right);
        len_ = left_->tupleLen() + right_->tupleLen();
        cols_ = left_->cols();
        auto right_cols = right_->cols();
        for (auto &col : right_cols) {
            col.offset += left_->tupleLen();
        }

        cols_.insert(cols_.end(), right_cols.begin(), right_cols.end());
        isend = false;
        fed_conds_ = std::move(conds);

    }

    std::vector<ColMeta> &cols() {
        return cols_;
    };

    bool is_end() const override {
        return isend;
    };

    size_t tupleLen() const {
        return len_;
    };

    std::string getType() {
        return "NestedLoopJoinExecutor";
    };

    //
    void beginTuple() override {
        left_->beginTuple();
        right_->beginTuple();
        //
        find_next_valid_tuple();
    }

    void nextTuple() override {
        assert(!is_end());
        // 移动一位
        if (right_->is_end()) {
            left_->nextTuple();
            right_->beginTuple();
        } else {
            right_->nextTuple();
        }
        // right_->nextTuple();
        //  找到下一个合法的tuple
        find_next_valid_tuple();
    }

    std::unique_ptr<RmRecord> Next() override {
        assert(!is_end());
        // 取left_record和right_record
        auto left_record = left_->Next();
        auto right_record = right_->Next();
        // 合并到一起
        auto ret = std::make_unique<RmRecord>(len_);
        memcpy(ret->data, left_record->data, left_record->size);
        memcpy(ret->data + left_record->size, right_record->data, right_record->size);
        return ret;
    }

    Rid &rid() override {
        return _abstract_rid;
    }

private:

    // 找到下一个符合fed_cond的tuple
    void find_next_valid_tuple() {
        assert(!is_end());
        while (!left_->is_end()) {
            // 取两边的record
            auto left_record = left_->Next();
            while (!right_->is_end()) {
                auto right_record = right_->Next();
                // 检查是否符合fed_cond
                bool is_fit = true;
                for (auto cond : fed_conds_) {
                    // 取left value

                    auto left_cols = left_->cols();
                    auto left_col = *(left_->get_col(left_cols, cond.lhs_col));
                    auto left_value = fetch_value(left_record, left_col);

                    // 取right value
                    Value right_value;
                    if (cond.is_rhs_val) {
                        right_value = cond.rhs_val;
                    } else {
                        auto right_cols = right_->cols();
                        auto right_col = *(right_->get_col(right_cols, cond.rhs_col));
                        right_value = fetch_value(right_record, right_col);
                    }

                    // 比较是否符合条件
                    if (!compare_value(left_value, right_value, cond.op)) {
                        is_fit = false;
                        break;
                    }
                }
                if (is_fit) {
                    return ;
                } else {
                    right_->nextTuple();
                }
            }
            left_->nextTuple();
            right_->beginTuple();
        }
        isend = true;
    }
};

