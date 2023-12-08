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

#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "defs.h"
#include "record/rm_defs.h"
#include "parser/ast.h"

struct TabCol {
    std::string tab_name;
    std::string col_name;

    friend bool operator<(const TabCol &x, const TabCol &y) {
        return std::make_pair(x.tab_name, x.col_name) < std::make_pair(y.tab_name, y.col_name);
    }
};

struct Value {
    ColType type;  // type of value
    union {
        int int_val;      // int value
        float float_val;  // float value
    };
    std::string str_val;  // string value
    ast::datetime datetime_val;
    std::string datetime_val_str;
    ast::bigint bigint_val;

    std::shared_ptr<RmRecord> raw;  // raw record buffer

    void get_date_str(std::string *data_) {
        std::ostringstream oss;
        if (static_cast<int>(datetime_val.time[7]) == 0)
            throw InvalidValueError();
        oss << std::setw(2) << std::setfill('0') << static_cast<int>(datetime_val.time[0])
            << std::setw(2) << std::setfill('0') << static_cast<int>(datetime_val.time[1]) << "-"
            << std::setw(2) << std::setfill('0') << static_cast<int>(datetime_val.time[2]) << "-"
            << std::setw(2) << std::setfill('0') << static_cast<int>(datetime_val.time[3]) << " "
            << std::setw(2) << std::setfill('0') << static_cast<int>(datetime_val.time[4]) << ":"
            << std::setw(2) << std::setfill('0') << static_cast<int>(datetime_val.time[5]) << ":"
            << std::setw(2) << std::setfill('0') << static_cast<int>(datetime_val.time[6]);

        *data_ = oss.str();
    }

    void set_int(int int_val_) {
        type = TYPE_INT;
        int_val = int_val_;
    }

    void set_float(float float_val_) {
        type = TYPE_FLOAT;
        float_val = float_val_;
    }

    void set_str(std::string str_val_) {
        type = TYPE_STRING;
        str_val = std::move(str_val_);
    }

    void set_datetime(int8_t *time_) {
        type = TYPE_DATETIME;
        for (auto i = 0; i < sizeof(time_); i++) {
            datetime_val.time[i] = time_[i];
        }
        get_date_str(&datetime_val_str);
    }

    void set_bigint(int64_t bigint, bool is_legal) {
        type = TYPE_BIGINT;
        if (!is_legal) {
            throw InvalidValueError();
        }
        bigint_val.val = bigint;
    }


    void init_raw(int len) {
        assert(raw == nullptr);
        raw = std::make_shared<RmRecord>(len);
        if (type == TYPE_INT) {
            assert(len == sizeof(int));
            *(int *)(raw->data) = int_val;
        } else if (type == TYPE_FLOAT) {
            assert(len == sizeof(float));
            *(float *)(raw->data) = float_val;
        } else if (type == TYPE_STRING) {
            if (len < (int)str_val.size()) {
                throw StringOverflowError();
            }
            memset(raw->data, 0, len);
            memcpy(raw->data, str_val.c_str(), str_val.size());
        } else if (type == TYPE_DATETIME) {
            //assert(len == sizeof(ast::datetime));
            memset(raw->data, 0, sizeof(datetime_val.time));
            memcpy(raw->data, datetime_val.time, sizeof(datetime_val.time));
        } else if (type == TYPE_BIGINT) {
            assert(len == sizeof(bigint_val));
            *(int64_t *)(raw->data) = bigint_val.val;
        }

    }

    bool operator>(const Value &rhs) const {
        if (!check_type(type, rhs.type)) {
            throw IncompatibleTypeError(coltype2str(type), coltype2str(rhs.type));
        }
        switch (type) {
        case TYPE_INT:
            if (rhs.type == TYPE_INT) {
                return int_val > rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return int_val > rhs.float_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return int_val > rhs.bigint_val.val;
            }
        case TYPE_FLOAT:
            if (rhs.type == TYPE_INT) {
                return float_val > rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return float_val > rhs.float_val;
            }
        case TYPE_STRING:
            return strcmp(str_val.c_str(), rhs.str_val.c_str()) > 0 ? true : false;
        // return str_val > rhs.str_val;
        case TYPE_DATETIME:
            return strcmp(datetime_val_str.c_str(), rhs.datetime_val_str.c_str()) > 0 ? true : false;
        case TYPE_BIGINT:
            if (rhs.type == TYPE_INT) {
                return bigint_val.val > rhs.int_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return bigint_val.val > rhs.bigint_val.val;
            }
        }
        throw IncompatibleTypeError(coltype2str(type), coltype2str(rhs.type));
    }

