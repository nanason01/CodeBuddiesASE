
#include "data/sqlite_stuff/sqlite3.h"
#include <iostream>
#include <vector>
#include <string>

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

static int fill_row(void* vec_ptr, int n_flds, char** p_flds, char** p_cols) {
    reinterpret_cast<vector<Row>*>(vec_ptr)->push_back(
        Row(p_flds, p_flds + n_flds)
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

    vector<Row> exec_sql(string sql) {
        vector<Row> ret;
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

    const auto res = dc.exec_sql("SELECT * FROM Test;");

    for (const auto row : res) {
        for (const auto item : row) {
            cout << item << " ";
        }
        cout << "\n";
    }
}