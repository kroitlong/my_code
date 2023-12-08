#include<iostream>
#include<unistd.h>
#include<string>
//#include<>GUI库，目前主流的有QT，MFC,Wxweight等
#include "present.h"
#include "MDYS.h"



using namespace std;
//---------------------------------------------------------------------------------
//定义全局的mdys对象，使全局所有的myds对象都是这个

int main(void) {
    //------------------------------------------------------------------------------
    cout << "\n" << "---------------------------------------------------------------------------------------------" << endl;
    cout << "Welcome to our Micro Database Management System , below is the Menu" << endl;
    cout << "--------------------------------------------I AM THE MENU-------------------------------------------" << endl;
    cout << "1.The capitalization of command is not required ,[]means optional,() is required,ignore <>" << endl;
    cout << "2.Below is the Command System >>" << endl;
    cout << "\t" << "[1].To Create a Database, please enter :                 Create Database <dbname>" << endl;
    cout << "\t" << "[2].To choose a specified Database, please enter :       Use <dbname>" << endl;
    cout << "\t" << "[3].To Delete a Database, please enter:                  Drop Database <dbname>" << endl;
    cout << "\t" << "[4].To Create a Table, please enter :                    Create table <tbname>(key type_of_key primary,value1 type_of_value1,...)" << endl;
    cout << "\t" << "[5].To Delect a Table, please enter :                    Drop table <tbname>" << endl;
    cout << "\t" << "[6].To Insert data in a table, please enter :            Insert <tbname> values(column1,column2...)" << endl;
    cout << "\t" << "[7].To Delete data in a table, please enter :            Delect from <tbname> [where<cond>]" << endl;
    cout << "\t" << "[8].To clear the whole table, please enter:              Delete from <tbnanme> " << endl;
    cout << "\t" << "[8].To check the entire table,please enter:              Select * from <tbname>" << endl;
    cout << "\t" << "[9].To select a specific column in table, please enter : Select <column> from <table>" << endl;
    cout << "\t" << "[10].To see the structure of the Database System, please enter:  Show" << endl;
    cout << "\t" << "[11].To quit, please enter exit " << endl;
    cout << endl;
    cout << "3.Attention,each value you insert shall be like this :data1,data2,data3... " << endl;
    cout << "4.Each time when operate with a Database, Please use the command \"Use\" to open a Database" << endl;
    cout << "5.you can check your account information in file '.account.idx'\n";
    cout << "---------------------------------------------------------------------------------------------------" << endl;


    char buffer[255];
    getcwd(buffer, sizeof(buffer));
    string buffer_s = buffer;
    //buffer_s获取成功，为当前工作目录的绝对路径
    //cout << buffer_s << endl;
    MDYS mdys(buffer_s);
    cout << mdys.basepath << endl;
    present l;
    l.set_MDYS(mdys);
    l.show();
    //------------------------------------------------------------------------------
    //以下是各个头文件的关系：
    /*
      MDYS-------->>DataBase-------->>table-------->>每行的数据
      管理           管理              管理
      ^              ^                ^
      *              *                *
      子类            子类              子类
      *              *                *         使用
      *----------->Dlist<-------------*<<<<<<---------container
    */
    return 0;
}