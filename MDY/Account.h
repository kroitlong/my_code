
#pragma once

#include<iostream>
#include<string>
#include<sstream>
#include<map>
#include<fstream>
#include<dirent.h>
#include <stdlib.h>
#include <vector>
#include "Dlist.h"
#include "MDYS.h"
#include "DataBase.h"
using namespace std;

fstream Aofile;
fstream Aifile;

class account_t {

private:
    //下面函数用于将txt文件的内容写入hash表中，逐行写入，账户密码分开写入,同时建立起对应的链表
    void write() {
        string data_t;
        string dir_t;
        string txt_dir;
        vector<string> vec;
        getline(Aifile, data_t, ' ');
        getline(Aifile, dir_t, ' ');
        getline(Aifile, txt_dir, '\n');
        database db(data_t, dir_t, txt_dir);
        vec.emplace(vec.begin(), dir_t);
        vec.emplace(vec.begin() + 1, txt_dir);
        database_list.push_front(db);
        database_map.insert(make_pair(data_t, vec));

        //测试通过
        //cout << database_list.get(data_t).get_name()<<" " << database_list.get(data_t).get_dir()<<" " << database_list.get(data_t).get_txt_dir() << endl;
    }
    string password;

public:

    bool flag = true; //账户登录的状态
    //账户名
    string account_name;
    //当前账户文件夹的路径
    string account_dir;
    string account_txt_dir;
    //我们账户对象管理的数据库链表
    dlist<database> database_list;
    //账户下所有database名字和路径以及下层txt路径都放在database.txt文件中的
    map<string, vector<string>> database_map;

    //该类的构造比较简单，不会创建默认账户，只有调用create函数才会进行创建--暂定
    account_t(const string &name, string num, const string &dir, const string &txt_dir) {
        this->account_name = name;
        this->account_dir = dir;
        this->account_txt_dir = txt_dir;
        this->password = num;
        this->flag = true;
    }

    //简单传参构造函数
    account_t(const string &name, const string &dir, const string &txt_dir) {
        this->account_txt_dir = txt_dir;
        this->account_name = name;
        this->account_dir = dir;
        this->flag = true;
    }

    //复制构造函数
    account_t(const account_t &act) {
        this->password = act.password;
        this->flag = act.flag;
        this->account_name = act.account_name;
        this->account_dir = act.account_dir;
        this->database_list = act.database_list;
        this->database_map = act.database_map;
        this->account_txt_dir = act.account_txt_dir;
    }

    //默认无参构造函数
    account_t() {}
    ~account_t() {
        database_map.clear();
    }

    //向哈希表输入，同时完成链表初始化
    void set_map() {
        //刚创建文件流时必须使用peek方法才能判断是否为空
        //测试通过
        //cout << account_txt_dir << endl;

        Aifile.open(this->account_txt_dir, ios::in);
        Aifile.peek();
        while (!Aifile.eof()) {
            write();
            Aifile.peek();
        }
        Aifile.close();
    }

    //write_txt用来写txt便于每次的链表建立
    void write_txt(const string &name) {
        Aofile.open(this->account_txt_dir, ios::app | ios::out);
        Aofile << database_map.find(name)->first << " " << (database_map.find(name)->second)[0] << " " <<  (database_map.find(name)->second)[1] << endl;
        Aofile.close();
    }

    string get_dir() {
        return this->account_dir;
    }

    void set_dir(const string &str) {
        this->account_dir = str;
    }

    bool get_flag() {
        return this->flag;
    }

    void set_flag(bool l) {
        this->flag = l;
    }

    void set_password(const string &num) {
        this->password = num;
    }

    void set_name(const string &str) {
        this->account_name = str;
    }

    string get_name() {
        return this->account_name;
    }

    void set_txt_dir(const string &str) {
        this->account_txt_dir = str;
    }

    string get_txt_dir() {
        return this->account_txt_dir;
    }
    void set_flag_false() {
        this->flag = false;
    }

};
