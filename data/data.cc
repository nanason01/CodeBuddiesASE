//
// Implementation of data.h
//

#include "data.h"

#include "common/types.h"
#include "exchanges/coinbase.h"
#include "exchanges/crypto_com.h"

#include <exception>
#include <iostream>
#include <cassert>
#include <tuple>
#include <string>
#include <chrono>

using std::vector;
using std::string;
using std::tuple;

// various templating to clean up fetching data from the database

// Convert a db_str to a T
template<typename T>
static T db_digest(string db_str);

template<>
string db_digest(string db_str) {
    return db_str;
}

template<>
int db_digest(string db_str) {
    return stoi(db_str);
}

template<>
double db_digest(string db_str) {
    return stod(db_str);
}

template<typename T, typename... Ts>
static tuple<T, Ts...> _digest_strings(char** p_flds) {
    if constexpr (sizeof...(Ts) == 0)
        return make_tuple(db_digest<T>(*p_flds));
    else
        return tuple_cat(make_tuple(db_digest<T>(*p_flds)), _digest_strings<Ts...>(p_flds + 1));
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

// if there's some template error for this, you're probably using an unsupported datatype
// if there's some runtime error for this, make sure your field types match what's expected
// int will work in decimal fields, be careful with that
template<typename... Ts>
vector<tuple<Ts...>> exec_sql(sqlite3* db_conn, string sql) {
    vector<tuple<Ts...>> ret;
    char* err_msg;

    if (sqlite3_exec(db_conn, sql.c_str(), fill_row<Ts...>, &ret, &err_msg) != SQLITE_OK) {
        cerr << err_msg << endl;
        free(err_msg);
        throw SqlError();
    }

    return ret;
}

void Data::add_user(AuthenticUser user) {
    // TODO: wait for @Urvee to define creds
}

static string get_delete_sql(AuthenticUser user, string table) {
    return "DELETE FROM " + table + " WHERE UserID = \'" + user.user + "\';";
}

// remove a user from our system
// throws UserNotFound if user doesn't exist
void Data::remove_user(AuthenticUser user) {
    check_user(user);

    exec_sql<>(db_conn, get_delete_sql(user, "Trades"));
    exec_sql<>(db_conn, get_delete_sql(user, "ExchangeKeys"));
    exec_sql<>(db_conn, get_delete_sql(user, "Users"));
}

// returns whether this date is over 1 day old
// in this case, we should refresh
static bool is_stale(int year, int month, int day) {
    return now() - from_usa_date(month, day, year) > from_cal(0, 1, 0);
}

// add an exchange for user
// may throw ExchangeDriver level errors
void Data::register_exchange(AuthenticUser user, Exchange exch, API_key key) {
    check_user(user);

    // check if we already have this apikey for user
    // not an error
    const string check_sql = "SELECT APIKey FROM ExchangeKeys " +
        "UserID = \'" + user.user + "\' AND ExchangeID = " + std::to_string(exch) + ";";
    const auto check_res = exec_sql<string>(db_conn, check_sql);

    if (check_res.size() && get<0>(check_res[0]) == key)
        return;

    if (check_res.size()) {
        const string update_key_sql =
            "UPDATE ExchangeKeys " +
            "SET APIKey = \'" + key + "\',"
            "LastUpdatedYear = " + std::to_string(get_year(now())) + ", "
            "LastUpdatedMonth = " + std::to_string(get_month(now())) + ", "
            "LastUpdatedDay = " + std::to_string(get_day(now())) + " " +
            "WHERE UserID = " + user.user + " AND " +
            "ExchangeID = " + std::to_string(exch) + ";";

        exec_sql<>(db_conn, update_key_sql);
    } else {
        const string insert_key_sql =
            "INSERT INTO ExchangeKeys VALUES (" +
            "\'" + user.user + "\', " +
            std::to_string(exch) + ", " +
            std::to_string(get_year(now())) + ", "
            std::to_string(get_month(now())) + ", "
            std::to_string(get_day(now())) +
            ");";

        exec_sql<>(db_conn, insert_key_sql);
    }

    // TODO: wait for @Alek to change his interface to support:
    // vector<Trade> trades = get_driver(exch)->get_trades(user, key);
    vector<Trade> trades = { // tmp until TODO
        {
            entry_time,
            "DECR",
            "INCR",
            2.0,
            2.0,
        },
        {
            entry_time,
            "INCR",
            "DECR",
            1.0,
            1.0,
        }
    };

    if (trades.empty())
        return; // no need to insert anything

    string insert_stmt = "INSERT INTO Trades VALUES "

}

// add a trade for user
void Data::upload_trade(AuthenticUser user, Trade trade) final;

// reading operations

static Timestamp str_to_ts(std::string str) {
    // @TODO good luck
}

static int get_trades_cb(void* data, int num_fields, char** p_fields, char** p_col_names) {
    p_fields++; // discard the UserID
    reinterpret_cast<std::vector<Trade>*>(data)->push_back({
        str_to_ts(*p_fields++),
        *p_fields++,
        *p_fields++,
        std::stod(*p_fields++),
        std::stod(*p_fields++),
        });

    return 0;
}

// get trades associated with user
// if exchange key is no longer valid, mark it as invalid and rethrow
// so consecutive calls will invalidate one exchange at a time until all
// remaining exchanges are valid, then return the valid list of trades
std::vector<Trade> Data::get_trades(AuthenticUser user, Exchange e = Exchange::All) {
    check_user(user);

    const auto sql_beg = "SELECT * FROM Trades WHERE UserID = \'";
    const auto sql_end = "\'";

    string sql = sql_beg + user.user + sql_end;

    std::vector<Trade> ret;

    char** errmsg = nullptr;

    if (sqlite3_exec(db_conn, sql.c_str(), get_trades_cb, &ret, errmsg)) {
        delete errmsg;
        throw DatabaseConnError();
    }

    return ret;
}

// get exchanges associated with user
std::vector<Exchange> Data::get_exchanges(AuthenticUser user) final;

// throws UserNotFound if user does not exist
void Data::check_user(AuthenticUser user) final;

// throws InvalidCreds if credentials don't match
void Data::check_creds(AuthenticUser user) final;

// get last time this exchange was updated for user
Timestamp Data::get_last_update(AuthenticUser user, Exchange e) final;
