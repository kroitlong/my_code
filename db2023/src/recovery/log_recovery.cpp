/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "log_recovery.h"

/**
 * @description: analyze阶段，需要获得脏页表（DPT）和未完成的事务列表（ATT）
 */
void RecoveryManager::analyze() {
    /*
    // 扫描日志文件获得最后一个持久化的LSN
    lsn_t durable_lsn = disk_manager_->GetLastDurableLsn();

    // 根据脏页表获得所有脏页
    std::unordered_set<Page *> dirty_pages = buffer_pool_manager_->dirty_pages_;

    // 遍历日志文件,比durable_lsn大的都是要重做的
    while (true) {
    LogRecord *log = disk_manager_->ReadLog();
    if (log->lsn_ > durable_lsn) {
        redo_logs_.push_back(log);
    }
    if (log->lsn_ == durable_lsn) {
        break;
    }
    }

    // 获得未完成事务列表
    std::unordered_set<txn_id_t> uncommitted_txns = GetUncommittedTxns();
    */
}

/**
 * @description: 重做所有未落盘的操作
 */
void RecoveryManager::redo() {
    /*
    for (auto page : dirty_pages_) {
    for (auto lsn : page->redo_lsns_) {
        LogRecord *log = GetLogRecord(lsn);
        Redo(page, log);
    }
    }
    */
}

/**
 * @description: 回滚未完成的事务
 */
void RecoveryManager::undo() {
    /*
    // 按照事务开始时间逆序遍历未完成事务
    for (auto txn_id : uncommitted_txns_) {

    // 遍历事务中的每个日志记录
    for (auto lsn : GetTransactionLogs(txn_id)) {
        LogRecord *log = GetLogRecord(lsn);
        Undo(log);
    }
    }
    */
}