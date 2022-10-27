//
// Module for inputting new data and retrieving user data
//
// Every function will validate creds and can throw InvalidCreds
//

#pragma once

#include "base_data.h"

#include "common/types.h"
#include "exchanges/coinbase.h"
#include "exchanges/kraken.h"

#include "sqlite3.h"
#include <exception>
#include <chrono>

constexpr auto DB_FILENAME = "db";

class Data final: public BaseData {
    CoinbaseDriver cb_driver;
    KrakenDriver k_driver;
    ExchangeDriver* const cb_driver_ptr, * const k_driver_ptr;

    sqlite3* db_conn;

    inline ExchangeDriver* get_driver(const Exchange& e) const {
        switch (e) {
        case (Exchange::Coinbase):
            return cb_driver_ptr;
        case (Exchange::Kraken):
            return k_driver_ptr;
        default:
            throw;
        }
    }

    // in this meaningless const scheme, Trades is "mutable"
    // as it is subjectively not changing anything we have to
    // pull and store trades that happened on some other exchange

    void update_exchange(const AuthenticUser& user, Exchange exch,
        API_key pub_key = "", API_key pvt_key = "") const;

public:
    // normal call
    Data()
        : cb_driver_ptr(&cb_driver), k_driver_ptr(&k_driver) {
        if (sqlite3_open(DB_FILENAME, &db_conn) != SQLITE_OK)
            throw DatabaseConnError();
    }
    // only for testing
    Data(ExchangeDriver* _cb, ExchangeDriver* _k, const std::string& test_db_filename)
        : cb_driver_ptr(_cb), k_driver_ptr(_k) {
        if (sqlite3_open(test_db_filename.c_str(), &db_conn) != SQLITE_OK)
            throw DatabaseConnError();
    }

    ~Data() override {
        sqlite3_close(db_conn);
    }

    // writing operations

    // add a user to our system
    // throws UserExists if user exists
    void add_user(const AuthenticUser& user) final;

    // remove a user from our system
    // throws UserNotFound if user doesn't exist
    void remove_user(const AuthenticUser& user) final;

    // add an exchange for user
    // may throw ExchangeDriver level errors
    void register_exchange(const AuthenticUser& user, Exchange exch,
        const API_key& pub_key, const API_key& pvt_key) final;

    // delete exchange for user
    // doesn't check whether user actually has exch registered
    void delete_exchange(const AuthenticUser& user, Exchange exch) final;

    // add a trade for user
    void upload_trade(const AuthenticUser& user, const Trade& trade) final;

    // reading operations

    // get exchanges associated with user
    std::vector<Exchange> get_exchanges(const AuthenticUser& user) const final;

    // get trades associated with user
    // if exchange key is no longer valid, mark it as invalid and rethrow
    // so consecutive calls will invalidate one exchange at a time until all
    // remaining exchanges are valid, then return the valid list of trades
    std::vector<Trade> get_trades(const AuthenticUser& user) const final;

    // throws UserNotFound if user does not exist
    // throws InvalidCreds if credentials don't match
    void check_user(const AuthenticUser& user) const final;

    // throws UserNotFound if user does not exist
    // throws InvalidCreds if refresh key doesn't match
    void check_refr(const User& user, const Refresh& refr_key) const final;

    // get last time this exchange was updated for user
    Timestamp get_last_update(const AuthenticUser& user, Exchange e) const final;
};
