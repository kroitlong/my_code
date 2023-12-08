#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <limits>
#include "MDYS.h"
#include "Account.h"
#include "Interface.h"
using namespace std;


//这个类用于处理传入的指令字符串，剥离出关键信息并传入interface接口
class present {
private:
    string input;
    string Create = "create";
    string exit = "exit";
    string Delete = "delete";
    string Use = "use";
    string Drop = "drop";
    string Select = "select";
    string Insert = "insert";
    string Show = "show";
    string Enter = "enter";
    //showdatabase用于输出当前账户下的所有database文件名即路径
    string Showdatabase = "showdatabase";
    MDYS mdys;
    Interface inf;
    bool log_flag = false;
    bool exit_flag = false;
    string password;
    //重载的字符拼接函数，旨在消除空格
    //@overload
    string strcmp(const vector<string> &res, int a, int b) {
        string str;
        for (int i = a; i < b; i++) {
            if (res[i] == " ")continue;
            str = str + res[i];
        }
        return str;
    }
public:
    //初始化构造函数
    present(const string &str, const int &num ) {
        this->input = str;
        this->password = num;
    }
    //复制构造函数
    present(const present &l) {
        this->input = l.input;
        this->password = l.password;
        this->mdys = l.mdys;
        this->inf = l.inf;
    }

    //默认无参构造函数
    present() {}
    ~present() {}
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
    //设置要操作的mdys
    void set_MDYS(const MDYS &mdys) {
        this->mdys = mdys;
    }

    //将输入的指令传到这里才处理，str表示传入的指令
    void database_operation(string str) try {
        //我们先将用户的输入语句进行拆分，database_operation存放的是指令中有多少的词
        vector<string> database_operation_v;
        string column;
        string condition;
        //name表示databasename和tablename
        string name;
        //type表示database或者table
        string type;
        string values;
        split(str, database_operation_v, ' ');
        //-----------------------------------------------select有关的操作，分别是对表查列和对库查表
        if (database_operation_v[0].compare(Select) == 0) {
            //Select <column> from <table> [where<cond>], select函数只用于表，条件不是必须的，条件格式为where(column_name=name)
            if (database_operation_v.size() < 4)throw - 1;//抛出一个异常，表示输入的指令不对

            //表示有条件时，确定condition
            if (database_operation_v.size() > 4) {
                condition = strcmp(database_operation_v, 4, database_operation_v.size());
            } else {
                condition = "";
            }
            //cout << condition << endl;
            column = database_operation_v[1];
            name = database_operation_v[3];
            //向interface传入参数
            inf.excute_select_operation(column, name, condition);

            //-----------------------------------------------drop有关的操作，分别是删库和删表
        } else if (database_operation_v[0].compare(Drop) == 0) {
            //Drop database <dbname>, drop table <tbname>
            if (database_operation_v.size() < 3)throw - 1;
            name = database_operation_v[2];
            type = database_operation_v[1];
            //向interface传入参数
            inf.excute_drop_operation(type, name);

            //-----------------------------------------------create有关的操作，分别是建库和建表，都默认从最后一个开始建
        } else if (database_operation_v[0].compare(Create) == 0) {
            //Create database <dbname>，create table <tbname> (每一列的名字)
            if (database_operation_v.size() < 3)throw - 1;
            type = database_operation_v[1];
            if (type.compare("database") == 0) {
                //只需要传入name即可，默认在当前账户下创建
                name = database_operation_v[2];
                inf.excute_create_database_operation(name);
            } else if (type.compare("table") == 0) { //表示创建的是一个表
                //values = strcmp(database_operation_v, 2, database_operation_v.size());
                //values = values.replace(values.begin(), values.begin() + name.size(), "");
                //测试,通过
                // cout << values << endl;
                //去括号
                name = database_operation_v[2].substr(0, database_operation_v[2].find("("));
                string values = str.substr(str.find("(") + 1, str.find(")") - str.find("(") - 1);
                //测试，已经去掉括号
                //cout<< values <<endl;
                if (values == "") {
                    cout << "please input the create_operation with condition!" << endl;
                } else {
                    inf.excute_create_table_operation(name, values);
                }
            }
            //-----------------------------------------------insert有关的操作，只有对表插数据，默认从最后一行插入,在select table之后操作
        } else if (database_operation_v[0].compare(Insert) == 0) {
            //Insert <tbname> values(<const_value>[,const_value...])
            if (database_operation_v.size() < 3)throw - 1;
            name = database_operation_v[1];
            values = strcmp(database_operation_v, 2, database_operation_v.size());
            //传入参数
            inf.excute_insert_operation(name, values);

            //-----------------------------------------------delete有关的操作，只有对表删数据，删除指定行的数据
        } else if (database_operation_v[0].compare(Delete) == 0) {
            //Delect from <tbname> [where<cond>],如果忘记指定where条件，则清空表
            //cout<<database_operation_v.size()<<endl;
            if (database_operation_v.size() < 3)throw - 1;
            if (database_operation_v.size() > 3) {
                //condition =  strcmp(database_operation_v, 3, database_operation_v.size());
                condition = database_operation_v[3];
            } else {
                condition = "";
            }
            name = database_operation_v[2];
            //传入参数
            inf.excute_delete_operation(name, condition);

            //-----------------------------------------------切换数据库即进入该数据库，之后对表的操作都是对该数据库而言的
        } else if (database_operation_v[0].compare(Use) == 0) {
            //Use <dbname>切换数据库操作
            if (database_operation_v.size() < 2)throw - 1;
            string database_name = database_operation_v[1];
            inf.excute_use_operation(database_name);

            //-----------------------------------------------用来输出整个数据库结构
        } else if (database_operation_v[0].compare(Show) == 0) {
            //按照层次结构来输出整个数据库里面的表结构.
            cout << "Show Operation" << endl;
            inf.excute_show_all();
        } else if (database_operation_v[0].compare(exit) == 0) {
            cout << "==================================================================" << endl;
            cout << endl;
            return;

        } else cout << "Erroe Command, Please Check The Menu Again " << endl;
    } catch (int &e) {
        cout << "Erroe Command, Please Check The Menu Again " << endl;
    }

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

