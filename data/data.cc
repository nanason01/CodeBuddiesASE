//
// Implementation of data.h
//

#include "data.h"

#include <exception>
#include <iostream>
#include <cassert>
#include <tuple>
#include <string>
#include <chrono>

using std::vector;
using std::string;
using std::tuple;
using std::get;
using std::tuple_cat;
using std::make_tuple;

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
    p_flds++; // suppress compiler warning unused param
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
        std::cerr << err_msg << std::endl;
        free(err_msg);
        throw SqlError{};
    }

    return ret;
}

void Data::exec_sql_file(const std::string& sql_filename) {
    std::ifstream fin(sql_filename.c_str());
    std::stringstream iss;
    iss << fin.rdbuf();

    exec_sql<>(db_conn, iss.str());
}

void Data::add_user(const AuthenticUser& user) {
    const string check_user_sql = "SELECT COUNT(*) FROM Users WHERE "
        "UserID = \'" + user.user + "\';";
    const auto check_user_res = exec_sql<int>(db_conn, check_user_sql);

    if (get<0>(check_user_res[0]) > 0)
        throw UserExists{};

    const string add_user_sql = "INSERT INTO Users VALUES "
        "(\'" + user.user + "\', \'" + user.creds + "\', \'" + user.refrToken + "\');";
    exec_sql<>(db_conn, add_user_sql);

    // we also need to make an entry in ExchangeKeys for their manual trades to be tracked
    const string insert_manual_key_sql = "INSERT INTO ExchangeKeys (UserID, ExchangeID) VALUES "
        "(\'" + user.user + "\', " + std::to_string(static_cast<int>(Exchange::Manual)) + ");";
    exec_sql<>(db_conn, insert_manual_key_sql);
}

void Data::update_user_creds(const AuthenticUser& user) {
    // updates the user credentials in db
    // check_refr needs to be called before this function is called
    // now update the creds and the refrToken
    const string update_user_creds_sql =
        "UPDATE Users "
        "SET Creds = \'" + user.creds + "\',"
        "Refrs = \'" + user.refrToken + "\'"
        "WHERE UserID = \'" + user.user + "\';";
    exec_sql<>(db_conn, update_user_creds_sql);
}

// remove a user from our system
// throws UserNotFound if user doesn't exist
void Data::remove_user(const AuthenticUser& user) {
    check_user(user);

    const string delete_sql = "DELETE FROM Users WHERE UserID = \'" + user.user + "\';";

    exec_sql<>(db_conn, delete_sql);
}

// returns whether this date is over 1 day old
// in this case, we should refresh
static bool is_stale(Timestamp ts) {
    return now() - ts > REFRESH_INTERVAL;
}

static string to_insert(const AuthenticUser& user, const Exchange exch, const Trade& tr) {
    return "(\'" + user.user + "\', " +
        std::to_string(static_cast<int>(exch)) + ", " +
        std::to_string(get_year(tr.timestamp)) + ", " +
        std::to_string(get_month(tr.timestamp)) + ", " +
        std::to_string(get_day(tr.timestamp)) + ", "
        "\'" + tr.bought_currency + "\', " +
        "\'" + tr.sold_currency + "\', " +
        std::to_string(tr.bought_amount) + ", " +
        std::to_string(tr.sold_amount) + ")";
}

void Data::update_exchange(const AuthenticUser& user, Exchange exch, API_key pub_key, API_key pvt_key) const {
    assert(exch != Exchange::Manual);
    assert(exch != Exchange::Invalid);

    // get current api key for this exchange
    const string check_sql = "SELECT PubKey, PvtKey FROM ExchangeKeys "
        "WHERE UserID = \'" + user.user + "\' AND ExchangeID = " + std::to_string(static_cast<int>(exch)) + ";";
    const auto check_res = exec_sql<string, string>(db_conn, check_sql);

    if (check_res.empty()) {
        assert(pub_key != "");
        assert(pvt_key != "");

        // if we don't have a key, insert it
        const string insert_key_sql =
            "INSERT INTO ExchangeKeys VALUES ("
            "\'" + user.user + "\', " +
            std::to_string(static_cast<int>(exch)) + ", "
            "\'" + pub_key + "\',"
            "\'" + pvt_key + "\'," +
            std::to_string(get_year(now())) + ", " +
            std::to_string(get_month(now())) + ", " +
            std::to_string(get_day(now())) +
            ");";

        exec_sql<>(db_conn, insert_key_sql);
    } else if (pub_key != "" &&
        (get<0>(check_res[0]) != pub_key || get<1>(check_res[0]) != pvt_key)) {
        // if we are overwriting a key, update it
        const string update_key_sql =
            "UPDATE ExchangeKeys "
            "SET PubKey = \'" + pub_key + "\',"
            "PvtKey = \'" + pvt_key + "\',"
            "LastUpdatedYear = " + std::to_string(get_year(now())) + ", "
            "LastUpdatedMonth = " + std::to_string(get_month(now())) + ", "
            "LastUpdatedDay = " + std::to_string(get_day(now())) + " "
            "WHERE UserID = " + user.user + " AND "
            "ExchangeID = " + std::to_string(static_cast<int>(exch)) + ";";

        exec_sql<>(db_conn, update_key_sql);
    } else {
        // otherwise, just use the existing key

        pub_key = get<0>(check_res[0]);
        pvt_key = get<1>(check_res[0]);
    }

    // remove any existing data for this exchange
    const string delete_sql = "DELETE FROM Trades WHERE "
        "UserID = \'" + user.user + "\' AND "
        "ExchangeID = " + std::to_string(static_cast<int>(exch)) + ";";
    exec_sql<>(db_conn, delete_sql);

    // then fetch data from the exchange and insert it
    vector<Trade> trades = get_driver(exch)->get_trades(pub_key, pvt_key);

    if (!trades.empty()) {
        string insert_sql = "INSERT INTO Trades VALUES";

        for (const Trade& tr : trades)
            insert_sql += "\n" + to_insert(user, exch, tr) + ",";

        // remove trailing "," add ';'
        insert_sql.pop_back();
        insert_sql.push_back(';');

        exec_sql<>(db_conn, insert_sql);
    }
}

