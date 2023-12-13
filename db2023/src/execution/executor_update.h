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

class UpdateExecutor : public AbstractExecutor {
   private:
    TabMeta tab_;
    std::vector<Condition> conds_;
    RmFileHandle *fh_;
    std::vector<Rid> rids_;
    std::string tab_name_;
    std::vector<SetClause> set_clauses_;
    SmManager *sm_manager_;

   public:
    UpdateExecutor(SmManager *sm_manager, const std::string &tab_name, std::vector<SetClause> set_clauses,
                   std::vector<Condition> conds, std::vector<Rid> rids, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = tab_name;
        set_clauses_ = set_clauses;
        tab_ = sm_manager_->db_.get_table(tab_name);
        fh_ = sm_manager_->fhs_.at(tab_name).get();
        conds_ = conds;
        rids_ = rids;
        context_ = context;
    }
    
    std::unique_ptr<RmRecord> Next() override {
        for(auto &rid : rids_) {
            auto rec = fh_->get_record(rid, context_);
            // 删除所有的index，这是为了下一步更新做准备
            for(size_t i = 0; i < tab_.indexes.size(); ++i) {
                auto& index = tab_.indexes[i];
                auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                char key[index.col_tot_len];
                int offset = 0;
                for(size_t i = 0; i < index.col_num; ++i) {
                    memcpy(key + offset, rec->data + index.cols[i].offset, index.cols[i].len);
                    offset += index.cols[i].len;
                }
                ih->delete_entry(key, context_->txn_);
            }

            // 更新record file
            char new_rec[fh_->get_file_hdr().record_size];
            memcpy(new_rec, rec->data, fh_->get_file_hdr().record_size);
            for(auto &set_clause: set_clauses_) {
                auto lhs_col = tab_.get_col(set_clause.lhs.col_name);
                // 在内存中修改
                memcpy(new_rec + lhs_col->offset, set_clause.rhs.raw->data, lhs_col->len);
            }
            // 对磁盘修改，将新的record写入
            fh_->update_record(rid, new_rec, context_);

            try {
                // 将rid插入在内存中更新后的新的record的cols对应key的index
                for(size_t i = 0; i < tab_.indexes.size(); ++i) {
                    auto& index = tab_.indexes[i];
                    auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                    char key[index.col_tot_len];
                    int offset = 0;
                    for(size_t i = 0; i < index.col_num; ++i) {
                        memcpy(key + offset, new_rec + index.cols[i].offset, index.cols[i].len);
                        offset += index.cols[i].len;
                    }
                    ih->insert_entry(key,rid,context_->txn_);
                }
            }catch(InternalError &error) {
                // 恢复record
                fh_->update_record(rid, rec->data, context_);
                // 恢复索引
                // 恢复所有的index
                for(size_t i = 0; i < tab_.indexes.size(); ++i) {
                    auto& index = tab_.indexes[i];
                    auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                    char key[index.col_tot_len];
                    int offset = 0;
                    for(size_t i = 0; i < index.col_num; ++i) {
                        memcpy(key + offset, rec->data + index.cols[i].offset, index.cols[i].len);
                        offset += index.cols[i].len;
                    }
                    ih->insert_entry(key, rid, context_->txn_);
                }

                // 继续抛出异常
                throw InternalError("Non-unique index!");
            }

        }
        return nullptr;
    }
    Rid &rid() override { return _abstract_rid; }
};