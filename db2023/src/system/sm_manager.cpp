/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sm_manager.h"

#include <sys/stat.h>
#include <unistd.h>

#include <fstream>

#include "index/ix.h"
#include "record/rm.h"
#include "record_printer.h"

/**
 * @description: 判断是否为一个文件夹
 * @return {bool} 返回是否为一个文件夹
 * @param {string&} db_name 数据库文件名称，与文件夹同名
 */
bool SmManager::is_dir(const std::string &db_name) {
    struct stat st;
    return stat(db_name.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

/**
 * @description: 创建数据库，所有的数据库相关文件都放在数据库同名文件夹下
 * @param {string&} db_name 数据库名称
 */
void SmManager::create_db(const std::string &db_name) {
    if (is_dir(db_name)) {
        throw DatabaseExistsError(db_name);
    }
    //为数据库创建一个子目录
    std::string cmd = "mkdir " + db_name;
    if (system(cmd.c_str()) < 0) {  // 创建一个名为db_name的目录
        throw UnixError();
    }
    if (chdir(db_name.c_str()) < 0) {  // 进入名为db_name的目录
        throw UnixError();
    }
    //创建系统目录
    DbMeta *new_db = new DbMeta();
    new_db->name_ = db_name;

    // 注意，此处ofstream会在当前目录创建(如果没有此文件先创建)和打开一个名为DB_META_NAME的文件
    std::ofstream ofs(DB_META_NAME);

    // 将new_db中的信息，按照定义好的operator<<操作符，写入到ofs打开的DB_META_NAME文件中
    ofs << *new_db;  // 注意：此处重载了操作符<<

    delete new_db;

    // 创建日志文件
    disk_manager_->create_file(LOG_FILE_NAME);
    // 回到根目录
    if (chdir("..") < 0) {
        throw UnixError();
    }
}

/**
 * @description: 删除数据库，同时需要清空相关文件以及数据库同名文件夹
 * @param {string&} db_name 数据库名称，与文件夹同名
 */
void SmManager::drop_db(const std::string &db_name) {
    if (!is_dir(db_name)) {
        throw DatabaseNotFoundError(db_name);
    }
    std::string cmd = "rm -r " + db_name;
    if (system(cmd.c_str()) < 0) {
        throw UnixError();
    }
}

/**
 * @description: 打开数据库，找到数据库对应的文件夹，并加载数据库元数据和相关文件
 * @param {string&} db_name 数据库名称，与文件夹同名
 */
void SmManager::open_db(const std::string &db_name) {
    if (!is_dir(db_name)) {
        throw DatabaseNotFoundError(db_name);
    }
    //进入数据库文件目录
    std::string db_path = "./" + db_name;
    if (chdir(db_path.c_str()) < 0) {
        throw UnixError();
    }
    // 加载数据库元数据，每个数据库文件夹下都有一个.meta文件存放数据库的元数据
    std::ifstream meta_file(DB_META_NAME);
    //用.meta文件初始化DBmeta元数据管理类
    meta_file >> db_;
    if (db_.tabs_.size() != 0) {
        //初始化数据库表元数据
        for (auto tabs : db_.tabs_) {
            TabMeta tab = tabs.second;
            std::string tab_name = tab.name;
            fhs_.emplace(tab_name, rm_manager_->open_file(tab_name));
        }
        //TODO初始化数据库表索引文件
        for (auto &tab_meta : db_.tabs_) {
            for (auto &index_meta : tab_meta.second.indexes) {
                // 打开索引文件
                std::string index_filename = ix_manager_->get_index_name(index_meta.tab_name, index_meta.cols);
                ihs_.emplace(index_filename, ix_manager_->open_index(index_meta.tab_name, index_meta.cols));
            }
        }
    }
}

/**
 * @description: 把数据库相关的元数据刷入磁盘中
 */
void SmManager::flush_meta() {
    // 默认清空文件
    std::ofstream ofs(DB_META_NAME);
    ofs << db_;
}

/**
 * @description: 关闭数据库并把数据落盘
 */
void SmManager::close_db() {
    flush_meta();
    fhs_.clear();
    ihs_.clear();
    db_ = DbMeta();
}

/**
 * @description: 显示所有的表,通过测试需要将其结果写入到output.txt,详情看题目文档
 * @param {Context*} context
 */
void SmManager::show_tables(Context *context) {
    std::fstream outfile;
    outfile.open("output.txt", std::ios::out | std::ios::app);
    outfile << "| Tables |\n";
    RecordPrinter printer(1);
    printer.print_separator(context);
    printer.print_record({"Tables"}, context);
    printer.print_separator(context);
    for (auto &entry : db_.tabs_) {
        auto &tab = entry.second;
        printer.print_record({tab.name}, context);
        outfile << "| " << tab.name << " |\n";
    }
    printer.print_separator(context);
    outfile.close();
}

/**
 * @description: 显示该表的所有索引项
 * @param {tab_name} 要显示的表的表名
 * @param {Context*} context
 */
void SmManager::show_index(std::string tab_name, Context *context) {
    // 打开输出文件
    std::fstream outfile;
    outfile.open("output.txt", std::ios::out | std::ios::app);
    // 获取表元数据
    TabMeta &tab = db_.get_table(tab_name);
    std::string title = "Table: " + tab_name + " Index";
    RecordPrinter printer(1);
    printer.print_separator(context);
    printer.print_record({title}, context);
    printer.print_separator(context);
    // 打印每一个索引
    for (auto &index : tab.indexes) {
        std::string index_name = index.cols[0].name;
        for (auto i = 1; i < index.cols.size(); ++i) {
            index_name += "," + index.cols[i].name;
        }
        // 打印到输出
        std::string record_str = tab_name + " | unique | (" + index_name + ")";
        printer.print_record({record_str}, context);
        outfile << "| " << record_str << " |\n";
        printer.print_separator(context);
    }
    // 关闭文件
    outfile.close();
}

/**
 * @description: 显示表的元数据
 * @param {string&} tab_name 表名称
 * @param {Context*} context
 */
void SmManager::desc_table(const std::string &tab_name, Context *context) {
    TabMeta &tab = db_.get_table(tab_name);

    std::vector<std::string> captions = {"Field", "Type", "Index"};
    RecordPrinter printer(captions.size());
    // Print header
    printer.print_separator(context);
    printer.print_record(captions, context);
    printer.print_separator(context);
    // Print fields
    for (auto &col : tab.cols) {
        std::vector<std::string> field_info = {col.name, coltype2str(col.type), col.index ? "YES" : "NO"};
        printer.print_record(field_info, context);
    }
    // Print footer
    printer.print_separator(context);
}

/**
 * @description: 创建表
 * @param {string&} tab_name 表的名称
 * @param {vector<ColDef>&} col_defs 表的字段
 * @param {Context*} context
 */
void SmManager::create_table(const std::string &tab_name, const std::vector<ColDef> &col_defs, Context *context) {
    if (db_.is_table(tab_name)) {
        throw TableExistsError(tab_name);
    }
    // Create table meta
    int curr_offset = 0;
    TabMeta tab;
    tab.name = tab_name;
    for (auto &col_def : col_defs) {
        ColMeta col = {.tab_name = tab_name,
                       .name = col_def.name,
                       .type = col_def.type,
                       .len = col_def.len,
                       .offset = curr_offset,
                       .index = false,
                      };
        curr_offset += col_def.len;
        tab.cols.push_back(col);
    }
    // Create & open record file
    int record_size = curr_offset;  // record_size就是col meta所占的大小（表的元数据也是以记录的形式进行存储的）
    rm_manager_->create_file(tab_name, record_size);
    db_.tabs_[tab_name] = tab;
    // fhs_[tab_name] = rm_manager_->open_file(tab_name);
    fhs_.emplace(tab_name, rm_manager_->open_file(tab_name));

    flush_meta();
}

/**
 * @description: 删除表
 * @param {string&} tab_name 表的名称
 * @param {Context*} context
 */
void SmManager::drop_table(const std::string &tab_name, Context *context) {
//注：当前函数不会检查是否存在依赖于该表的索引文件，而是直接级联删除该表的索引文件
    // 检查表是否存在
    if (!db_.is_table(tab_name)) {
        throw TableNotFoundError(tab_name);
    }
    // 获取表元数据
    TabMeta &tab_meta = db_.get_table(tab_name);
    // 删除表数据文件
    auto file_handle_iter = fhs_.find(tab_name);
    if (file_handle_iter != fhs_.end()) {
        fhs_.erase(file_handle_iter);
        rm_manager_->destroy_file(tab_name);
    }
    // 删除表索引文件
    for (const IndexMeta &index_meta : tab_meta.indexes) {
        //注意此处的索引文件的命名格式
        std::string index_file_name = tab_name + "." + std::to_string(index_meta.col_tot_len) + ".ix";
        auto index_handle_iter = ihs_.find(index_file_name);
        if (index_handle_iter != ihs_.end()) {
            ihs_.erase(index_handle_iter);
            rm_manager_->destroy_file(index_file_name);
        }
    }
    // 删除表元数据
    db_.tabs_.erase(tab_name);
    // 在上下文中记录日志或进行其他必要的操作
    if (context != nullptr) {
        // TODO记录记录日志或执行其他操作

    }
    //删除表后要刷新元数据文件到磁盘
}


/**
 * @description: 创建索引
 * @param {string&} tab_name 表的名称
 * @param {vector<string>&} col_names 索引包含的字段名称
 * @param {Context*} context
 */
void SmManager::create_index(const std::string &tab_name, const std::vector<std::string> &col_names, Context *context) {
    // 1. 检查表是否存在
    if (!db_.is_table(tab_name)) {
        throw TableNotFoundError(tab_name);
    }
    // 2. 检查索引是否已存在
    if (db_.get_table(tab_name).is_index(col_names)) {
        throw IndexExistsError(tab_name, col_names);
    }
    // 3. 获取字段元数据
    TabMeta &tab_meta = db_.get_table(tab_name);
    std::vector<ColMeta> index_cols;
    for (auto &col_name : col_names) {
        tab_meta.get_col(col_name)->index = 1;;
        index_cols.push_back(*tab_meta.get_col(col_name));
    }
    // 4. 创建索引文件
    ix_manager_->create_index(tab_name, index_cols);
    // 5. 更新表中的索引元数据
    int index_len = 0;
    for (auto &col : index_cols) {
        index_len += col.len;
    }
    IndexMeta index_meta;
    index_meta.tab_name = tab_name;
    index_meta.col_num = index_cols.size();
    index_meta.col_tot_len = index_len;
    index_meta.cols = index_cols;
    std::string index_filename = ix_manager_->get_index_name(tab_name, index_meta.cols);
    ihs_.emplace(index_filename, ix_manager_->open_index(tab_name, index_meta.cols));
    tab_meta.indexes.push_back(index_meta);
    // 6. 在Context中记录日志等
    // TODO
    flush_meta();
}

/**
 * @description: 删除索引
 * @param {string&} tab_name 表名称
 * @param {vector<string>&} col_names 索引包含的字段名称
 * @param {Context*} context
 */
void SmManager::drop_index(const std::string &tab_name, const std::vector<std::string> &col_names, Context *context) {
    // 1. 检查索引是否存在
    auto &tab_meta = db_.get_table(tab_name);
    if (tab_meta.is_index(col_names) == false) {
        throw IndexNotFoundError(tab_name, col_names);
    }
    // 2. 关闭索引句柄
    auto index = tab_meta.get_index_meta(col_names);
    std::string index_filename = ix_manager_->get_index_name(tab_name, index->cols);
    auto ih = ihs_[index_filename].get();
    ihs_.erase(index_filename);
    ix_manager_->close_index(ih);
    // 3. 销毁索引文件
    ix_manager_->destroy_index(tab_name, col_names);
    tab_meta.indexes.erase(index);
    flush_meta();
    // 4. 其他日志、回滚相关记录
    // TODO
}

/**
 * @description: 删除索引
 * @param {string&} tab_name 表名称
 * @param {vector<ColMeta>&} 索引包含的字段元数据
 * @param {Context*} context
 */
void SmManager::drop_index(const std::string &tab_name, const std::vector<ColMeta> &cols, Context *context) {
    // 1. 检查索引是否存在
    auto &tab_meta = db_.get_table(tab_name);
    auto index_it = tab_meta.indexes.begin();
    for (; index_it != tab_meta.indexes.end(); ++index_it) {
        if (index_it->cols == cols) {
            break;
        }
    }
    if (index_it == tab_meta.indexes.end()) {
        std::vector<std::string> col_names;
        for (auto i = 0; i < cols.size(); ++i) {
            col_names.push_back(cols[i].name);
        }
        throw IndexNotFoundError(tab_name, col_names);
    }
    // 2. 关闭索引句柄
    std::string index_filename = ix_manager_->get_index_name(tab_name, index_it->cols);
    auto ih = ihs_[index_filename].get();
    ihs_.erase(index_filename);
    ix_manager_->close_index(ih);
    // 3. 销毁索引文件
    ix_manager_->destroy_index(tab_name, cols);
    tab_meta.indexes.erase(index_it);
    flush_meta();
    // 4. 其他日志、回滚相关记录
    // TODO
}