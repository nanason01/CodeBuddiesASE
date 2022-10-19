//
// Module for inputting new data and retrieving user data
//
// Every function will validate creds and can throw InvalidCreds
//

#pragma once

#include "common/types.h"
#include "exchanges/coinbase.h"
#include "exchanges/kraken.h"

#include <exception>
#include <chrono>

using namespace std::chrono_literals;

constexpr std::chrono::duration REFRESH_INTERVAL = 24h;

// TODO: subject to change on 0Auth exploration
struct AuthenticUser {
    User user;
    Creds creds;
};

// errors
struct UserNotFound : std::exception {
    const char* what() const noexcept override {
        return "User not found in system";
    }
};
struct UserExists : std::exception {
    const char* what() const noexcept override {
        return "User already in system";
    }
};
struct InvalidCreds : std::exception {
    const char* what() const noexcept override {
        return "Failed to authenticate user with given creds";
    }
};

// writing operations

// add a user to our system
// throws UserExists if user exists
void add_user(AuthenticUser user);

// remove a user from our system
// throws UserNotFound if user doesn't exist
void remove_user(AuthenticUser user);

// add an exchange for user
// may throw ExchangeDriver level errors
void register_exchange(AuthenticUser user, Exchange exch, API_key key);

// add a trade for user
void upload_trade(AuthenticUser user, Trade trade);

// reading operations

// get trades associated with user
// if exchange key is no longer valid, mark it as invalid and rethrow
// so consecutive calls will invalidate one exchange at a time until all
// remaining exchanges are valid, then return the valid list of trades
std::vector<Trade> get_trades(AuthenticUser user, Exchange e = Exchange::All);

// get exchanges associated with user
std::vector<Exchange> get_exchanges(AuthenticUser user);

// throws UserNotFound if user does not exist
void __check_user(AuthenticUser user);

// throws InvalidCreds if credentials don't match
void __check_creds(AuthenticUser user);

// get last time this exchange was updated for user
std::time_t __get_last_update(AuthenticUser user, Exchange e);
