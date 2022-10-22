//
// Module for inputting new data and retrieving user data
//
// Every function will validate creds and can throw InvalidCreds
//

#pragma once

#include "base_data.h"

#include "common/types.h"
#include "exchanges/coinbase.h"
#include "exchanges/crypto_com.h"

#include <exception>
#include <chrono>

class Data final : public BaseData {
    const CoinbaseDriver cb_driver;
    const Crypto_comDriver cc_driver;
    const ExchangeDriver* cb_driver_ptr, * cc_driver_ptr;

    inline const ExchangeDriver* get_driver(const Exchange& e) {
        switch (e) {
        case (Exchange::Coinbase):
            return cb_driver_ptr;
        case (Exchange::Crypto_com):
            return cc_driver_ptr;
        default:
            throw;
        }
    }

public:
    // normal call
    Data()
        : cb_driver_ptr(&cb_driver), cc_driver_ptr(&cc_driver) {}
    // only for testing
    Data(ExchangeDriver* _cb, ExchangeDriver* _cc)
        : cb_driver_ptr(_cb), cc_driver_ptr(_cc) {}

    // writing operations

    // add a user to our system
    // throws UserExists if user exists
    void add_user(AuthenticUser user) final;

    // remove a user from our system
    // throws UserNotFound if user doesn't exist
    void remove_user(AuthenticUser user) final;

    // add an exchange for user
    // may throw ExchangeDriver level errors
    void register_exchange(AuthenticUser user, Exchange exch, API_key key) final;

    // add a trade for user
    void upload_trade(AuthenticUser user, Trade trade) final;

    // reading operations

    // get trades associated with user
    // if exchange key is no longer valid, mark it as invalid and rethrow
    // so consecutive calls will invalidate one exchange at a time until all
    // remaining exchanges are valid, then return the valid list of trades
    std::vector<Trade> get_trades(AuthenticUser user, Exchange e = Exchange::All) final;

    // get exchanges associated with user
    std::vector<Exchange> get_exchanges(AuthenticUser user) final;

    // throws UserNotFound if user does not exist
    void check_user(AuthenticUser user) final;

    // throws InvalidCreds if credentials don't match
    void check_creds(AuthenticUser user) final;

    // get last time this exchange was updated for user
    Timestamp get_last_update(AuthenticUser user, Exchange e) final;
};