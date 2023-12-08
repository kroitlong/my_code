/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "transaction_manager.h"
#include "record/rm_file_handle.h"
#include "system/sm_manager.h"

std::unordered_map<txn_id_t, Transaction *> TransactionManager::txn_map = {};

/**
 * @description: 事务的开始方法
 * @return {Transaction*} 开始事务的指针
 * @param {Transaction*} txn 事务指针，空指针代表需要创建新事务，否则开始已有事务
 * @param {LogManager*} log_manager 日志管理器指针
 */
Transaction *TransactionManager::begin(Transaction *txn, LogManager *log_manager) {
    // Todo:
    // 1. 判断传入事务参数是否为空指针
    // 2. 如果为空指针，创建新事务
    // 3. 把开始事务加入到全局事务表中
    // 4. 返回当前事务指针
    if (txn == nullptr) {
        // 创建新事务
        txn = new Transaction(next_txn_id_++);
    }

    // 生成开始事务日志
    /*
     *暂时不涉及日志操作，且相关函数暂时未完善
    BeginLogRecord begin_log(txn->get_transaction_id());

    // 将开始日志写入日志缓冲区
    log_manager->add_log_to_buffer(&begin_log);

    // 获取lsn,记录在事务对象中
    lsn_t lsn = begin_log.lsn_;
    txn->set_prev_lsn(lsn);
    */
    std::lock_guard<std::mutex> lock(latch_);
    txn_map[txn->get_transaction_id()] = txn;


    return txn;
}

/**
 * @description: 事务的提交方法
 * @param {Transaction*} txn 需要提交的事务
 * @param {LogManager*} log_manager 日志管理器指针
 */
void TransactionManager::commit(Transaction *txn, LogManager *log_manager) {
    // Todo:
    // 1. 如果存在未提交的写操作，提交所有的写操作
    // 2. 释放所有锁
    // 3. 释放事务相关资源，eg.锁集
    // 4. 把事务日志刷入磁盘中
    // 5. 更新事务状态

    /* 暂时不涉及事务的提交
    // 提交写操作
    for (auto write_record : *txn->get_write_set()) {
        // 根据write_record类型调用底层页面的插入/删除/更新方法提交操作
    }

    // 释放锁
    auto lock_set = txn->get_lock_set();
    for (auto lock_data_id : *lock_set) {
        lock_manager_->Release(lock_data_id);
    }

    // 清空资源
    txn->get_write_set()->clear();
    txn->get_lock_set()->clear();

    // 刷日志
    log_manager->FlushLog();
    txn->set_state(TransactionState::COMMITTED);
    */
}
/**

 * @description: 事务的终止（回滚）方法
 * @param {Transaction *} txn 需要回滚的事务
 * @param {LogManager} *log_manager 日志管理器指针
 */
void TransactionManager::abort(Transaction *txn, LogManager *log_manager) {
    // Todo:
    // 1. 回滚所有写操作
    // 2. 释放所有锁
    // 3. 清空事务相关资源，eg.锁集
    // 4. 把事务日志刷入磁盘中
    // 5. 更新事务状态

    /*暂时不涉及事务的回滚
    // 提交写操作
    for (auto write_record : *txn->get_write_set()) {
        // 根据write_record类型调用底层页面的插入/删除/更新方法提交操作
    }

    // 释放锁
    auto lock_set = txn->get_lock_set();
    for (auto lock_data_id : *lock_set) {
        lock_manager_->Release(lock_data_id);
    }

    // 清空资源
    txn->get_write_set()->clear();
    txn->get_lock_set()->clear();

    // 刷日志
    log_manager->FlushLog();
    txn->set_state(TransactionState::ABORTED);
    */
}