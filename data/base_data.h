// Copyright 2022 CodeBuddies ASE Group
//
// Interface for inputting new data and retrieving user data
//

#pragma once

#include <exception>
#include <string>
#include <vector>

#include "common/types.h"

const TimeDelta REFRESH_INTERVAL = from_cal(0, 1, 0);

struct AuthenticUser {
    User user;
    Creds creds;
    Refresh refrs;
    mutable bool validated = false;

    bool operator==(const AuthenticUser& other) const {
        return
            user == other.user &&
            creds == other.creds;
    }
};

// errors
struct SqlError : std::exception {
    const char* what() const noexcept override {
        return "Invalid SQL";
    }
};
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
    virtual ~BaseData() {}

    // writing operations

    // unconditionally execute a sql file
    // should only be called by admin
    virtual void exec_sql_file(const std::string& sql_filename) = 0;

    // add a user to our system
    // throws UserExists if user exists
    virtual void add_user(const AuthenticUser& user) = 0;

    // update the credentials of a user
    // throws if the refr key on old_user doesn't match
    // sets old_users creds and refrs to the new vals
    virtual void update_user_creds(
        const AuthenticUser& old_user,
        const Creds& new_creds,
        const Refresh& new_refrs) = 0;

    // remove a user from our system
    // throws UserNotFound if user doesn't exist
    virtual void remove_user(const AuthenticUser& user) = 0;

    // add an exchange for user
    // may throw ExchangeDriver level errors
    virtual void register_exchange(
        const AuthenticUser& user,
        Exchange exch,
        const API_key& pub_key,
        const API_key& pvt_key) = 0;

    // delete exchange for user
    // doesn't check whether user actually has exch registered
    virtual void delete_exchange(const AuthenticUser& user, Exchange exch) = 0;

    // add a trade for user
    virtual void upload_trade(const AuthenticUser& user, const Trade& trade) = 0;

    // reading operations

    // get exchanges associated with user
    virtual std::vector<Exchange> get_exchanges(const AuthenticUser& user) const = 0;

    // get trades associated with user
    // if exchange key is no longer valid, mark it as invalid and rethrow
    // so consecutive calls will invalidate one exchange at a time until all
    // remaining exchanges are valid, then return the valid list of trades
    virtual std::vector<Trade> get_trades(const AuthenticUser& user) const = 0;

    // throws UserNotFound if user does not exist
    // throws InvalidCreds if credentials don't match
    virtual void check_user(const AuthenticUser& user) const = 0;

    // get last time this exchange was updated for user
    virtual Timestamp get_last_update(const AuthenticUser& user, Exchange e) const = 0;
};
