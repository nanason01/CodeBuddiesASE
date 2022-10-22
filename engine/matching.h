//
// The header for all data processing
//

#pragma once

#include "common/types.h"
#include "pricer/pricer.h"

#include <vector>

using namespace std::chrono_literals;

// this does not need to be exact
// chrono::months do not convert to chrono::hours
std::vector<TimeDelta> DEFAULT_SAMPLES = {
    std::chrono::days(0),
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
    PNL lt_realized, st_realized;
    PNL actual;
};

struct SnapshotPNL {
    Timestamp timestamp;
    PNL pnl;
};

// get all matched trades for user
std::vector<MatchedTrade> get_matched_trades(const vector<Trade>& trades_in);

// get pnl of trade
// internally, this assumes the trade was net-0 value the first day
// then calculated the present value of both legs to get the result
PNL get_pnl_from(Trade trade, Timestamp end_time = now());

// get net pnl of a user up to end_date
PNL get_net_pnl(const vector<Trade>& trades, Timestamp end_time = now());

// get year end stats for a year
YearEndPNL get_year_end_pnl(const vector<Trade>& trades, std::chrono::time_point<std::chrono::system_clock> year);

// get pnl over various points in time
std::vector<SnapshotPNL> get_pnl_snapshots(const vector<Trade>& trades, std::vector<TimeDelta> timestamps = DEFAULT_SAMPLES);

// returns the earliest dates user could sell each of his cryptos for
// all of them to be considered long term cap gains
// Trade::bought_amount is meaningless as the future price is unknown
std::vector<Trade> get_earliest_long_term_sells(const vector<Trade>& trades, );
