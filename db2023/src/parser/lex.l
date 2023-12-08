    /* keywords are case insensitive */
%option caseless
    /* we don't need yywrap() function */
%option noyywrap
    /* we don't need yyunput() function */
%option nounput
    /* we don't need input() function */
%option noinput
    /* enable location */
%option bison-bridge
%option bison-locations

%{
#include "ast.h"
#include "yacc.tab.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>


// automatically update location
#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line; \
    yylloc->first_column = yylloc->last_column; \
    for (int i = 0; yytext[i] != '\0'; i++) { \
        if(yytext[i] == '\n') { \
            yylloc->last_line++; \
            yylloc->last_column = 1; \
        } else { \
            yylloc->last_column++; \
        } \
    }

    //判断是不是闰年
    bool isLeapYear(int16_t year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return true;
    } else {
        return false;
    }
    }

    ast::datetime parse_datetime(const std::string &datetime_str) {
    int16_t year;
    int8_t year_h;
    int8_t year_l;
    int8_t month;
    int8_t day;
    int8_t hour;
    int8_t minute;
    int8_t second;

    if (datetime_str.length() != 21) {
        return ast::datetime();
    }

    year_h = static_cast<int8_t>(stoi(datetime_str.substr(1,2)));
    // 定位第一个'-'
    size_t pos = datetime_str.find('-');
    year_l = static_cast<int8_t>(stoi(datetime_str.substr(3, 2)));

    // 定位第二个'-'
    size_t pos2 = datetime_str.find('-', pos + 1);
    month = static_cast<int8_t>(stoi(datetime_str.substr(pos + 1, 2)));

    // 定位第一个' '
    size_t pos3 = datetime_str.find(' ');
    day = static_cast<int8_t>(stoi(datetime_str.substr(pos2 + 1, 2)));

    // 定位第一个':'
    size_t pos4 = datetime_str.find(':', pos3 + 1);
    hour = static_cast<int8_t>(stoi(datetime_str.substr(pos3 + 1, 2)));

    // 分割分、秒
    minute = static_cast<int8_t>(stoi(datetime_str.substr(pos4 + 1, 2)));
    second = static_cast<int8_t>(stoi(datetime_str.substr(pos4 + 4, 2)));

    //获得完整年并判断是否为闰年
    year = stoi(datetime_str.substr(1,pos-1));
    bool is_LeapYear=isLeapYear(year);

    // Check validity of datetime
    if (static_cast<int>(hour) < 0 || static_cast<int>(hour) > 23 ||
        static_cast<int>(minute) < 0 || static_cast<int>(minute) > 59 || 
        static_cast<int>(second) < 0 || static_cast<int>(second) > 59) {
        return ast::datetime();
    }
    if(year < 1000 || year > 9999 || month < 1 || month > 12){
        return ast::datetime();
    }
    if( static_cast<int>(day) < 1 || static_cast<int>(day)> 31){
        return ast::datetime();
    }
    if( is_LeapYear && static_cast<int>(month) == 2 && static_cast<int>(day) != 29){
        return ast::datetime();
    }
    if( !is_LeapYear && static_cast<int>(month) == 2 && static_cast<int>(day) != 28){
        return ast::datetime();
    }

    return ast::datetime(year_h, year_l, month, day, hour, minute, second);
    }

%}

alpha [a-zA-Z]
digit [0-9]
white_space [ \t]+
new_line "\r"|"\n"|"\r\n"
sign "+"|"-"
identifier {alpha}(_|{alpha}|{digit})*
value_int {sign}?{digit}+
value_float {sign}?{digit}+\.({digit}+)?
value_datetime '([0-9]{1,4})-(0?[0-9]|[1-9][0-9])-(0?[0-9]|[1-9][0-9])[ \t]+(0?[0-9]|[1-9][0-9]):(0?[0-9]|[1-9][0-9]):(0?[0-9]|[1-9][0-9])'
value_string '[^']*'
single_op ";"|"("|")"|","|"*"|"="|">"|"<"|"."

%x STATE_COMMENT

%%
    /* block comment */
"/*" { BEGIN(STATE_COMMENT); }
<STATE_COMMENT>"*/" { BEGIN(INITIAL); }
<STATE_COMMENT>[^*] { /* ignore the text of the comment */ }
<STATE_COMMENT>\* { /* ignore *'s that aren't part of */ }
    /* single line comment */
"--".* { /* ignore single line comment */ }
    /* white space and new line */
{white_space} { /* ignore white space */ }
{new_line} { /* ignore new line */ }
    /* keywords */
"SHOW" { return SHOW; }
"BEGIN" { return TXN_BEGIN; }
"COMMIT" { return TXN_COMMIT; }
"ABORT" { return TXN_ABORT; }
"ROLLBACK" { return TXN_ROLLBACK; }
"TABLES" { return TABLES; }
"CREATE" { return CREATE; }
"TABLE" { return TABLE; }
"DROP" { return DROP; }
"DESC" { return DESC; }
"INSERT" { return INSERT; }
"INTO" { return INTO; }
"VALUES" { return VALUES; }
"DELETE" { return DELETE; }
"FROM" { return FROM; }
"WHERE" { return WHERE; }
"UPDATE" { return UPDATE; }
"SET" { return SET; }
"SELECT" { return SELECT; }
"INT" { return INT; }
"CHAR" { return CHAR; }
"FLOAT" { return FLOAT; }
"DATETIME" { return DATETIME;}
"BIGINT" { return BIGINT; }
"INDEX" { return INDEX; }
"AND" { return AND; }
"JOIN" {return JOIN;}
"EXIT" { return EXIT; }
"HELP" { return HELP; }
"ORDER" { return ORDER; }
"BY" {  return BY;  }
"ASC" { return ASC; }
    /* operators */
">=" { return GEQ; }
"<=" { return LEQ; }
"<>" { return NEQ; }
{single_op} { return yytext[0]; }
    /* id */
{identifier} {
    yylval->sv_str = yytext;
    return IDENTIFIER;
}
    /* literals */
{value_int} {
    auto tmp = atoll(yytext);
    std::string int64_t_up_range = std::to_string(INT64_MAX);
    std::string int64_t_low_range = std::to_string(INT64_MIN);
    if(tmp > INT32_MAX || tmp < INT32_MIN){
        yylval->sv_bigint.val = static_cast<int64_t>(atoll(yytext));
        yylval->sv_bigint.is_legal=1;
        if ((yylval->sv_bigint.val > 0 && strcmp(int64_t_up_range.c_str() , yytext) < 0) ||
         (yylval->sv_bigint.val < 0 && strcmp(int64_t_low_range.c_str() , yytext) < 0)){
            yylval->sv_bigint.is_legal=0;
        }
        return VALUE_BIGINT;
    }else{
        yylval->sv_int = atoi(yytext);
        return VALUE_INT;
    }
}
{value_float} {
    yylval->sv_float = atof(yytext);
    return VALUE_FLOAT;
}
{value_datetime} {
    yylval->sv_datetime = parse_datetime(std::string(yytext));
    return VALUE_DATETIME;
 }
{value_string} {
    yylval->sv_str = std::string(yytext + 1, strlen(yytext) - 2);
    return VALUE_STRING;
}
    /* EOF */
<<EOF>> { return T_EOF; }
    /* unexpected char */
. { std::cerr << "Lexer Error: unexpected character " << yytext[0] << std::endl; }
%%