// add an exchange for user
// may throw ExchangeDriver level errors
void Data::register_exchange(const AuthenticUser& user, Exchange exch, const API_key& pub_key, const API_key& pvt_key) {
    assert(exch != Exchange::Manual);
    assert(exch != Exchange::Invalid);

    check_user(user);

    update_exchange(user, exch, pub_key, pvt_key);
}

// deletes exchange for user
void Data::delete_exchange(const AuthenticUser& user, Exchange exch) {
    assert(exch != Exchange::Manual);
    assert(exch != Exchange::Invalid);

    check_user(user);

    const string delete_sql = "DELETE FROM ExchangeKeys "
        "WHERE UserID = \'" + user.user + "\' AND "
        "ExchangeID = " + std::to_string(static_cast<int>(exch)) +
        ";";
    exec_sql<>(db_conn, delete_sql);
}

// add a trade for user
void Data::upload_trade(const AuthenticUser& user, const Trade& trade) {
    check_user(user);

    const string insert_sql = "INSERT INTO Trades VALUES " +
        to_insert(user, Exchange::Manual, trade) + ";";
    exec_sql<>(db_conn, insert_sql);
}

// reading operations

// get trades associated with user
// if exchange key is no longer valid, mark it as invalid and rethrow
// so consecutive calls will invalidate one exchange at a time until all
// remaining exchanges are valid, then return the valid list of trades
std::vector<Trade> Data::get_trades(const AuthenticUser& user) const {
    check_user(user);

    for (const Exchange e : get_exchanges(user)) {
        if (is_stale(get_last_update(user, e)))
            update_exchange(user, e);
    }

    const string get_all_trades_sql = "SELECT "
        "TradeYear, TradeMonth, TradeDay, "
        "SoldCurrency, BoughtCurrency, "
        "SoldAmount, BoughtAmount "
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
vector<Exchange> Data::get_exchanges(const AuthenticUser& user) const {
    check_user(user);

    const string get_exch_sql = "SELECT ExchangeID FROM ExchangeKeys "
        "WHERE UserID = \'" + user.user + "\';";
    const auto get_exch_res = exec_sql<int>(db_conn, get_exch_sql);

    // I wish the formatting was a bit cleaner
    // exclude non-exchange "manual" uploads from results
    vector<Exchange> ret;
    for (const auto& [exch] : get_exch_res) {
        if (static_cast<Exchange>(exch) == Exchange::Manual) continue;

        ret.push_back(static_cast<Exchange>(exch));
    }
    return ret;
}

// throws UserNotFound if user does not exist
void Data::check_user(const AuthenticUser& user) const {
    if (user.validated)
        return;

    const string find_user_sql = "SELECT Creds FROM Users "
        "WHERE UserID = \'" + user.user + "\';";
    const auto find_user_res = exec_sql<Creds>(db_conn, find_user_sql);

    if (find_user_res.empty())
        throw UserNotFound{};
    if (user.creds != get<0>(find_user_res[0]))
        throw InvalidCreds{};

    // this prevents overhead from unnecessary check_user calls
    user.validated = true;
}

// throws UserNotFound if user does not exist
void Data::check_refr(const User& user, const Refresh& refr) const {
    const string find_refr_sql = "SELECT Refrs FROM Users "
        "WHERE UserID = \'" + user + "\';";
    const auto find_refr_res = exec_sql<Refresh>(db_conn, find_refr_sql);

    if (find_refr_res.empty())
        throw UserNotFound{};
    if (refr != get<0>(find_refr_res[0]))
        throw InvalidCreds{};
}

// get last time this exchange was updated for user
Timestamp Data::get_last_update(const AuthenticUser& user, Exchange e) const {
    assert(e != Exchange::Invalid);

    check_user(user);

    // we track manual trades, they are always up-to-date
    if (e == Exchange::Manual)
        return now();

    const string update_date_sql = "SELECT "
        "LastUpdatedYear, LastUpdatedMonth, LastUpdatedDay "
        "FROM ExchangeKeys WHERE UserID = \'" + user.user +
        "\' AND ExchangeID = " + std::to_string(static_cast<int>(e)) + ";";
    const auto update_date_res = exec_sql<int, int, int>(db_conn, update_date_sql);

    if (update_date_res.empty())
        return beginning_of_time();

    const auto& [year, month, day] = update_date_res[0];
    return from_usa_date(month, day, year);
}
