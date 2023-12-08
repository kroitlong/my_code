/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "storage/disk_manager.h"

#include <assert.h>    // for assert
#include <string.h>    // for memset
#include <sys/stat.h>  // for stat
#include <unistd.h>    // for lseek

#include "defs.h"

DiskManager::DiskManager() {
    memset(fd2pageno_, 0, MAX_FD * (sizeof(std::atomic<page_id_t>) / sizeof(char)));
}

/**
 * @description: 将数据写入文件的指定磁盘页面中
 * @param {int} fd 磁盘文件的文件句柄
 * @param {page_id_t} page_no 写入目标页面的page_id
 * @param {char} *offset 要写入磁盘的数据
 * @param {int} num_bytes 要写入磁盘的数据大小
 */
void DiskManager::write_page(int fd, page_id_t page_no, const char *offset, int num_bytes) {
    // Todo:
    // 1.lseek()定位到文件头，通过(fd,page_no)可以定位指定页面及其在磁盘文件中的偏移量
    // 2.调用write()函数
    // 注意write返回值与num_bytes不等时 throw InternalError("DiskManager::write_page Error");
    // 定位到指定页面的偏移量
    if (fd2path_.find(fd) == fd2path_.end()) {
        //std::cout << "Invalid FD, write cancel" << std::endl;
        return;
    }
    off_t offset_ = static_cast<off_t>(page_no) * PAGE_SIZE;
    //debug
    //std::cout << "fd2path[fd]:" << fd2path_[fd] << std::endl;
    //std::cout << "fd:" << fd << std::endl;
    //std::cout << "page_no:" << page_no << " offset:" << offset_ << std::endl;
    // 将文件偏移量设置为指定页面的位置
    if (lseek(fd, offset_, SEEK_SET) == -1) {
        throw std::runtime_error("w_Error: Failed to set file offset.");
    }
    // 写入数据到磁盘
    ssize_t bytes_written = write(fd, offset, num_bytes);
    if (bytes_written == -1) {
        throw std::runtime_error("Error: Failed to write data to disk.");
    }

    // 检查写入的字节数是否与预期的字节数相等
    if (bytes_written != num_bytes) {
        throw InternalError("DiskManager::write_page Error");
    }
}

/**
 * @description: 读取文件中指定编号的页面中的部分数据到内存中
 * @param {int} fd 磁盘文件的文件句柄
 * @param {page_id_t} page_no 指定的页面编号
 * @param {char} *offset 读取的内容写入到offset中
 * @param {int} num_bytes 读取的数据量大小
 */
void DiskManager::read_page(int fd, page_id_t page_no, char *offset, int num_bytes) {
    // Todo:
    // 1.lseek()定位到文件头，通过(fd,page_no)可以定位指定页面及其在磁盘文件中的偏移量
    // 2.调用read()函数
    // 注意read返回值与num_bytes不等时，throw InternalError("DiskManager::read_page Error");
    if (fd2path_.find(fd) == fd2path_.end()) {
        //std::cout << "Invalid FD, write cancel" << std::endl;
        return;
    }
    off_t offset_ = static_cast<off_t>(page_no) * PAGE_SIZE;
    // 将文件偏移量设置为指定页面的位置
    if (lseek(fd, offset_, SEEK_SET) == -1) {
        throw std::runtime_error("r_Error: Failed to set file offset.");
    }
    // 从磁盘读取数据
    ssize_t bytes_readed = read(fd, offset, num_bytes);
    if (bytes_readed == -1) {
        throw std::runtime_error("Error: Failed to read data to disk.");
    }

    // 检查读取的字节数是否与预期的字节数相等
    if (bytes_readed != num_bytes) {
        throw InternalError("DiskManager::read_page Error");
    }
}

/**
 * @description: 分配一个新的页号
 * @return {page_id_t} 分配的新页号
 * @param {int} fd 指定文件的文件句柄
 */
page_id_t DiskManager::allocate_page(int fd) {
    // 简单的自增分配策略，指定文件的页面编号加1
    assert(fd >= 0 && fd < MAX_FD);
    return fd2pageno_[fd]++;
}

void DiskManager::deallocate_page(__attribute__((unused)) page_id_t page_id) {}

