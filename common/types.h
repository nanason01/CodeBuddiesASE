// Copyright 2022 CodeBuddies ASE Group
//
// Common types used between modules
//

#pragma once

#include <string_view>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>
#include <ctime>

// Defined inside home dir (~/)
constexpr auto CODEBUDDIES_DIR = "/CodeBuddies";

enum class Term : uint8_t {
    Short,
    Long,
    Held,
    UnmatchedSell,
};

constexpr const char* to_string(const Term term) {
    switch (term) {
    case(Term::Short):
        return "Short term";
    case(Term::Long):
        return "Long term";
    case(Term::Held):
        return "Held";
    case(Term::UnmatchedSell):
        return "Unmatched sell";
    default:
        return "Invalid";
    }
}

enum class Exchange : uint8_t {
    Invalid,
    Coinbase,
    Kraken,

    Manual = std::numeric_limits<uint8_t>::max(),
};

constexpr const char* to_string(const Exchange e) {
    switch (e) {
    case Exchange::Coinbase:
        return "Coinbase";
    case Exchange::Kraken:
        return "Kraken";
    case Exchange::Manual:
        return "Manual";
    default:
        return "invalid";
    }
}

constexpr const Exchange from_string(const std::string_view str) {
    if (str == "Coinbase")
        return Exchange::Coinbase;
    else if (str == "Kraken")
        return Exchange::Kraken;
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
    std::tm buf;
    std::tm* ts_tm = localtime_r(&ts, &buf);

    return ts_tm->tm_year + 1900;
}
inline int get_month(Timestamp ts) {
    std::tm buf;
    std::tm* ts_tm = localtime_r(&ts, &buf);

    return ts_tm->tm_mon + 1;
}
inline int get_day(Timestamp ts) {
    std::tm buf;
    std::tm* ts_tm = localtime_r(&ts, &buf);

    return ts_tm->tm_mday;
}

inline std::string to_string(const Timestamp& ts) {
    // yyyy-mm-dd
    const int yr = get_year(ts);
    const int mo = get_month(ts);
    const int da = get_day(ts);

    std::string yr_str = to_string(yr);
    std::string mo_str = to_string(mo);
    std::string da_str = to_string(da);

    while (yr_str.size() < 4)
        yr_str = "0" + yr_str;
    while (mo_str.size() < 2)
        mo_str = "0" + mo_str;
    while (da_str.size() < 2)
        da_str = "0" + da_str;

    return yr_str + "-" + mo_str + "-" + da_str;
}

// this is needed for gtest to print human-readable dates
namespace std {
    void PrintTo(const Timestamp& ts, std::ostream* os);
}

using PNL = double;

// Singular dated swap
struct Trade {
    Timestamp timestamp;

    std::string sold_currency, bought_currency;
    double sold_amount, bought_amount;

    friend std::ostream& operator<<(std::ostream& os, const Trade& tr);
    bool operator==(const Trade& other) const {
        return
            timestamp == other.timestamp &&
            sold_currency == other.sold_currency &&
            bought_currency == other.bought_currency &&
            sold_amount == other.sold_amount &&
            bought_amount == other.bought_amount;
    }
};

std::ostream& operator<<(std::ostream& os, const Trade& tr);

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
    bool operator==(const MatchedTrade& other) const {
        return
            bought_timestamp == other.bought_timestamp &&
            sold_timestamp == other.sold_timestamp &&
            term == other.term &&
            currency == other.currency &&
            sz == other.sz &&
            pnl == other.pnl;
    }
};

std::ostream& operator<<(std::ostream& os, const MatchedTrade& mt);

using User = std::string;
using Creds = std::string;
using Refresh = std::string;
using API_key = std::string;
