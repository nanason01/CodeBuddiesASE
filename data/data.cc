//
// Implementation of data.h
//

#include "data.h"

#include "common/types.h"
#include "exchanges/coinbase.h"
#include "exchanges/crypto_com.h"

#include <exception>
#include <string>
#include <chrono>

using std::string;

// writing operations

// add a user to our system
// throws UserExists if user exists
void Data::add_user(AuthenticUser user) {
    // TODO: wait for @Urvee to define creds
}

// remove a user from our system
// throws UserNotFound if user doesn't exist
void Data::remove_user(AuthenticUser user) {
    check_user(user);

    const auto sql_beg = "DELETE FROM ";
    const auto sql_mid = " WHERE UserID = \'";
    const auto sql_end = "\'";

    string trades_sql = sql_beg + string("Trades") + sql_mid + user.user + sql_end;
    string exchange_sql = sql_beg + string("ExchangeKeys") + sql_mid + user.user + sql_end;
    string users_sql = sql_beg + string("Users") + sql_mid + user.user + sql_end;

    auto noop = [](void*, int, char**, char**) -> int {
        return 0;
    };

    char** errmsg = nullptr;

    if (sqlite3_exec(db_conn, trades_sql.c_str(), noop, nullptr, errmsg)) {
        delete errmsg;
        throw DatabaseConnError();
    }
    if (sqlite3_exec(db_conn, exchange_sql.c_str(), noop, nullptr, errmsg)) {
        delete errmsg;
        throw DatabaseConnError();
    }
    if (sqlite3_exec(db_conn, users_sql.c_str(), noop, nullptr, errmsg)) {
        delete errmsg;
        throw DatabaseConnError();
    }
}

// add an exchange for user
// may throw ExchangeDriver level errors
void Data::register_exchange(AuthenticUser user, Exchange exch, API_key key);

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