#pragma once

#include<iostream>
#include<exception>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include "Table.h"
#include "Dlist.h"


//一个数据库应当是一个容器，用于存储表，管理对象是表，因此其操作应该是对表的
//---------------------------------------------------------

//还没看，请修正后发给我整合
using namespace std;
//前两个是针对.idx文件的
fstream Dofile;
fstream Difile;
//这个是针对.dat文件的
ifstream infile;

class database {
private:
//下面函数用于将txt文件的内容写入hash表中，逐行写入，账户密码分开写入,同时建立起对应的链表
    void write() {
        string name;
        string dir_t;
        string values;
        string row_values;
        vector<string> vec;
        getline(Difile, name, ' ');
        getline(Difile, dir_t, ' ');
        getline(Difile, values, '\n');

        //我觉得这里应该把这个values里面的具体的属性给解析出来，传给我们的table对象
        //-----------------重用之前的代码，看看能不能优化以减少代码量--------------------
        vector<string> temp1;
        split(values, temp1, ',');
        vector<string> column_name;
        vector<string> column_type;
        vector<string> if_primary;
        for (auto i = 0; i < static_cast<int>(temp1.size()); i++) {
            temp1[i].erase(0, temp1[i].find_first_not_of(" ")); //这里用到了erase()函数，作用是将字符串从第一个字符到第一个不为空的字符这之间的字符全部清楚
            temp1[i].erase(temp1[i].find_last_not_of(" ") + 1);//去掉字符串两边的空格
            vector<string> temp2;
            split(temp1[i], temp2, ' '); //这里进一步对子字符串按照空格进行划分
            if (temp2.size() == 2) { //表示不是主键
                column_name.push_back(temp2[0]);
                column_type.push_back(temp2[1]);
                if_primary.push_back("false");
            } else if (temp2.size() == 3) { //表示是主键
                column_name.push_back(temp2[0]);
                column_type.push_back(temp2[1]);
                if_primary.push_back("true");
            }
        }
        //-------------------------------------------------------------------------
        table tb(name, dir_t, column_name, column_type, if_primary);
        // for(size_t i=0;i<column_name.size();i++){
        //     cout<<column_name[i]<<" ";
        // }
        tb.set_values_t(values);
        //table_column初始化成功

        //接下来我们就要去初始化每一个表对象的数据，这个文件流的对象是dat文件
        //it表示第一列
        auto it = tb.table_column.begin();
        //cout<<it->first<<endl;//这里输出的是col2，出现了问题
        int num = 0; //表的行数
        infile.open(dir_t, ios::in); //为读取文件而打开文件
        infile.peek();
        while (!infile.eof()) { //当读到文件结束符的时候会返回0
            getline(infile, row_values, '\n');
            infile.peek();
            //cout << row_values << endl;
            ++num;//每读一行数据我们就让行数加一
            vector<string> temp;
            split(row_values, temp, ' '); //对我们读取到的一行值进行拆分
            //temp没问题
            for (size_t i = 0; i < temp.size() && it != tb.table_column.end(); i++, ++it) {
                //cout<< it->first<<" ";
                (it->second).push_back(temp[i]);
            }
            //cout<<endl;
            it = tb.table_column.begin(); //重新让我们的迭代器回到开头，开始下一行的读取.
        }

        tb.set_row_num(num);
        //tb.show_all();

        //cout << tb.get_row_num() << endl;
        infile.close();

        //输出正确
        //cout << tb.get_row_num() << endl;
        vec.emplace(vec.begin(), dir_t);
        vec.emplace(vec.begin() + 1, values);
        table_list.push_front(tb);//将我们创建好的表节点加入到database的链表里面
        table_map.insert(make_pair(name, vec));
    }
    //数据库对象的名字
    string database_name;
    //对象的路径
    string path;
    string database_txt_dir;
public:
    dlist<table> table_list;
    //下面的vector<string>存两个东西，路径和values
    map<string, vector<string>> table_map;//这个table_map是用来存储我们所有的对这个database里面的某一个表的所有操作的
    bool flag = true;//有用，勿删
    //传值的构造函数
    database(const string &database_name, const string &path, const string &txt_dir)noexcept {
        this->database_name = database_name;
        this->path = path;
        this->database_txt_dir = txt_dir;

    }
    //传类对象的构造函数
    database(const database &p) {
        this->database_name = p.database_name;
        this->path = p.path;
        this->table_list = p.table_list;
        this->database_txt_dir = p.database_txt_dir;
        this->table_map = p.table_map;
        this->flag=p.flag;
    }
    //默认构造函数
    database() {}
    ~database() {
        table_map.clear();
    }

    //读取文件向哈希表输入，同时完成链表初始化
    void set_map() {
        //刚创建文件流时必须使用peek方法才能判断是否为空
        //测试通过
        //cout<<this->database_txt_dir<<endl;
        //cout<<"end"<<endl;
        Difile.open(this->database_txt_dir, ios::in);
        Difile.peek();
        while (!Difile.eof()) {
            write();
            Difile.peek();
        }

        Difile.close();
    }

    //write_txt用来写txt便于每次的链表建立
    void write_txt(const string &name) {
        // cout<< database_map.find(name)->first << " " << database_map.find(name)->second << endl;
        Dofile.open(this->database_txt_dir, ios::app | ios::out);
        Dofile << table_map.find(name)->first << " " << (table_map.find(name)->second)[0] << " " << (table_map.find(name)->second)[1] << endl;
        Dofile.close();
    }//这个函数是用来将创建表的时候指定的列的属性插入到文件里面去

    string get_name() {
        return this->database_name;
    }

    string get_dir() {
        return this->path;
    }

    void set_name(const string &str) {
        this->database_name = str;
    }

    void set_path(const string &str) {
        this->path = str;
    }

    void set_flag_false() {
        this->flag = false;
    }

    bool get_flag(){
        return this->flag;
    }

    void set_txt_dir(const string &str) {
        this->database_txt_dir = str;
    }

    string get_txt_dir() {
        return this->database_txt_dir;
    }

    //定义一个字符串分割函数，用于对用户输入的数据库语句进行分割，方便后续处理
    void split(string &str, vector<string> &res, const char pattern) {
        for (auto it1 = str.begin(); it1 != str.end(); it1++) {
            *it1 = tolower(*it1);
        }
        //istringstream函数，位于sstring库中，构造函数参数为string，用于将string中的字符提取出来，得到一个字符串流
        istringstream is(str);

        string temp;
        //getline函数，接受一个字符流，将其传入temp对象中
        while (getline(is, temp, pattern))
            res.push_back(temp);//stoi(temp)转整型
        //循环结束后res内则是一个个被分解的字符串。
        return ;
    }
};
