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
vector<tuple<Ts...>> exec_sql(sqlite3* db_conn, const string& sql) {
    vector<tuple<Ts...>> ret;
    char* err_msg;

    if (sqlite3_exec(db_conn, sql.c_str(), fill_row<Ts...>, &ret, &err_msg) != SQLITE_OK) {
        cerr << err_msg << endl;
        free(err_msg);
        throw SqlError();
    }

    return ret;
}

void Data::add_user(const AuthenticUser& user) {
    // @TODO Urvee define what creds are
    // this simply check that what we have stored as the "creds"
    // for this user is what is passed into this function

    // first, check whether user exists
    const string check_user_sql = "SELECT COUNT(*) FROM Users WHERE " +
        "UserID = \'" + user.user + "\';";
    const auto check_user_res = exec_sql<int>(db_conn, check_user_sql);

    if (get<0>(check_user_res[0]) > 0)
        throw UserExists{};

    const string add_user_sql = "INSERT INTO Users VALUES " +
        "(\'" + user.user + "\', \'" + user.creds + "\');";
    exec_sql<>(db_conn, add_user_sql);
}

static string get_delete_sql(const AuthenticUser& user, const string& table) {
    return "DELETE FROM " + table + " WHERE UserID = \'" + user.user + "\';";
}

// remove a user from our system
// throws UserNotFound if user doesn't exist
void Data::remove_user(const AuthenticUser& user) {
    check_user(user);

    exec_sql<>(db_conn, get_delete_sql(user, "Trades"));
    exec_sql<>(db_conn, get_delete_sql(user, "ExchangeKeys"));
    exec_sql<>(db_conn, get_delete_sql(user, "Users"));
}

// returns whether this date is over 1 day old
// in this case, we should refresh
static bool is_stale(Timestamp ts) {
    return now() - ts > from_cal(0, 1, 0);
}

static string to_insert(const AuthenticUser& user, const Trade& tr) {
    return "(\'" + user.user + "\', " +
        std::to_string(get_year(tr.timestamp)) + ", " +
        std::to_string(get_month(tr.timestamp)) + ", " +
        std::to_string(get_day(tr.timestamp)) + ", " +
        "\'" + tr.bought_currency + "\', " +
        "\'" + tr.sold_currency + "\', " +
        std::to_string(tr.bought_amount) + ", " +
        std::to_string(tr.sold_amount) + ")";
}

void Data::update_exchange(const AuthenticUser& user, Exchange exch, const API_key& key) {
    assert(exch != Exchange::All);
    assert(exch != Exchange::Invalid);

    // get current api key for this exchange
    const string check_sql = "SELECT APIKey FROM ExchangeKeys " +
        "UserID = \'" + user.user + "\' AND ExchangeID = " + std::to_string(exch) + ";";
    const auto check_res = exec_sql<string>(db_conn, check_sql);

    if (check_res.empty()) {
        assert(key != "");

        // if we don't have a key, insert it
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
    else if (key != "" && get<0>(check_res[0]) != key) {
        // if we are overwriting a key, update it
        const string update_key_sql =
            "UPDATE ExchangeKeys " +
            "SET APIKey = \'" + key + "\',"
            "LastUpdatedYear = " + std::to_string(get_year(now())) + ", "
            "LastUpdatedMonth = " + std::to_string(get_month(now())) + ", "
            "LastUpdatedDay = " + std::to_string(get_day(now())) + " " +
            "WHERE UserID = " + user.user + " AND " +
            "ExchangeID = " + std::to_string(exch) + ";";

        exec_sql<>(db_conn, update_key_sql);
    }

    // then fetch data from the exchange and insert it
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

    string insert_stmt = "INSERT INTO Trades VALUES ";
    for (const Trade& tr : trades)
        insert_stmt += to_insert(user, tr) + ",\n";
    // remove trailing "",\n" add ';'
    insert_stmt.pop_back();
    insert_stmt.pop_back();
    insert_stmt.push_back(';');
    exec_sql<>(db_conn, insert_stmt);
}

// add an exchange for user
// may throw ExchangeDriver level errors
void Data::register_exchange(const AuthenticUser& user, Exchange exch, const API_key& key) {
    assert(exch != Exchange::All);
    assert(exch != Exchange::Invalid);

    check_user(user);

    update_exchange(user, exch, key);
}

// add a trade for user
void Data::upload_trade(const AuthenticUser& user, const Trade& trade) {
    check_user(user);

    const string insert_sql = "INSERT INTO Trades VALUES " +
        to_insert(trade) + ";";
    exec_sql<>(db_conn, insert_sql);
}

// reading operations

// get trades associated with user
// if exchange key is no longer valid, mark it as invalid and rethrow
// so consecutive calls will invalidate one exchange at a time until all
// remaining exchanges are valid, then return the valid list of trades
std::vector<Trade> Data::get_trades(const AuthenticUser& user) {
    check_user(user);

    for (const Exchange e : get_exchanges(user)) {
        if (is_stale(get_last_update(user, e)))
            update_exchange(user, e);
    }

    const string get_all_trades_sql = "SELECT " +
        "TradeYear, TradeMonth, TradeDay, " +
        "BoughtCurrency, SoldCurrency, " +
        "BoughtAmount, SoldAmount " +
        "FROM Trades WHERE UserID = \'" + user.user + "\';";
    const auto get_all_trades_res =
        exec_sql<int, int, int, string, string, double, double>(
            db_conn, get_all_trades_sql
            );

    vector<Trade> ret;
    for (const auto& [yr, mo, da, bc, sc, ba, sa] : get_all_trades_res) {
        ret.push_back({
            from_usa_date(mo, da, yr),
            bc,
            sc,
            ba,
            sa
            });
    }
    return ret;
}

// get exchanges associated with user
vector<Exchange> Data::get_exchanges(const AuthenticUser& user) {
    check_user(user);

    const string get_exch_sql = "SELECT ExchangeID FROM ExchangeKeys " +
        "WHERE UserID = \'" + user.user + "\';";
    const auto get_exch_res = exec_sql<int>(db_conn, get_exch_sql);

    // I wish the formatting was a bit cleaner
    vector<Exchange> ret;
    for (const auto& [exch] : get_exch_res)
        ret.push_back(exch);
    return ret;
}

// throws UserNotFound if user does not exist
void Data::check_user(const AuthenticUser& user) {
    if (user.validated)
        return;

    const string find_user_sql = "SELECT Creds FROM Users " +
        "WHERE UserID = \'" + user.user + "\';";
    const auto find_user_res = exec_sql<Creds>(db_conn, find_user_sql);

    if (find_user_res.empty())
        throw UserNotFound{};
    if (user.creds != get<0>(find_user_res[0]))
        throw InvalidCreds{};

    // this prevents overhead from unnecessary check_user calls
    user.validated = true;
}

// get last time this exchange was updated for user
Timestamp Data::get_last_update(const AuthenticUser& user, Exchange e) {
    assert(e != Exchange::All);
    assert(e != Exchange::Invalid);

    check_user(user);

    const string update_date_sql = "SELECT " +
        "LastUpdatedYear, LastUpdatedMonth, LastUpdatedDay " +
        "FROM ExchangeKeys WHERE UserID = \'" + user.user +
        "\' AND ExchangeID = " + std::to_string(e) + ";";
    const auto update_date_res = exec_sql<int, int, int>(db_conn, update_date_sql);

    if (update_date_res.empty())
        return beginning_of_time();

    const auto& [year, month, day] = update_date_res[0];
    return from_usa_date(month, day, year);
}
