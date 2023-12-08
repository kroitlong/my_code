
#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <dirent.h>
#include <algorithm>
#include "Account.h"
#include "Table.h"
#include "DataBase.h"
#include "MDYS.h"
#include "Dlist.h"
using namespace std;


//定义一个字符串分割函数，用于对用户输入的数据库语句进行分割，方便后续处理
void split(string &str, vector<string> &res, const char pattern) {
    for (auto it1 = str.begin(); it1 != str.end(); it1++) {
        *it1 = tolower(*it1);
    }
    //istringstream函数，位于sstring库中，构造函数参数为string，用于将string中的字符提取出来，得到一个字符串流
    istringstream is(str);

    string temp;
    //getline函数，接受一个字符流，将其传入temp对象中
    while (getline(is, temp, pattern))//这里使用了正则表达式进行字符串匹配
        res.push_back(temp);//stoi(temp)转整型
    //循环结束后res内则是一个个被分解的字符串。
    return ;
}


//该类用于接收上层指令信息，作为相应信息的借口
class Interface {
private:
    string condition;
    string condition_name;
    string condition_value;
    string column_name;

public:
    //下面3个类用于与底层实现对接，表示当前使用的对象
    //act在登录时设置
    MDYS mdys;
    account_t act;
    //db在use和create database里确认
    database  db;
    table tb;
    //复制构造函数
    Interface(const Interface &l) {
        this->act = l.act;
        this->db = l.db;
        this->tb = l.tb;
    }
    Interface() {}
    ~Interface() {}