bool DiskManager::is_dir(const std::string &path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

void DiskManager::create_dir(const std::string &path) {
    // Create a subdirectory
    std::string cmd = "mkdir " + path;
    if (system(cmd.c_str()) < 0) {  // 创建一个名为path的目录
        throw UnixError();
    }
}

void DiskManager::destroy_dir(const std::string &path) {
    std::string cmd = "rm -r " + path;
    if (system(cmd.c_str()) < 0) {
        throw UnixError();
    }
}

/**
 * @description: 判断指定路径文件是否存在
 * @return {bool} 若指定路径文件存在则返回true
 * @param {string} &path 指定路径文件
 */
bool DiskManager::is_file(const std::string &path) {
    // 用struct stat获取文件信息
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

/**
 * @description: 用于创建指定路径文件
 * @return {*}
 * @param {string} &path
 */
void DiskManager::create_file(const std::string &path) {
    // Todo:
    // 调用open()函数，使用O_CREAT模式
    // 注意不能重复创建相同文件
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        if (errno == EEXIST) {
            throw FileExistsError(path);
        } else {
            printf("Failed to create file.\n");
            //std::cerr << "Failed to create file. Error code: " << errno << std::endl;
        }
    } else {
        close(fd);
        //std::cout << "File created successfully." << std::endl;
    }
}

/**
 * @description: 删除指定路径的文件
 * @param {string} &path 文件所在路径
 */
void DiskManager::destroy_file(const std::string &path) {
    // Todo:
    // 调用unlink()函数
    // 注意不能删除未关闭的文件
    // 检查文件是否已打开
    if (path2fd_.count(path) > 0) {
        //std::cout << "File is open" << std::endl;
        printf("File is open\n");
    }
    // 调用 unlink() 函数删除文件
    int result = unlink(path.c_str());
    if (result == 0) {
        //std::cout << "File deleted successfully." << std::endl;
        printf("File deleted successfully.\n");
    } else {
        throw FileNotFoundError(path);
    }
}


/**
 * @description: 打开指定路径文件
 * @return {int} 返回打开的文件的文件句柄
 * @param {string} &path 文件所在路径
 */
int DiskManager::open_file(const std::string &path) {
    // Todo:
    // 调用open()函数，使用O_RDWR模式
    // 注意不能重复打开相同文件，并且需要更新文件打开列表
    // 检查文件是否已经打开
    if (path2fd_.find(path) != path2fd_.end()) {
        throw FileNotClosedError(path);
    }
    int fd = open(path.c_str(), O_RDWR);
    if (fd == -1) {
        //std::cerr << "Failed to open file: " << path << std::endl;
        throw FileNotFoundError(path);
    } else {
        //do nothing
    }
    //debug
    //std::cout << "fd:" << fd << std::endl;
    //std::cout << "path:" << path << std::endl;
    // 将打开的文件添加到文件打开列表
    path2fd_.insert(std::make_pair(path, fd));
    fd2path_.insert(std::make_pair(fd, path));
    //std::cout << "fd2path[fd]:" << fd2path_[fd] << std::endl;
    return fd;
}

/**
 * @description:用于关闭指定路径文件
 * @param {int} fd 打开的文件的文件句柄
 */
void DiskManager::close_file(int fd) {
    // Todo:
    // 调用close()函数
    // 注意不能关闭未打开的文件，并且需要更新文件打开列表
    if (fd2path_.count(fd) <= 0) {
        throw FileNotOpenError(fd);
    }
    int res = close(fd);
    if (res == -1) {
        throw FileNotClosedError(fd2path_.find(fd)->second);
    }
    auto iter = fd2path_.find(fd);
    std::string path = iter->second;
    //更新文件打开列表
    fd2path_.erase(fd);
    path2fd_.erase(path);
}


/**
 * @description: 获得文件的大小
 * @return {int} 文件的大小
 * @param {string} &file_name 文件名
 */
int DiskManager::get_file_size(const std::string &file_name) {
    struct stat stat_buf;
    int rc = stat(file_name.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

/**
 * @description: 根据文件句柄获得文件名
 * @return {string} 文件句柄对应文件的文件名
 * @param {int} fd 文件句柄
 */
std::string DiskManager::get_file_name(int fd) {
    if (!fd2path_.count(fd)) {
        throw FileNotOpenError(fd);
    }
    return fd2path_[fd];
}

/**
 * @description:  获得文件名对应的文件句柄
 * @return {int} 文件句柄
 * @param {string} &file_name 文件名
 */
int DiskManager::get_file_fd(const std::string &file_name) {
    if (!path2fd_.count(file_name)) {
        return open_file(file_name);
    }
    return path2fd_[file_name];
}


/**
 * @description:  读取日志文件内容
 * @return {int} 返回读取的数据量，若为-1说明读取数据的起始位置超过了文件大小
 * @param {char} *log_data 读取内容到log_data中
 * @param {int} size 读取的数据量大小
 * @param {int} offset 读取的内容在文件中的位置
 */
int DiskManager::read_log(char *log_data, int size, int offset) {
    // read log file from the previous end
    if (log_fd_ == -1) {
        log_fd_ = open_file(LOG_FILE_NAME);
    }
    int file_size = get_file_size(LOG_FILE_NAME);
    if (offset > file_size) {
        return -1;
    }

    size = std::min(size, file_size - offset);
    if (size == 0) return 0;
    lseek(log_fd_, offset, SEEK_SET);
    ssize_t bytes_read = read(log_fd_, log_data, size);
    assert(bytes_read == size);
    return bytes_read;
}


/**
 * @description: 写日志内容
 * @param {char} *log_data 要写入的日志内容
 * @param {int} size 要写入的内容大小
 */
void DiskManager::write_log(char *log_data, int size) {
    if (log_fd_ == -1) {
        log_fd_ = open_file(LOG_FILE_NAME);
    }

    // write from the file_end
    lseek(log_fd_, 0, SEEK_END);
    ssize_t bytes_write = write(log_fd_, log_data, size);
    if (bytes_write != size) {
        throw UnixError();
    }
}
