#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <map>
#include <fstream>
#include "Dlist.h"


class table {
//----类型----------------------------------------
public:
    dlist<map<size_t, vector<string>>> column_list;//这个链表的每一个节点装的就是table里面的一列
    //用这个键值对来存所有的数据
    map<size_t, vector<string>> table_column;//前面的string存储列名称，后面的集合存储这一列所有的列值
    bool flag = true;//有用，勿删
    bool isnum(string s) { //判断字符串是否为数字，用于我们检测插入值的类型
        stringstream sin(s);
        double t;
        char p;
        if (!(sin >> t))
            return false;
        if (sin >> p)
            return false;
        else
            return true;
    }
private:
    int row_num;
    string table_dir;
    string table_name;
    //特殊的values，并非为模板，而是一个带括号的字符串，有用，勿删
    string values_t;
    vector<string> column_name;//存储列名
    vector<string> column_type;//存储列的属性
    vector<string> if_primary;//存储列是否为主键

public:

    //传条件和名字的构造函数
    table(const string &name, const string &dir, vector<string> &column_name, vector<string> &column_type, vector<string> &if_primary) {
        this->table_name = name;
        this->table_dir = dir;
        this->column_name = column_name;
        this->column_type = column_type;
        this->if_primary = if_primary;
        //我们还要设置table_column里面每一个列的列名称

        for (size_t i = 0; i < column_name.size(); i++) {
            vector<string> column_values;
            column_values.emplace(column_values.begin(), ""); //也就是在这个存储列值的集合里面初始化一个空的值
            //cout<<column_name[i];
            table_column.insert(make_pair(i, column_values)); //注意这里的column_values还是空的，因为我们在创建表对象的时候我们只指定了列的名称和属性
        }
        /*
        for(auto s:table_column){
            cout<<s.first<<" ";
        }
        */
        // cout<<endl;
        //我们在create table里面才使用这个构造函数，这个时候我们的table对象里面还没有值
        //auto s=table_column.begin();//这里拿到的为什么是col2？
        //cout<<"实例化键值对的时候:"<<s->first<<endl;

    }
    //我们现在只是把表对象里面列的相关属性给出传进去了，但是我们还要讲这个属性写入到文件里面去
    //这样保证下次重新创建table链表的时候可以直接从文件里面读取。

    //传名字和路径的构造函数
    table(const string &name, const string &dir) {
        this->table_name = name;
        this->table_dir = dir;
    }

    //复制构造函数
    table(const table &tb) {
        this->column_list = tb.column_list;
        this->table_column = tb.table_column;
        this->table_dir = tb.table_dir;
        this->table_name = tb.table_name;
        this->column_name = tb.column_name;
        this->column_type = tb.column_type;
        this->if_primary = tb.if_primary;
        this->values_t = tb.values_t;
        this->row_num = tb.row_num;
    }

    //默认的构造函数
    table() {}
    ~table() {}

    //获取当前对象路径
    string get_dir() {
        return this->table_dir;
    }

    //设置路径
    void set_dir(const string &str ) {
        this->table_dir = str;
    }
    /*
    //获取值
    vector<string> get_values() {
        return this->values;
    }
    //设置值
    void set_values(vector<string> &vec) {
        this->values = vec;
    }
    */
    //设置特殊values
    void set_values_t(const string &str) {
        this->values_t = str;
    }

    //获取特殊values
    string get_values_t() {
        return this->values_t;
    }

    //设置表的行数
    void set_row_num(const int num) {

        this->row_num = num;

    }

    //获取表的行数
    int get_row_num() {
        return this->row_num;
    }


    void show_all_column_type() {
        for (auto s : column_name) {
            cout << s;
            cout << "\t";
        }
        cout << endl;
        for (auto s : column_type) {
            cout << s;
            cout << "\t";
        }
        cout << endl;
        for (auto s : if_primary) {
            cout << s;
            cout << "\t";
        }
        cout << endl;
    }

