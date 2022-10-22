//
// Common types used between modules
//

#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <limits>
#include <iostream>
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
    Held,
    UnmatchedSell,
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

using Timestamp = std::chrono::year_month_day;
using TimeDelta = std::chrono::days;

static inline Timestamp now() {
    return std::chrono::floor<std::chrono::days>(
        std::chrono::system_clock::now()
        );
}

static inline Timestamp beginning_of_time() {
    using std::chrono::month;

    return Timestamp(month{ 1 } / 1 / 1970);
}

// courtesy of cppreference: operator sys_days
constexpr Timestamp normalize(Timestamp ymd) {
    ymd += std::chrono::months{ 0 }; // normalizes year and month
    return std::chrono::sys_days{ ymd }; // normalizes day
}

// appearently this is only defined for months and years
// the cpp std committee probably has an incredibly enlightened
// reasoning behind this, probably adding a day to a date is stupid
// I, for one, live most of my life on a per-year or per-month basis
// I go to class once a month, I'll get back to you within a month,
// and I live for the month-end.
constexpr Timestamp operator+(const Timestamp& ts, const TimeDelta& delta) {
    return normalize(Timestamp{ ts.year(), ts.month(), ts.day() + delta });
}
constexpr Timestamp operator+(const TimeDelta& delta, const Timestamp& ts) {
    return normalize(Timestamp{ ts.year(), ts.month(), ts.day() + delta });
}
constexpr Timestamp operator-(const Timestamp& ts, const TimeDelta& delta) {
    return normalize(Timestamp{ ts.year(), ts.month(), ts.day() + delta });
}

// The more I use this lib, the more I regret it
// CHECK THE ORDER, usa meaning month/day/year like Jan 1st 2020
constexpr inline Timestamp from_usa_date(const unsigned int& _m, const unsigned int& _d, const int& _y) {
    using std::chrono::year;
    using std::chrono::month;
    using std::chrono::day;
    return Timestamp(year{ _y }, month{ _m }, day{ _d });
}

// this is needed for gtest to print human-readable dates
namespace std::chrono {
    void PrintTo(const Timestamp& ts, std::ostream* os) {
        if (!ts.ok()) {
            *os << "Invalid timestamp";
            return;
        }

        auto year = static_cast<int>(ts.year());
        auto month = static_cast<unsigned>(ts.month());
        auto day = static_cast<unsigned>(ts.day());

        *os << month << "/" << day << "/" << year;
    }
}

using PNL = double;

// Singular dated swap
struct Trade {
    Timestamp timestamp;

    std::string sold_currency, bought_currency;
    double sold_amount, bought_amount;
};

// Matched buy with sell swap
// if term == Term::Held, the position is still open,
// and therefore the sold_timestamp is invalid
// if term == Term::UnmatchedSell, the position was
// closed without a corresponding open, so the bought_timestamp
// and pnl are therefore invalid
struct MatchedTrade {
    Timestamp bought_timestamp, sold_timestamp;
    Term term;

    std::string currency;
    double sz;
    PNL pnl;
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