    bool operator<(const Value &rhs) const {
        if (!check_type(type, rhs.type)) {
            throw IncompatibleTypeError(coltype2str(type), coltype2str(rhs.type));
        }

        switch (type) {
        case TYPE_INT:
            if (rhs.type == TYPE_INT) {
                return int_val < rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return int_val < rhs.float_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return int_val < rhs.bigint_val.val;
            }
        case TYPE_FLOAT:
            if (rhs.type == TYPE_INT) {
                return float_val < rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return float_val < rhs.float_val;
            }
        case TYPE_STRING:
            return strcmp(str_val.c_str(), rhs.str_val.c_str()) < 0 ? true : false;
        case TYPE_DATETIME:
            return strcmp(datetime_val_str.c_str(), rhs.datetime_val_str.c_str()) < 0 ? true : false;
        case TYPE_BIGINT:
            if (rhs.type == TYPE_INT) {
                return bigint_val.val < rhs.int_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return bigint_val.val < rhs.bigint_val.val;
            }
        }
        throw IncompatibleTypeError(coltype2str(type), coltype2str(rhs.type));
    }

    bool operator==(const Value &rhs) const {
        if (!check_type(type, rhs.type)) {
            return false;
        }

        switch (type) {
        case TYPE_INT:
            if (rhs.type == TYPE_INT) {
                return int_val == rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return int_val == rhs.float_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return int_val == rhs.bigint_val.val;
            }
        case TYPE_FLOAT:
            if (rhs.type == TYPE_INT) {
                return float_val == rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return float_val == rhs.float_val;
            }
        case TYPE_STRING:
            return strcmp(str_val.c_str(), rhs.str_val.c_str()) == 0 ? true : false;
        case TYPE_DATETIME:
            return strcmp(datetime_val_str.c_str(), rhs.datetime_val_str.c_str()) == 0 ? true : false;
        case TYPE_BIGINT:
            if (rhs.type == TYPE_INT) {
                return bigint_val.val == rhs.int_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return bigint_val.val == rhs.bigint_val.val;
            }
        }
        throw IncompatibleTypeError(coltype2str(type), coltype2str(rhs.type));
    }

    bool operator!=(const Value &rhs) const {
        return !(*this == rhs);
    }

    bool operator>=(const Value &rhs) const {
        if (!check_type(type, rhs.type)) {
            throw IncompatibleTypeError(coltype2str(type), coltype2str(rhs.type));
        }

        switch (type) {
        case TYPE_INT:
            if (rhs.type == TYPE_INT) {
                return int_val >= rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return int_val >= rhs.float_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return int_val >= rhs.bigint_val.val;
            }
        case TYPE_FLOAT:
            if (rhs.type == TYPE_INT) {
                return float_val >= rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return float_val >= rhs.float_val;
            }
        case TYPE_STRING:
            return strcmp(str_val.c_str(), rhs.str_val.c_str()) >= 0 ? true : false;
        case TYPE_DATETIME:
            return strcmp(datetime_val_str.c_str(), rhs.datetime_val_str.c_str()) >= 0 ? true : false;
        case TYPE_BIGINT:
            if (rhs.type == TYPE_INT) {
                return bigint_val.val >= rhs.int_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return bigint_val.val >= rhs.bigint_val.val;
            }
        }
        throw IncompatibleTypeError(coltype2str(type), coltype2str(rhs.type));
    }

    bool operator<=(const Value &rhs) const {
        if (!check_type(type, rhs.type)) {
            throw IncompatibleTypeError(coltype2str(type), coltype2str(rhs.type));
        }
        switch (type) {
        case TYPE_INT:
            if (rhs.type == TYPE_INT) {
                return int_val <= rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return int_val <= rhs.float_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return int_val <= rhs.bigint_val.val;
            }
        case TYPE_FLOAT:
            if (rhs.type == TYPE_INT) {
                return float_val <= rhs.int_val;
            } else if (rhs.type == TYPE_FLOAT) {
                return float_val <= rhs.float_val;
            }
        case TYPE_STRING:
            return strcmp(str_val.c_str(), rhs.str_val.c_str()) <= 0 ? true : false;
        case TYPE_DATETIME:
            return strcmp(datetime_val_str.c_str(), rhs.datetime_val_str.c_str()) <= 0 ? true : false;
        case TYPE_BIGINT:
            if (rhs.type == TYPE_INT) {
                return bigint_val.val <= rhs.int_val;
            } else if (rhs.type == TYPE_BIGINT) {
                return bigint_val.val <= rhs.bigint_val.val;
            }
        }
        throw IncompatibleTypeError(coltype2str(type), coltype2str(rhs.type));
    }
};


enum CompOp { OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE };

struct Condition {
    TabCol lhs_col;   // left-hand side column
    CompOp op;        // comparison operator
    bool is_rhs_val;  // true if right-hand side is a value (not a column)
    TabCol rhs_col;   // right-hand side column
    Value rhs_val;    // right-hand side value
};

struct SetClause {
    TabCol lhs;
    Value rhs;
};