    //原始的show函数，用于输出整张表
    void show_all() {
        //我们先输出每一列的列名：
        for (size_t i = 0; i < column_name.size(); i++) {
            cout << column_name[i] << " ";
        }
        cout << endl;
        /*
        for(auto s: table_column){
            cout<<s.first<<" ";
        }
        cout<<endl;
        */

        //按行输出整个表
        for (int i = 1; i <= row_num; i++) {
            for (auto s : table_column) { //同样拿到每一个键值对
                cout << s.second[i] << " "; //输出第i行的数据
            }
            cout << endl;
        }

        /*
         cout << (this->row_num) << endl;
         for (auto s : table_column) { //这里的S就是我们tb表对象里面的每一个键值对
             for (auto t : s.second) { //这里的s.second就是我们表对象里面的每一列的值列表,t就是我们这个列表里面每一列的值
                 cout << t << "\t";
             }
             cout << endl;
         }
         cout << endl;
         */
    }

    //重载的show函数，用于输出列名为column_name的整列
    void show(const string &name) {//这个时候我们查询的条件为空
        //我们先要找到这个column是第几列:
        for (size_t i = 0; i < column_name.size(); i++) {
            if (column_name[i].compare(name) == 0) { //此时的i就是我们要输出的列号
                //输出表的这一列的数据
                auto it = table_column.begin(); //获取到第一列
                for (size_t m = 1; m <= i; m++) {
                    it++;
                }
                cout << column_name[i] << endl;
                for (int j = 1; j <= row_num; j++) {
                    cout << it->second[j] << endl;
                }
            }
        }
        cout << "show done2" << endl;
    }

