//
// Common types used between modules
//

#pragma once

#include <string>
#include <string_view>
#include <ctime>

enum class Exchange : uint8_t {
    Invalid,
    Coinbase,
    Crypto_com,
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
    std::time_t timestamp;

    std::string sold_currency, bought_currency;
    double sold_amount, bought_amount;
};

// Matched buy with sell swap
struct MatchedTrade {
    std::time_t bought_timestamp, sold_timestamp;
    Term term;

    std::string currency;
    double pnl;
};