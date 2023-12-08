/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include <iostream>
#include <map>

// 此处重载了<<操作符，在ColMeta中进行了调用
template<typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
std::ostream & operator<<(std::ostream &os, const T &enum_val) {
    os << static_cast<int>(enum_val);
    return os;
}

template<typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
std::istream & operator>>(std::istream &is, T &enum_val) {
    int int_val;
    is >> int_val;
    enum_val = static_cast<T>(int_val);
    return is;
}

struct Rid {
    int page_no;
    int slot_no;

    friend bool operator==(const Rid &x, const Rid &y) {
        return x.page_no == y.page_no && x.slot_no == y.slot_no;
    }

    friend bool operator!=(const Rid &x, const Rid &y) {
        return !(x == y);
    }
};

enum ColType {
    TYPE_INT, TYPE_FLOAT, TYPE_STRING, TYPE_DATETIME, TYPE_BIGINT
};


// 定义了check type
inline bool check_type(ColType lhs, ColType rhs) {
    // int 和 float可以比较
    if ((lhs == TYPE_INT && rhs == TYPE_FLOAT) || (lhs == TYPE_FLOAT && rhs == TYPE_INT)) {
        return true;
    } else {
        return lhs == rhs;
    }
    //bigint 可以向下兼容int
    if (lhs == TYPE_BIGINT && rhs == TYPE_INT || lhs == TYPE_INT && rhs == TYPE_BIGINT) {
        return true;
    } else {
        return lhs == rhs;
    }
}

inline std::string coltype2str(ColType type) {
    std::map<ColType, std::string> m = {
        {TYPE_INT,    "INT"},
        {TYPE_FLOAT,  "FLOAT"},
        {TYPE_STRING, "STRING"},
        {TYPE_DATETIME, "DATETIME"},
        {TYPE_BIGINT, "BIGINT"}
    };
    return m.at(type);
}

class RecScan {
public:
    virtual ~RecScan() = default;

    virtual void next() = 0;

    virtual bool is_end() const = 0;

    virtual Rid rid() const = 0;
};
