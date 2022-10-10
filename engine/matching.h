//
// The header for all data processing
//

#pragma once

#include "common/types.h"
#include "data/user_data.h"

#include <vector>


using namespace std::chrono_literals;
using Timestamp = std::chrono::time_point<std::chrono::system_clock>;
using TimeDelta = std::chrono::hours;

// this does not need to be exact
// chrono::months do not convert to chrono::hours
std::vector<TimeDelta> DEFAULT_SAMPLES = {
    std::chrono::hours(0),
    std::chrono::weeks(1),
    std::chrono::weeks(2),
    std::chrono::weeks(3),
    std::chrono::weeks(4 * 1),
    std::chrono::weeks(4 * 2),
    std::chrono::weeks(4 * 3),
    std::chrono::weeks(4 * 4),
    std::chrono::weeks(4 * 5),
    std::chrono::weeks(4 * 6),
    std::chrono::days(365 * 1),
    std::chrono::days(365 * 2),
    std::chrono::days(365 * 3),
};

struct YearEndPNL {
    double lt_realized, st_realized;
    double actual;
};

struct SnapshotPNL {
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    double pnl;
};

// get all matched trades for user
std::vector<MatchedTrade> get_matched_trades(AuthenticUser user);

// get pnl of trade
// internally, this assumes the trade was net-0 value the first day
// then calculated the present value of both legs to get the result
MatchedTrade get_pnl_from(AuthenticUser user, Trade trade);

// get year end stats for a year
YearEndPNL get_year_end_pnl(AuthenticUser user, std::chrono::time_point<std::chrono::system_clock> year);

// get pnl over various points in time
std::vector<SnapshotPNL> get_pnl_snapshots(AuthenticUser user, std::vector<TimeDelta> timestamps = DEFAULT_SAMPLES);

// returns the earliest dates user could sell each of his cryptos for
// all of them to be considered long term cap gains
std::vector<Trade> get_earliest_long_term_sells(AuthenticUser user);