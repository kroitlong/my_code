/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */
#undef NDEBUG

#include <cassert>

#include "parser.h"

int main() {
    std::vector<std::string> sqls = {
        "show tables;",
        "desc tb;",
        "create table tb (a int, b float, c char(4));",
        "drop table tb;",
        "create index tb(a);",
        "create index tb(a, b, c);",
        "drop index tb(a, b, c);",
        "drop index tb(b);",
        "insert into tb values (1, 3.14, 'pi');",
        "delete from tb where a = 1;",
        "update tb set a = 1, b = 2.2, c = 'xyz' where x = 2 and y < 1.1 and z > 'abc';",
        "select * from tb;",
        "select * from tb where x <> 2 and y >= 3. and z <= '123' and b < tb.a;",
        "select x.a, y.b from x, y where x.a = y.b and c = d;",
        "select x.a, y.b from x join y where x.a = y.b and c = d;",
        "exit;",
        "help;",
        "create table t(id int , time datetime);",
        "insert into t values(1, '2023-05-18 09:12:19');",
        "insert into t values(2, '2023-05-30 12:34:32');",
        "select *from t;",
        "delete from t where time = '2023-05-30 12:34:32';",
        "update t set id = 2023 where time = '2023-05-18 09:12:19';",
        "select *from t;",
        "create table t1(time datetime, temperature float);",
        "insert into t1 values('1999-07-07 12:30:00', 36.0);",
        "select * from t1;",
        "insert into t1 values('1999-1-07 12:30:00', 36.0);",
        "insert into t1 values('1999-13-07 12:30:00', 36.0);",
        "insert into t1 values('1999-00-07 12:30:00', 36.0);",
        "insert into t1 values('1999-07-00 12:30:00', 36.0);",
        "insert into t1 values('0001-07-10 12:30:00', 36.0);",
        "insert into t1 values('1999-02-30 12:30:00', 36.0);",
        "insert into t1 values('1999-02-28 12:30:61', 36.0);",
        "select * from t1;",
        "CREATE TABLE t(bid bigint, sid int);",
        "INSERT INTO t VALUES(372036854775807, 233421);",
        "INSERT INTO t VALUES(-922337203685477580, 124332);",
        "INSERT INTO t VALUES(1,114514);",
        "UPDATE t SET bid=2 where sid=114514;",
        "SELECT * FROM t;",
        "INSERT INTO t VALUES(9223372036854775809, 12345);",
        "SELECT * FROM t;",
        "update t set sid=114514 where bid=-922337203685477580;",
        "delete from t where bid=2;",
        "create table warehouse (id int, name char(8));",
        "create index warehouse (id);",
        "show index from warehouse;",
        "create index warehouse (id, name);",
        "show index from warehouse;",
        "drop index warehouse (id);",
        "drop index warehouse (id, name);",
        "show index from warehouse;",
        "",
    };
    for (auto &sql : sqls) {
        std::cout << sql << std::endl;
        YY_BUFFER_STATE buf = yy_scan_string(sql.c_str());
        assert(yyparse() == 0);
        if (ast::parse_tree != nullptr) {
            ast::TreePrinter::print(ast::parse_tree);
            yy_delete_buffer(buf);
            std::cout << std::endl;
        } else {
            std::cout << "exit/EOF" << std::endl;
        }
    }
    ast::parse_tree.reset();
    return 0;
}