    //重载的show函数，用于输出condition_n列值为condition_v的这整行
    void show_by_equal(const string &condition_n, const string &condition_v) {
        //cout<<"condition_n:"<<condition_n<<endl;
        //cout<<"condition_v:"<<condition_v<<endl;
        map<size_t, vector<string>>::iterator column;
        for (size_t i = 0; i < column_name.size(); i++) {
            if (column_name[i].compare(condition_n) == 0) {
                column = this->table_column.find(i); //这样就定位到了我们要进行条件判断的列

            }
        }
        /*
        测试用：
        for(auto s: column->second){
           cout<<s<<" ";
        }
        cout<<endl;
        */
        for (size_t i = 1; i < column->second.size(); i++) {
            if (column->second[i].compare(condition_v) == 0) { //此时的i就是我们要输出的行号
                for (auto s : this->table_column) {
                    cout << s.second[i] << " ";
                }//输出表里面的这一行
                cout << endl;
            }

        }
        cout << "show done3" << endl;
    }
    //重载的show函数，用于输出condition_n列值小于condition_v的这整行
    void show_by_smaller(const string &condition_n, const string &condition_v){
        map<size_t, vector<string>>::iterator column;
        for (size_t i = 0; i < column_name.size(); i++) {
            if (column_name[i].compare(condition_n) == 0) {
                column = this->table_column.find(i); //这样就定位到了我们要进行条件判断的列

            }
        }
        for (size_t i = 1; i < column->second.size(); i++) {
            if (column->second[i].compare(condition_v) < 0) { //此时的i就是我们要输出的行号
                for (auto s : this->table_column) {
                    cout << s.second[i] << " ";
                }//输出表里面的这一行
                cout << endl;
            }

        }
        cout << "show done5" << endl;
        
    }
    //重载的show函数，用于输出condition_n列值大于condition_v的这整行
    void show_by_larger(const string &condition_n, const string &condition_v){
        map<size_t, vector<string>>::iterator column;
        for (size_t i = 0; i < column_name.size(); i++) {
            if (column_name[i].compare(condition_n) == 0) {
                column = this->table_column.find(i); //这样就定位到了我们要进行条件判断的列

            }
        }
        for (size_t i = 1; i < column->second.size(); i++) {
            if (column->second[i].compare(condition_v) > 0) { //此时的i就是我们要输出的行号
                for (auto s : this->table_column) {
                    cout << s.second[i] << " ";
                }//输出表里面的这一行
                cout << endl;
            }

        }
        cout << "show done7" << endl;
        
    }
    //重载的show函数，用于输出满足条件的行里面的名称为column_n的列
    void show_column_by_equal(const string &column_n, const string &condition_n, const string &condition_v) {
        //cout<<condition_n<<endl;
        /*
        for(auto s: this->table_column){
            cout<<s.first<<" ";
            for(auto m: s.second){
                cout<<m<<" ";
            }
            cout<<endl;
        }
        cout<<endl;
        */
        map<size_t, vector<string>>::iterator column;
        for (size_t i = 0; i < column_name.size(); i++) {
            if (column_name[i].compare(condition_n) == 0) {
                column = this->table_column.find(i); //这样就定位到了我们要进行条件判断的列

            }
        }
        //cout<<column->first<<endl;
        /*
        for(size_t i=1;i<column->second.size();i++){
            cout<<column->second[i]<<endl;
        }
        */
        for (size_t i = 1; i < column->second.size(); i++) {
            if (column->second[i].compare(condition_v) == 0) { //此时的i就是我们要输出的行号
                //cout<<"查找到:"<<i<<endl;
                map<size_t, vector<string>>::iterator column;
                for (size_t j = 0; j < column_name.size(); j++) {
                    if (column_name[j].compare(column_n) == 0) {
                        column = this->table_column.find(j); //这样就定位到了我们要进行条件判断的列
                        cout << column->second[i];
                    }
                }
                cout << endl;
            }

        }

        cout << "show done4" << endl;
    }
    //重载的show函数，用于输出满足条件的行里面的名称为column_n的列
    void show_column_by_smaller(const string &column_n, const string &condition_n, const string &condition_v) {
        
        map<size_t, vector<string>>::iterator column;
        for (size_t i = 0; i < column_name.size(); i++) {
            if (column_name[i].compare(condition_n) == 0) {
                column = this->table_column.find(i); //这样就定位到了我们要进行条件判断的列

            }
        }
        
        for (size_t i = 1; i < column->second.size(); i++) {
            if (column->second[i].compare(condition_v) < 0) { //此时的i就是我们要输出的行号
                //cout<<"查找到:"<<i<<endl;
                map<size_t, vector<string>>::iterator column;
                for (size_t j = 0; j < column_name.size(); j++) {
                    if (column_name[j].compare(column_n) == 0) {
                        column = this->table_column.find(j); //这样就定位到了我们要进行条件判断的列
                        cout << column->second[i];
                    }
                }
                cout << endl;
            }

        }

        cout << "show done6" << endl;
    }
    //重载的show函数，用于输出满足条件的行里面的名称为column_n的列
    void show_column_by_larger(const string &column_n, const string &condition_n, const string &condition_v) {
        
        map<size_t, vector<string>>::iterator column;
        for (size_t i = 0; i < column_name.size(); i++) {
            if (column_name[i].compare(condition_n) == 0) {
                column = this->table_column.find(i); //这样就定位到了我们要进行条件判断的列

            }
        }
        
        for (size_t i = 1; i < column->second.size(); i++) {
            if (column->second[i].compare(condition_v) > 0) { //此时的i就是我们要输出的行号
                //cout<<"查找到:"<<i<<endl;
                map<size_t, vector<string>>::iterator column;
                for (size_t j = 0; j < column_name.size(); j++) {
                    if (column_name[j].compare(column_n) == 0) {
                        column = this->table_column.find(j); //这样就定位到了我们要进行条件判断的列
                        cout << column->second[i];
                    }
                }
                cout << endl;
            }

        }

        cout << "show done8" << endl;
    }
    //插入函数
    bool insert_data(vector<string> &vec) {
        //cout << this->table_dir<< endl;//测试用,判断文件的路径是否正确
        /*
        for (size_t i = 0; i < vec.size(); i++) {
            cout << vec[i] << " ";
        }
        */
        //我们首先要判断我们插入的值是否符合我们定义表对象的时候的要求
        if (vec.size() > this->column_name.size()) { //表示插入的值的数量不符合要求
            cout << "please insert the lack values accorrding to the table defination" << endl;
            return false;
        } else if (vec.size() < this->column_name.size()) { //表示插入的值的数量不符合要求
            cout << "please insert the enough values acorrding to the table defination" << endl;
            return false;
        } else {
            //依次判断我们插入值的每一个类型
            for (int i = 0; i < int(vec.size()); i++) {
                if (isnum(vec[i]) && (this->column_type[i] == "int")) {
                    //cout<<"int"<<" ";//测试用
                    continue;
                } else if (!isnum(vec[i]) && (this->column_type[i] == "string")) {
                    //cout<<"string"<<endl;//测试用
                    continue;
                } else {
                    cout << "please insert the correct type value according to the table defination" << endl;
                    return false;
                }

            }
        }//通过这个循环我们才能判断插入的值符合我们的要求

        //我们要打开我们对应的表文件，向里面写入值
        fstream infile;//创建文件输入流
        infile.open(this->table_dir, ios::app); //打开本table对象对应的文件路径,以追加方式打开文件
        //向表文件里面写入插入的数据
        for (auto s : vec) {
            infile << s << " ";//依次向文件里面写入数据，值之间以空格隔开
        }
        infile << endl;//每一行的末尾追加换行符
        infile.close();

        return true;
    }

