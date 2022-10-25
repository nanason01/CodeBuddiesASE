
#include "data/sqlite_stuff/sqlite3.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <tuple>

using namespace std;

using Row = vector<string>;

struct DatabaseConnError : std::exception {
    const char* what() const noexcept override {
        return "Can't open database";
    }
};
struct SqlError : std::exception {
    const char* what() const noexcept override {
        return "sql error";
    }
};

// Convert a db_str to a T
template<typename T>
static T db_digest(string db_str);

template<>
string db_digest(string db_str) {
    return db_str;
}

template<typename T, typename... Ts>
static tuple<T, Ts...> _digest_strings(char** p_flds) {
    if constexpr (sizeof...(Ts) == 0)
        return make_tuple(db_digest<T>(*p_flds));
    else
        return tuple_cat(tie(db_digest<T>(*p_flds)), _digest_strings<Ts...>(p_flds + 1));
}

template<typename... Ts>
static tuple<Ts...> digest_strings(char** p_flds) {
    if constexpr (sizeof...(Ts) == 0)
        return tuple<>();
    else
        return _digest_strings<Ts...>(p_flds);
}

template<typename... Ts>
static int fill_row(void* vec_ptr, int n_flds, char** p_flds, char** p_cols) {
    // make sure we aren't accidentally dropping fields
    assert(n_flds == sizeof...(Ts));

    reinterpret_cast<vector<tuple<Ts...>>*>(vec_ptr)->push_back(
        digest_strings<Ts...>(p_flds)
    );

    return 0;
}

class DataConn {

    sqlite3* db_conn;

public:
    DataConn(string db_filename) {
        if (sqlite3_open(db_filename.c_str(), &db_conn) != SQLITE_OK)
            throw DatabaseConnError();
    }

    template<typename... Ts>
    vector<tuple<Ts...>> exec_sql(string sql) {
        vector<tuple<Ts...>> ret;
        char* err_msg;

        if (sqlite3_exec(db_conn, sql.c_str(), fill_row, &ret, &err_msg) != SQLITE_OK) {
            cerr << err_msg << endl;
            free(err_msg);
            throw SqlError();
        }

        return ret;
    }


};


int main() {
    DataConn dc("/Users/nick/Desktop/COMS/4156/CodeBuddiesASE/data/test_db");

    vector<tuple<string, string, string>> exp;
    const auto res = dc.exec_sql<string, string, string>("SELECT * FROM Test;");

    for (const auto row : res) {
        cout << get<0>(row) << " " << get<1>(row) << " " << get<2>(row);
        cout << "\n";
    }
}
