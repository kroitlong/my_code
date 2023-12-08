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

class SeqScanExecutor : public AbstractExecutor {
   private:
    std::string tab_name_;              // 表的名称
    std::vector<Condition> conds_;      // scan的条件
    RmFileHandle *fh_;                  // 表的数据文件句柄
    std::vector<ColMeta> cols_;         // scan后生成的记录的字段
    size_t len_;                        // scan后生成的每条记录的长度
    std::vector<Condition> fed_conds_;  // 同conds_，两个字段相同

    Rid rid_;
    std::unique_ptr<RecScan> scan_;     // table_iterator

    SmManager *sm_manager_;

   public:
    SeqScanExecutor(SmManager *sm_manager, std::string tab_name, std::vector<Condition> conds, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = std::move(tab_name);
        conds_ = std::move(conds);
        TabMeta &tab = sm_manager_->db_.get_table(tab_name_);
        fh_ = sm_manager_->fhs_.at(tab_name_).get();
        cols_ = tab.cols;
        len_ = cols_.back().offset + cols_.back().len;

        context_ = context;

        fed_conds_ = conds_;
    }

    // 判断一个col是否满足指定条件
    bool is_fed_cond(const std::vector<ColMeta> &rec_cols,const Condition &cond,const RmRecord *target){
        //  获取左操作数的colMet
        auto lhs_col = cond.lhs_col;
        auto lhs_col_meta = *get_col(rec_cols,lhs_col);

        //  如果右操作数是Col类型，获取右操作数的ColMeta
        ColMeta rhs_col_meta;
        if(!cond.is_rhs_val){
            //  如果是col类型的
            auto rhs_col = cond.rhs_col;
            rhs_col_meta = *get_col(rec_cols,rhs_col);
        }
        
        //  比较lhs和rhs的值
        auto rcd_ptr = std::make_unique<RmRecord>(*target);
        auto lhs_val = fetch_value(rcd_ptr,lhs_col_meta);
        Value rhs_val;
        if(cond.is_rhs_val){
            rhs_val = cond.rhs_val;
        }else{
            rhs_val = fetch_value(rcd_ptr,rhs_col_meta);
        }
        return compare_value(lhs_val,rhs_val,cond.op);
    }

    void beginTuple() override {
        // 获取一个RmScan对象的指针,赋值给算子的变量scan_
        scan_ = std::make_unique<RmScan>(fh_);
        
        //  用seq_scan来对表中的所有非空闲字段进行遍历，逐个判断是否满足所有条件
        while(!scan_->is_end()){
            auto rcd = fh_->get_record(scan_->rid(),context_);
            
            //  用一个变量记录是否该rcd满足所有条件
            bool fed_all_conds = true;
            for(auto it = fed_conds_.begin();it!=fed_conds_.end();++it){
                if(!is_fed_cond(cols_,*it,rcd.get())){
                   fed_all_conds = false;
                   break;
                }
            }
            //  如果不满足所有条件，RmScan遍历下一个record
            if(!fed_all_conds){
                scan_->next();
            }else{
            // 如果满足所有条件，break并且将该算子现在指向的rid_标记为找到的record的rid
                rid_ = scan_->rid();
                break;
            }
        }
    }

    void nextTuple() override {
        assert(!is_end());
        // 继续查询下一个满足conds的record
        for (scan_->next(); !scan_->is_end(); scan_->next()) {
            auto rcd = fh_->get_record(scan_->rid(),context_);
            
            //  用一个变量记录是否该rcd满足所有条件
            bool fed_all_conds = true;
            
            for(auto it = fed_conds_.begin();it!=fed_conds_.end();++it){
                if(!is_fed_cond(cols_,*it,rcd.get())){
                   fed_all_conds = false;
                   break;
                }
            }
           

            //  如果满足所有条件，将当前扫描的RmScan的rid赋值给算子的rid，并break
            if(fed_all_conds){
                rid_ = scan_->rid();
                break;
            }
        }
    }

    std::unique_ptr<RmRecord> Next() override {
        return fh_->get_record(rid_,context_);
    }

    Rid &rid() override { return rid_; }

    bool is_end() const override { return scan_->is_end(); }

    const std::vector<ColMeta> &cols() const override { return cols_; }

    size_t tupleLen() const { return len_; };
};