    //显示该表列名用于提示用户输入规范
    void show_column() {
        cout << "Please match this patten when you insert the values:" << endl;
        for (size_t i = 0; i < column_name.size(); i++) {
            cout << column_name[i] << " ";
        }
    }
    void show_column_type() {
        for (auto s : this->column_type) {
            cout << s << " ";
        }
    }

    //默认的clear函数，用于清空该表
    void clear() {
        //cout<<this->get_dir()<<endl;
        ofstream delete_file(this->table_dir, ios::trunc); //ios::trunc表示清空当前文件的内容
        delete_file.close();
        cout << "clear successfully" << endl;
    }

    //重载的clear函数，用于删除列名condition_n值condition_v的这一行
    void clear(const string &condition_n, const string &condition_v) {
        map<size_t, vector<string>>::iterator column;
        for (size_t i = 0; i < column_name.size(); i++) {
            if (column_name[i].compare(condition_n) == 0) {
                column = this->table_column.find(i); //这样就定位到了我们要进行条件判断的列

            }
        }     
        for (size_t j = 1; j < column->second.size(); j++) {
            if (column->second[j].compare(condition_v) == 0) { //此时的j就是我们要删除的行号
                //cout<<column->second[j]<<endl;
                cout << "要删除的数据：";
                //将表里面这一行的元素全部删除erase掉
                for (auto &s : this->table_column) {
                    auto fst = s.second.begin();
                    fst += j;
                    cout << *fst << " "; //输出一下我们要删除的这一行
                    s.second.erase(fst);//将迭代器对应的元素从集合里面删除掉
                    //cout << s.second[j];
                }
                cout << endl;
                this->row_num--;//这里记得将表的行号-1
                /*  
                for (int t = 1; t <= this->row_num; t++) {
                    for (auto s : this->table_column) {
                        cout << s.second[t] << " ";
                    }
                    cout << endl;
                }
                */
            }
        }


        //打开文件，并且清除文件的内容
        ofstream delete_file(this->table_dir,ios::trunc);//ios::trunc表示清空当前文件的内容
        delete_file.close();
        //重新将我们表里面的数据写入文件当中
        fstream infile;//创建文件输入流
        infile.open(this->table_dir, ios::app); //打开本table对象对应的文件路径,以追加方式打开文件
        //向表文件里面写入插入的数据
        for (int i = 1; i <= this->row_num; i++) { //i就是行号
            for (auto &s : this->table_column) {
                infile<<s.second[i]<<" ";
                //cout << s.second[i] << " ";
            }
            infile<<endl;//每一行的末尾追加换行符
            //cout << endl;
        }
        infile.close();

        cout << "clear successfully" << endl;
    }

    void table_to_file(const string file_dir) { //这个函数用来将我们当前的table表里面的数据更新到文件里面去

    }

    string get_name() {
        return this->table_name;
    }

    //一般不会用到，但是不要删它
    void set_flag_false() {
        this->flag = false;
    }

    bool get_flag() {
        return this->flag;
    }


};


