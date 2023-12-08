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

using namespace std;

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
        //同一字段最多最多匹配两个判断条件
        
        /*
 
        */
        
        
        //设置左边界
        int start_offset = 0;
        for (std::string index_col_name : index_col_names) {
            Value rhs_value;
            Condition find_cond;
            ColMeta index_col;
            int match = 0;
            //搜索条件中是否含有该索引字段
            for (auto &cond : conds_) {
                //如果有
                if (cond.is_rhs_val && cond.lhs_col.tab_name == tab_name_ && cond.lhs_col.col_name == index_col_name) {
                
                    rhs_value = cond.rhs_val;
                    find_cond = cond;
                    index_col = *get_col(index_meta_.cols, find_cond.lhs_col);
                    
                    //buf保存比较条件的右值
                    char *buf = new char[index_col.len+1];
                    memset(buf, 0, index_col.len+1);
                    
                    if (find_cond.op == OP_EQ || find_cond.op == OP_GT || find_cond.op == OP_GE)
                    {
                        match = 1;
                    }
                    
                    //如果判断类型不属于那三个，搜索下一个条件
                    else
                    {
                        continue;
                    }
                    
                    switch (rhs_value.type) 
                    {
                    case TYPE_INT:
                        {
                          //buf = (char *)&rhs_value.int_val;
                          for (int i = 0; i < 4; i++)
	                 {
		            buf[i] = (rhs_value.int_val >> (8 * i)) & 0xff;
	                 }
                         break;
                        }
                    case TYPE_FLOAT:
                       {
                         char* tem;
                         tem = (char *)&rhs_value.float_val;
                         for(int i=0; i<4; i++)
                         {
                           buf[i] = tem[i];
                         }
                         break;
                       }
                    case TYPE_STRING:
                        {
                          //buf = rhs_value.str_val.c_str(); 
                          rhs_value.str_val.copy(buf, rhs_value.str_val.size());
                          break;
                        }
                        
                   /* case TYPE_DATETIME: {
                        std::memcpy(const_cast<char *>(buf), rhs_value.datetime_val.time,       sizeof(rhs_value.datetime_val.time));
                    }
                    case TYPE_BIGINT:
                        buf = (char *)&rhs_value.bigint_val;
                        */
                    default:
                        break;
                    }
                    
                    //确定扫描上界
                      
                    memcpy(key_start + start_offset, buf, index_col.len);
                    start_initialized = true; 
                    start_offset += index_col.len;  
                    break;     
                }
            }
            
            if(match == 0)
            break;         
        }
        
        
        //设置右边界
        int end_offset = 0;
        int last_type;
        int last_offset = 0;
        for (std::string index_col_name : index_col_names) {
            Value rhs_value;
            Condition find_cond;
            ColMeta index_col;
            int match = 0;
            //搜索条件中是否含有该索引字段
            for (auto &cond : conds_) {
                //如果有
                if (cond.is_rhs_val && cond.lhs_col.tab_name == tab_name_ && cond.lhs_col.col_name == index_col_name) {
                
                    rhs_value = cond.rhs_val;
                    find_cond = cond;
                    index_col = *get_col(index_meta_.cols, find_cond.lhs_col);
                    
                    //buf保存比较条件的右值
                    char *buf = new char[index_col.len+1];
                    memset(buf, 0, index_col.len+1);
                    
                    if (find_cond.op == OP_EQ || find_cond.op == OP_LT || find_cond.op == OP_LE)
                    {
                        match = 1;
                    }
                    else
                    {
                        continue;
                    }
                    
                    switch (rhs_value.type) 
                    {
                      case TYPE_INT:
                      {
                        for (int i = 0; i < 4; i++)
	                 {
		            buf[i] = (rhs_value.int_val >> (8 * i)) & 0xff;
	                 }
                        break;
                      }
                      case TYPE_FLOAT:
                      {
                         char* tem;
                         tem = (char *)&rhs_value.float_val;
                         for(int i=0; i<4; i++)
                         {
                           buf[i] = tem[i];
                         }
                        buf = (char *)&rhs_value.float_val;
                        break;
                      }
                      case TYPE_STRING:
                      {
                        //buf = rhs_value.str_val.c_str(); 
                        rhs_value.str_val.copy(buf, rhs_value.str_val.size());
                        break;     
                      }
                    }
                    
                   
                    memcpy(key_end + end_offset, buf, index_col.len);
                    end_initialized = true;
                    last_offset = end_offset; 
                    last_type = rhs_value.type;
                    end_offset += index_col.len;  
                    
                 /*   case TYPE_DATETIME: {
                        std::memcpy(const_cast<char *>(buf), rhs_value.datetime_val.time,       sizeof(rhs_value.datetime_val.time));
                    }
                    case TYPE_BIGINT:
                        buf = (char *)&rhs_value.bigint_val;
                    default:
                        break;
                 */
                 
                }
                
                break;
              }
              
              if(match == 0)
              break;
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
            //使key_end最后一个字段的值略增大一点
            switch (last_type) 
                    {
                      case TYPE_INT:
                      {
                        int* num = (int*)(key_end+last_offset);
                        (*num)++;
                        break;
                      }
                      case TYPE_FLOAT:
                      {
                        float* num = (float*)(key_end+last_offset);
                        (*num)++;
                        break;
                      }
                      case TYPE_STRING:
                      {
                        int len = strlen(key_end+last_offset);
                        *(key_end+last_offset+len-1)+=1;
                        break;     
                      }
                    }
        
            end_Iid = ix_index_handle->upper_bound(key_end);
        }
        // 将过滤后的条件设置为与原始条件相同
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
        // 创建IxScan对象，并设置起始和结束迭代器
        
        scan_ = std::make_unique<IxScan>(ix_index_handle,
                                         start_Iid,
                                         end_Iid,
                                         sm_manager_->get_bpm());
        // 在扫描范围内循环，直到找到满足条件的记录或扫描结束
       
        
        while (!scan_->is_end()) {
            rid_ = scan_->rid();
            auto rec = fh_->get_record(rid_, context_);
            /*
            try {
                // 获取记录并检查是否满足条件
                rec = fh_->get_record(rid_, context_);
            } catch (RecordNotFoundError &e) {
                std::cerr << e.what() << std::endl;
            }
            */

            //  用一个变量记录是否该rcd满足所有条件
            
            bool fed_all_conds = true;
            
            for(auto it = fed_conds_.begin();it!=fed_conds_.end();++it){
                if(!is_fed_cond(cols_,*it,rec.get())){
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
        // 移动到下一个记录并检查是否满足条件
        for (scan_->next(); !scan_->is_end(); scan_->next()) {
       
            auto rec = fh_->get_record(scan_->rid(), context_);
            
            bool fed_all_conds = true;
            
        //看是否满足所有条件     
            for(auto it = fed_conds_.begin();it!=fed_conds_.end();++it){
                if(!is_fed_cond(cols_,*it,rec.get())){
                   fed_all_conds = false;
                   break;
                }
            }
            
        // 如果满足所有条件，break并且将该算子现在指向的rid_标记为找到的record的rid
            if(fed_all_conds){
                rid_ = scan_->rid();
                break;
            }
           
        }
    }

    std::unique_ptr<RmRecord> Next() override {
        // 获取当前记录并返回
        //printf("page_no: %d, slot_no: %d\n", rid_.page_no, rid_.slot_no);
        return fh_->get_record(rid_, context_);
    }

    bool is_end() const override {
        // 检查扫描是否结束
        return scan_->is_end();
    }

    Rid &rid() override {
        return rid_;
    }
    
    const std::vector<ColMeta> &cols() const {
        return cols_;
    };
};    
