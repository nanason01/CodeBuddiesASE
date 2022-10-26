//
// Interface for inputting new data and retrieving user data
//

#pragma once

#include "common/types.h"

#include <exception>
#include <chrono>

using namespace std::chrono_literals;

constexpr TimeDelta REFRESH_INTERVAL = std::chrono::days{ 1 };

// TODO: subject to change on 0Auth exploration
struct AuthenticUser {
    User user;
    Creds creds;
    mutable bool validated = false;
};

// errors
struct DatabaseConnError : std::exception {
    const char* what() const noexcept override {
        return "Can't open database";
    }
};
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

class BaseData {
public:
    // writing operations

    // add a user to our system
    // throws UserExists if user exists
    virtual void add_user(const AuthenticUser& user) = 0;

    // remove a user from our system
    // throws UserNotFound if user doesn't exist
    virtual void remove_user(const AuthenticUser& user) = 0;

    // add an exchange for user
    // may throw ExchangeDriver level errors
    virtual void register_exchange(const AuthenticUser& user, Exchange exch, API_key pub_key, API_key pvt_key) = 0;

    // add a trade for user
    virtual void upload_trade(const AuthenticUser& user, const Trade& trade) = 0;

    // reading operations

    // get trades associated with user
    // if exchange key is no longer valid, mark it as invalid and rethrow
    // so consecutive calls will invalidate one exchange at a time until all
    // remaining exchanges are valid, then return the valid list of trades
    virtual std::vector<Trade> get_trades(const AuthenticUser& user) = 0;

    // get exchanges associated with user
    virtual std::vector<Exchange> get_exchanges(const AuthenticUser& user) = 0;

    // throws UserNotFound if user does not exist
    // throws InvalidCreds if credentials don't match
    virtual void check_user(const AuthenticUser& user) const = 0;

    // get last time this exchange was updated for user
    virtual Timestamp get_last_update(const AuthenticUser& user, Exchange e) const = 0;
};