    //创建表的操作
    //table有个构造函数table(const string &str,vector<string> &vec)
    void excute_create_table_operation(string name, string values) {
        vector<string> vec;
        //values也得先保存下来，已经去掉括号了
        string value_copy = values;

        vector<string> temp1;
        split(values, temp1, ','); //将每一个结果按照 ，进行提取
        vector<string> column_name;
        vector<string> column_type;
        vector<string> if_primary;
        //static_cast<type>表示进行显式的类型转换
        for (size_t i = 0; i < temp1.size(); i++) {
            temp1[i].erase(0, temp1[i].find_first_not_of(" ")); //这里用到了erase()函数，作用是将字符串从第一个字符到第一个不为空的字符这之间的字符全部清除
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
        //表示表所创建的路径
        string dir = db.get_dir() + "/" + name + ".dat";
        //创建表文件
        create_txt(dir);
        //将对象插入链表,同时完成记录
        vec.emplace(vec.begin(), dir);
        vec.emplace(vec.begin() + 1, value_copy);//这里是将我们创建表的时候插入的values值插入到table_map里面去，方便我们后续写入文件

        //测试通过
        //cout << vec[0] << " " << vec[1] << endl;

        db.table_map.insert(make_pair(name, vec));//我们将对应表名称创建的时候的指定列的属性给插入到table_map里面去
        table tb1(name, dir, column_name, column_type, if_primary); //创建表对象

        db.table_list.push_front(tb1);
        tb = db.table_list.get(name);
        tb.show_all_column_type();

        //测试通过
        //cout << db.get_txt_dir() << endl;
        db.write_txt(name);//这里是将name表名对应的键值对插入到我们对应的数据库表记录文件里面去
        cout << "table " << name << " created" << endl;
        //实例化一个表对象，并且把它作为节点添加到数据库的链表里面去
    }

    //创建database的操作
    void excute_create_database_operation(const string &name) {
        //要在当前account下创建一个database，在登陆时已经选择了当前的account，因此选择act操作
        //确定路径作为传入参数
        //一个模板参数用来存放dir和txt_dir
        vector<string> vec;
        string database_path = act.get_dir() + "/" + name;
        string database_txt_path = act.get_dir() + "/" + name + "/" + name + "_store.idx";
        //创建文件夹
        create_file(database_path);
        create_txt(database_txt_path);
        //将对象插入链表,同时完成记录
        vec.emplace(vec.begin(), database_path);
        vec.emplace(vec.begin() + 1, database_txt_path);
        act.database_map.insert(make_pair(name, vec));
        database db1(name, database_path, database_txt_path);
        act.database_list.push_front(db1);
        db = act.database_list.get(name);
        //之后应该将database的名字和路径写入创建账户是生成的txt文件中
        act.write_txt(name);
        cout << "database " << name << " created" << endl;
    }

    //drop功能接口
    //table函数：get_dir()获取当前对象的路径
    void  excute_drop_operation(string type, string name) {
        if (type.compare("database") == 0) { //表示要删除数据库
            //先将act中链表里的相应结点删除
            auto p = act.database_list.find(name);
            //获取待删除结点的路径便于进行文件删除;
            if (p == nullptr) {
                cout << "NO this database there ";
                return;
            }
            string dir = (p->data).get_dir();
            act.database_list.pop(name);
            //删除database文件夹
            delete_file(dir);
            //更新act下的索引文件，1-删除，2-重写
            delete_file(act.account_txt_dir);
            create_txt(act.account_txt_dir);
            for (auto p = act.database_list.get_head()->next; p != act.database_list.get_tail(); p = p->next )
                act.write_txt(p->data.get_name());
            cout << "database " << name << " dropped" << endl;
            cout << endl;
        } else if (type.compare("table") == 0) {
            //先删除结点
            auto p = db.table_list.find(name);
            if (p == nullptr) {
                cout << "NO this table there ";
                return;
            }
            string dir = (p->data).get_dir();
            db.table_list.pop(name);
            //删除table_name对应的表
            delete_file(dir);
            //更新database下的索引文件，1-删除，2-重写
            delete_file(db.get_txt_dir());
            create_txt(db.get_txt_dir());
            for (auto p = db.table_list.get_head()->next; p != db.table_list.get_tail(); p = p->next )
                db.write_txt(p->data.get_name());
            cout << "table " << name << " dropped" << endl;
            cout << endl;
        }
    }

    //-------------------------------------------------------------------------------------------------------
    //-------------------------以上功能均已实现-----------------------------------------------------------------
    //select功能接口,针对表使用
    void  excute_select_operation(string column, string name, string condition_t ) {
        //这里的condition比如说可以是类似于：where(Major='软件工程')
        //如果有条件，则将条件表示的那行全部输出 
        //如果无条件，即column为all，则输出
        condition = condition_t;
        tb = db.table_list.get(name);
        //在all的条件下不应该有condition,all表示输出整张表
        if (column.compare("all") == 0) {
            if (condition.empty()) {
                //表示无条件查询，输出整张表
                tb.show_all();
                cout << "Seleting operation to all : Done" << endl;
                cout << endl;
            } else {
                //在输入为all的条件下，如果还存在condition应该发出警告并让其重新输入,select all from <tbname> where(name='longtao')不应该存在
                cout << "Condition shall not exist if you select all" << endl;
                cout << endl;
            }
        } else if (column.compare("*") == 0) { //表示我们要查询的是全部的列
            if (condition.empty()) { //无条件输出全部列
                tb.show_all();
            } else {
                string str = condition;
                string value = str.substr(str.find("(") + 1, str.find(")") - str.find("(") - 1);
                auto vc1 = vector<string>();
                split(value, vc1, '='); //将where字句按'='拆分
                if(vc1.size()==2){
                    //cout<<"等于"<<endl;
                    //条件里面的列名
                    string condition_name = vc1[0];
                    //条件里面的列值
                    string condition_value = vc1[1];
                    //重载的show函数,输出对应列中满足条件的这一行
                    tb.show_by_equal(condition_name, condition_value);
                    cout << endl;
                }else{
                    auto vc2 = vector<string>();
                    split(value, vc2, '<'); //将where字句按'<'拆分
                    if(vc2.size()==2){
                        //cout<<"小于"<<endl;
                        //条件里面的列名
                        string condition_name = vc2[0];
                        //条件里面的列值
                        string condition_value = vc2[1];
                        //重载的show函数,输出对应列中满足条件的这一行
                        tb.show_by_smaller(condition_name, condition_value);
                    }else{
                        auto vc3 = vector<string>();
                        split(value, vc3, '>'); //将where字句按'<'拆分
                        if(vc3.size()==2){
                            //cout<<"大于"<<endl;
                            //条件里面的列名
                            string condition_name = vc3[0];
                            //条件里面的列值
                            string condition_value = vc3[1];
                            //重载的show函数,输出对应列中满足条件的这一行
                            tb.show_by_larger(condition_name, condition_value);
                        }
                    }
                }
            }
        } else { //表示column是我们要查询的列名称
            string column_name = column;
            cout << column_name << endl;
            if (condition.empty()) { //表示无条件查询
                //打开table_name对应的文件，把所有的对应列记录读出来
                cout << "Below is all values from " << column << endl;
                cout << endl;
                //重载的show函数，用于输出具有colum标签的这一整列及其对应的索引值
                tb.show(column_name);
            } else {
                /* 
                //将 “where(“ 置为空
                string str = condition;
                string value = str.substr(str.find("(") + 1, str.find(")") - str.find("(") - 1);
                auto vc = vector<string>();
                split(value, vc, '='); //将where字句拆分
                //条件里面的列名
                condition_name = vc[0];
                //条件里面的列值
                condition_value = vc[1];
                //重载的show函数,输出对应列中满足条件的这一行
                tb.show_column_by_equal(column_name, condition_name, condition_value);
                cout << endl;
                */
                string str = condition;
                string value = str.substr(str.find("(") + 1, str.find(")") - str.find("(") - 1);
                auto vc1 = vector<string>();
                split(value, vc1, '='); //将where字句按'='拆分
                if(vc1.size()==2){
                    //cout<<"等于"<<endl;
                    //条件里面的列名
                    string condition_name = vc1[0];
                    //条件里面的列值
                    string condition_value = vc1[1];
                    //重载的show函数,输出对应列中满足条件的这一行
                    tb.show_column_by_equal(column_name,condition_name, condition_value);
                    cout << endl;
                }else{
                    auto vc2 = vector<string>();
                    split(value, vc2, '<'); //将where字句按'<'拆分
                    if(vc2.size()==2){
                        //cout<<"小于"<<endl;
                        //条件里面的列名
                        string condition_name = vc2[0];
                        //条件里面的列值
                        string condition_value = vc2[1];
                        //重载的show函数,输出对应列中满足条件的这一行
                        tb.show_column_by_smaller(column_name,condition_name, condition_value);
                    }else{
                        auto vc3 = vector<string>();
                        split(value, vc3, '>'); //将where字句按'<'拆分
                        if(vc3.size()==2){
                            //cout<<"大于"<<endl;
                            //条件里面的列名
                            string condition_name = vc3[0];
                            //条件里面的列值
                            string condition_value = vc3[1];
                            //重载的show函数,输出对应列中满足条件的这一行
                            tb.show_column_by_larger(column_name,condition_name, condition_value);
                        }
                    }
            }

        }

    }
}


    //insert功能接口
    void excute_insert_operation(string name, string values) {
        //这里的insert格式大致为 "insert table_name values(value, value, value3...)", values作为后面的整个代表，内部无空格，可以用哈希表
        tb = db.table_list.get(name);//这里我们拿到对应的表文件对象
        //用来输出这个表的所有列名，提示用户怎么输入
        cout << "please insert the columns as the type like:";
        tb.show_column_type();
        cout << endl;
        //除括号，new_str存（）里面的内容
        string new_str = values.substr(values.find("(") + 1, values.find(")") - values.find("(") - 1);
        vector<string> vc;
        //此时vc里面装的就是我们要插入的一个一个的值
        split(new_str, vc, ',');
        //我们还要对输入的值进一步处理，避免用户输入了空格,也就是将输入的空格全部删除掉
        for (auto s : vc) {
            s.erase(0, s.find_first_not_of(" "));
            s.erase(s.find_last_not_of(" "), s.size()); //这里移除的最后一个空字符到字符串最后一个字符之间的空格
        }
        if (tb.insert_data(vc) == true) { //我们在这个函数里面肯定要执行一些判断逻辑
            cout << "Insert successfully" << endl;
        } else {
            cout << "Insert unseccessfully" << endl;
        }//这里只是将文件里面的内容进行更新了，但是我们的数据库对象还没有更新
        db.set_map();//同步更新数据库对象
        cout << endl;
    }

    //delete功能接口,删除表中某一行
    void excute_delete_operation(string name, string values) {
        /*
        功能：根据条件（如果有）删除表中的记录。
        语法：
        delete from <table_name> [ where <cond> ]，删除满足条件的这一行数据,一般条件应该为索引值即哈希表中键值对中的键
        where 子句：语法同 select 的。
        */
        //如果没有where则清空表，但不是删除表
        //cout<<values<<endl;
        tb = db.table_list.get(name);
        if (values.empty()) {
            tb.clear();//清空文件的内容
            db.set_map();//重新初始化数据库
            cout << "Delete successfully" << endl;
            cout << endl;
            //删除表table_name里面所有的记录，但是不删除表
        } else {
            //去括号
            string new_str = values.substr(values.find("(") + 1, values.find(")") - values.find("(") - 1);
            vector<string> vc;
            split(new_str, vc, '='); //将where字句拆分
            condition_name = vc[0]; //条件里面的列名
            condition_value = vc[1]; //条件里面的列值
            //重载的clear函数，函数满足条件的这一行
            //cout<<condition_name<<endl;
            //cout<<condition_value<<endl;
            tb.clear(condition_name, condition_value);
            db.set_map();
            //cout << vc[0] << " : " << vc[1] << endl;
            cout << endl;
        }
    }


    //--------------------------------------------------------------------------------------------
    //-----------------------------------以下功能均已完善-------------------------------------------
    //use功能接口
    void excute_use_operation(string name) {
        //使当前所有操作的数据库为name对应的数据库
        db = act.database_list.get(name);
        if (db.get_flag() == false) {
            return ;
        }
        db.set_map();
        //修改路径至名为database_name的数据库
        cout << endl;
        cout << "Use " << name << " Successfully" << endl;

    }



    //该方法用来展示以当前操作对象下的所有文件结构,传入相关的指令，在这里面进行选择show哪个东西
    void excute_show_all() {
        cout << "show the structure now:" << endl;
        vector<string> vec_act;
        vector<string> vec_dat;
        size_t count1 = 0;
        int count = 0;
        //输出All_Account文件夹下的所有结构,精确到txt文件
        vec_act = get_structure(act.account_dir);
        //定义一个迭代器用来查找vector,以删除外层的.idx文件，因为不在链表中
        vector<string>::iterator it = vec_act.begin();
        while (it != vec_act.end()) {
            if (*it == act.get_name() + ".idx") {
                it = vec_act.erase(vec_act.begin() + count);
            } else {
                ++it;
            }
            ++count;
        }
        cout << act.get_name() << "//目录" << endl;
        while (count1 != vec_act.size()) {
            cout << "|----" << vec_act[count1] << "//目录，数据库名称" << endl;
            database db_t = act.database_list.get(vec_act[count1]);
            db_t.set_map();
            vec_dat = get_structure(db_t.get_dir());
            size_t count2 = 0;
            while (count2 != vec_dat.size()) {
                cout << "|\t|----" << vec_dat[count2] << endl;
                ++count2;
            }
            ++count1;
        }
        cout << "|----" << act.account_name << ".idx" << "//索引文件" << endl;
        cout << "Done" << endl;
    }


//删除文件函数
    void delete_file(const string &dir) {
        string command = "sudo rm -rf " + dir;
        system(command.c_str());
    }

//创建文件操作
    void create_file(const string &str) {
        string command = "mkdir " + str;
        system(command.c_str());
    }

//创建文本文件
    void create_txt(const string &str) {
        string command = "touch " + str;
        system(command.c_str());
    }

//展示当前路径下文件结构
    vector<string> get_structure(const string &str) {
        //定义必要参数
        vector<string> vec;
        int count = 0;
        struct dirent *filename;
        DIR *dir;
        //opendir函数打开目录，返回指向目录的DIR结构体
        dir = opendir(str.c_str());
        //readdir函数读取DIR参数返回指向对应目录下的所有文件的dirent结构体，遍历该结构体即可得到信息
        while ( ( filename = readdir(dir) ) != NULL ) {
            // get rid of "." and ".."
            if ( strcmp( filename->d_name, "." ) == 0 ||
                    strcmp( filename->d_name, "..") == 0    )
                continue;
            string name = filename->d_name;
            vec.emplace(vec.begin() + count, name);
            ++count;
        }
        closedir(dir);
        return vec;

    }


    void set_act(account_t p) {
        this->act = p;
    }

    void set_dir(const string &str) {
        act.set_dir(str);
    }

    void set_MDYS(const MDYS &mdys) {
        this->mdys = mdys;
    }

    account_t *get_ACT() {
        auto p = &(this->act);
        return p;
    }
};