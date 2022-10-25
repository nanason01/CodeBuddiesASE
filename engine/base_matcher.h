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
    from_cal(0,0,0),
    from_cal(0,7,0),
    from_cal(0,14,0),
    from_cal(0,21,0),
    from_cal(1,0,0),
    from_cal(2,0,0),
    from_cal(3,0,0),
    from_cal(6,0,0),
    from_cal(0,0,1),
    from_cal(0,0,2),
    from_cal(0,0,3),
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
    virtual vector<MatchedTrade> get_matched_trades(const vector<Trade>& trades_in, const Timestamp end_time = now()) = 0;
    virtual PNL get_pnl_from(Trade trade, Timestamp end_time = now()) = 0;
    virtual PNL get_net_pnl(const vector<Trade>& trades_in, Timestamp end_time = now()) = 0;
    virtual YearEndPNL get_year_end_pnl(const vector<Trade>& trades_in, Timestamp year = now()) = 0;
    virtual vector<SnapshotPNL> get_pnl_snapshots(const vector<Trade>& trades_in, vector<TimeDelta> timestamps = DEFAULT_SAMPLES) = 0;
    virtual vector<Trade> get_earliest_long_term_sells(const vector<Trade>& trades, const Timestamp end_time = now()) = 0;
};
