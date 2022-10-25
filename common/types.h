//
// Common types used between modules
//

#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <limits>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

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

using Timestamp = std::time_t;
using TimeDelta = std::time_t;

static inline Timestamp now() {
    return std::time(nullptr);
}
static inline Timestamp beginning_of_time() {
    return 0;
}

// CHECK THE ORDER, usa meaning month/day/year like Jan 1st 2020
inline Timestamp from_usa_date(const int& _m, const int& _d, const int& _y) {
    std::tm time{};

    time.tm_year = _y - 1900;
    time.tm_mon = _m - 1;
    time.tm_mday = _d;
    time.tm_hour = 12;
    time.tm_min = 0;
    time.tm_isdst = 0;

    return std::mktime(&time);
}

// returns a timedelta representing this many days
inline TimeDelta from_cal(const int& _m, const int& _d, const int& _y) {
    return ((((12 * _y) + _m) * 30) + _d) * 24 * 60 * 60;
}

inline int get_year(Timestamp ts) {
    std::tm* ts_tm = localtime(&ts);

    return ts_tm->tm_year + 1900;
}
inline int get_month(Timestamp ts) {
    std::tm* ts_tm = localtime(&ts);

    return ts_tm->tm_mon + 1;
}
inline int get_day(Timestamp ts) {
    std::tm* ts_tm = localtime(&ts);

    return ts_tm->tm_mday;
}

// this is needed for gtest to print human-readable dates
namespace std {
    void PrintTo(const Timestamp& ts, std::ostream* os) {
        std::tm* cal_ts = localtime(&ts);

        *os << std::put_time(cal_ts, "%c %Z");
    }
}

using PNL = double;

// Singular dated swap
struct Trade {
    Timestamp timestamp;

    std::string sold_currency, bought_currency;
    double sold_amount, bought_amount;

    friend std::ostream& operator<<(std::ostream& os, const Trade& tr);
};

std::ostream& operator<<(std::ostream& os, const Trade& tr) {
    os << "trade at ";
    std::PrintTo(tr.timestamp, &os);
    os << " exchanged " << tr.sold_amount << " " <<
        tr.sold_currency << " for " << tr.bought_amount <<
        tr.bought_currency;

    return os;
}

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

    friend std::ostream& operator<<(std::ostream& os, const MatchedTrade& mt);
    bool operator==(const MatchedTrade& other) const = default;
};

std::ostream& operator<<(std::ostream& os, const MatchedTrade& mt) {
    switch (mt.term) {
    case(Term::Short):
        os << "Short term " << mt.sz << ":" << mt.currency <<
            " bought at ";
        std::PrintTo(mt.bought_timestamp, &os);
        os << " sold at ";
        std::PrintTo(mt.sold_timestamp, &os);
        os << " pnl: " << mt.pnl;
        break;
    case(Term::Long):
        os << "Long term " << mt.sz << ":" << mt.currency <<
            " bought at ";
        std::PrintTo(mt.bought_timestamp, &os);
        os << " sold at ";
        std::PrintTo(mt.sold_timestamp, &os);
        os << " pnl: " << mt.pnl;
        break;
    case(Term::UnmatchedSell):
        os << "Unmatched Sell " << mt.sz << ":" << mt.currency <<
            " sold at ";
        std::PrintTo(mt.sold_timestamp, &os);
        os << " pnl: " << mt.pnl;
        break;
    case(Term::Held):
        os << "Held " << mt.sz << ":" << mt.currency <<
            " bought at ";
        std::PrintTo(mt.bought_timestamp, &os);
        os << " pnl: " << mt.pnl;
        break;
    }

    return os;
}

// TODO define these types with 0Auth
using User = std::string;
using Creds = std::string;
using API_key = std::string;