    void show() { //此方法用于数据库界面的展示以及用户的交互

        cout << "Please enter the Account_name :(don't have a account? please enter Create )"  << endl;
        getline(cin, input);
        while (input == "") {
            cout << "illegal input, please try again: ";
            getline(cin, input);
        }
        //转小写操作
        for (auto it1 = input.begin(); it1 != input.end(); it1++) {
            *it1 = tolower(*it1);
        }
        if (input.compare(Create) == 0) {
            //执行account创建
            while (!mdys.flag) {
                cout << "Now please set the account_name: ";
                getline(cin, input);
                //向interface传递mdys
                inf.set_MDYS(mdys);
                mdys.set_account(input);
                //设置当前账户为执行路径
                auto act = mdys.account_list.get(input);
                inf.set_act(act);
            }
            //设置账户对应的密码
            cout << "Please set a password: ";
            getline(cin, password);
            mdys.set_password(input, password);
            //调用设置密码函数，将之与先前输入的account合并为一个键值对加入哈希表中。
            log_flag = true;
        } else {
            //设置act,在初始化MDYS时已经完成了向account_list里输入的工作
            //cout << mdys.account_list.get_length() << endl;
            auto act = mdys.account_list.find(input);
            while (act == nullptr) {
                cout << "account not exist, please try again: ";
                getline(cin, input);
                act = mdys.account_list.find(input);
            }
            (act->data).set_map();
            inf.set_act(act->data);
            int i = 0;
            for (; i <= 4; i++) {
                cout << "Please enter the password" << "(you have " << 5 - i << " choices): ";
                getline(cin, password);
                //开始比较password和input,调用匹配函数
                log_flag = mdys.match(input, password);
                //验证成功则进入以input命名的账户(MDYS)里
                if (log_flag == true) {
                    cout << "Load Successfully!" << endl;
                    break;
                } else {
                    cout << "Wrong password, please try again!!(enter 'ENTER' to try again)" << endl;
                    cin.ignore(std::numeric_limits< streamsize >::max(), '\n');

                }
            }
            if (i == 5) {
                cout << "you have no permissions to load" << endl;
                return;
            }
            //验证密码的操作
            //我预计实现的操作是用一个专门的文件来存储用户的所有的密码，验证密码的时候就去打开这个专门的文件
            //遍历整个文件，当读取到一行相同的记录时表示验证身份成功
            //log_flag表时登录状态
        }
        //表示登录成功后
        if (log_flag == true) {

            while (exit_flag == false) {
                cout << "Enter the command >> ";
                getline(cin, input);
                for (auto it1 = input.begin(); it1 != input.end(); it1++) {
                    *it1 = tolower(*it1);
                }
                if (input.compare(exit) == 0) {
                    exit_flag = true;
                    continue;
                } else if (input == "")continue;
                else {
                    //进行数据库操作
                    database_operation(input);
                    cout << endl;
                    continue;
                }

                log_flag = false;
            }

        }
        return;

    }

    Interface *get_interface() {
        auto p = &inf;
        return p;
    }
};

