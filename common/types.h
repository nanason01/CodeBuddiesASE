//
// Common types used between modules
//

#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <limits>
#include <chrono>

enum class Exchange : uint8_t {
    Invalid,
    Coinbase,
    Crypto_com,

    All = std::numeric_limits<uint8_t>::max(),
};

enum class Term : uint8_t {
    Short,
    Long,
};

constexpr const char* to_string(const Exchange e) {
    switch (e) {
    case Exchange::Coinbase:
        return "Coinbase";
    case Exchange::Crypto_com:
        return "crypto.com";
    default:
        return "invalid";
    }
}

constexpr const Exchange from_string(const std::string_view str) {
    if (str == "Coinbase")
        return Exchange::Coinbase;
    else if (str == "crypto.com")
        return Exchange::Crypto_com;
    else
        return Exchange::Invalid;
}

// Convenience unary + "unpack" operator to string
constexpr const char* operator+(const Exchange e) {
    return to_string(e);
}

// Singular dated swap
struct Trade {
    std::chrono::time_point<std::chrono::system_clock> timestamp;

    std::string sold_currency, bought_currency;
    double sold_amount, bought_amount;
};

// Matched buy with sell swap
struct MatchedTrade {
    std::chrono::time_point<std::chrono::system_clock> bought_timestamp, sold_timestamp;
    Term term;

    std::string currency;
    double pnl;
};

// TODO define these types with 0Auth
using User = std::string;
using API_key = std::string;
using Creds = std::string;

// Interface for getting trades from a source
class Driver {
protected:
    User user;
public:
    Driver(User _user) : user(_user) {}
    virtual ~Driver() = default;

    // TODO: add a more fine-grained option
    virtual std::vector<Trade> get_trades() = 0;
};