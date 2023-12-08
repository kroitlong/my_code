/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include <cstring>
#include "log_manager.h"

/**
 * @description: 添加日志记录到日志缓冲区中，并返回日志记录号
 * @param {LogRecord*} log_record 要写入缓冲区的日志记录
 * @return {lsn_t} 返回该日志的日志记录号
 */
lsn_t LogManager::add_log_to_buffer(LogRecord *log_record) {
    /*
    // 获取全局lsn并赋值给日志记录
    lsn_t lsn = global_lsn_++;
    log_record->lsn_ = lsn;

    // 计算日志记录大小
    uint32_t log_size = log_record->size();

    // 加锁写入日志缓冲区
    std::lock_guard<std::mutex> lock(latch_);

    // 检查是否可以写入
    if (log_buffer_.is_full(log_size)) {
    // 缓冲区满了,需要先刷盘
    flush_log_to_disk();
    }

    // 将日志写入缓冲区
    char *dest = log_buffer_.buffer_ + log_buffer_.offset_;
    log_record->serialize(dest);

    // 更新偏移量
    log_buffer_.offset_ += log_size;

    return lsn;
    */
}

/**
 * @description: 把日志缓冲区的内容刷到磁盘中，由于目前只设置了一个缓冲区，因此需要阻塞其他日志操作
 */
void LogManager::flush_log_to_disk() {
    /*
    // 加锁
    std::lock_guard<std::mutex> lock(latch_);

    // 刷盘
    disk_manager_->WriteLog(log_buffer_.buffer_, log_buffer_.offset_);

    // 更新持久化LSN
    persist_lsn_ = global_lsn_.load();

    // 重置缓冲区
    log_buffer_.offset_ = 0;
    memset(log_buffer_.buffer_, 0, LOG_BUFFER_SIZE);
    */
}
