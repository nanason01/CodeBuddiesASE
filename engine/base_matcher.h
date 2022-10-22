//
// Mock class of matcher for use by testers
//

#pragma once

#include "common/types.h"
#include "pricer/base_pricer.h"

using std::vector;

// this does not need to be exact
// chrono::months do not convert to chrono::hours
static const vector<TimeDelta> DEFAULT_SAMPLES = {
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
class BaseMatcher {
public:
    virtual ~BaseMatcher() = default;
    virtual vector<MatchedTrade> get_matched_trades(const vector<Trade>& trades_in) = 0;
    virtual PNL get_pnl_from(Trade trade, Timestamp end_time = now()) = 0;
    virtual PNL get_net_pnl(const vector<Trade>& trades_in, Timestamp end_time = now()) = 0;
    virtual YearEndPNL get_year_end_pnl(const vector<Trade>& trades_in, Timestamp year = now()) = 0;
    virtual vector<SnapshotPNL> get_pnl_snapshots(const vector<Trade>& trades_in, vector<TimeDelta> timestamps = DEFAULT_SAMPLES) = 0;
    virtual vector<Trade> get_earliest_long_term_sells(const vector<Trade>& trades) = 0;
};
