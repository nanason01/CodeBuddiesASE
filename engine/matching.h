//
// The header for all data processing
//

#pragma once

#include "common/types.h"
#include "pricer/pricer.h"

#include <vector>

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

class Matcher {
    PricerBase* pricer;
public:

    Matcher(PricerBase* _pricer) : pricer(_pricer) {}
    virtual ~Matcher() = default;

    struct YearEndPNL {
        PNL lt_realized, st_realized;
        PNL actual;
    };

    struct SnapshotPNL {
        Timestamp timestamp;
        PNL pnl;
    };

    // get all matched trades for user
    virtual vector<MatchedTrade> get_matched_trades(const vector<Trade>& trades_in);

    // get pnl of trade
    // internally, this assumes the trade was net-0 value the first day
    // then calculated the present value of both legs to get the result
    virtual PNL get_pnl_from(Trade trade, Timestamp end_time = now());

    // get net pnl of a user up to end_date
    virtual PNL get_net_pnl(const vector<Trade>& trades, Timestamp end_time = now());

    // get year end stats for a year
    virtual YearEndPNL get_year_end_pnl(const vector<Trade>& trades, Timestamp year = now());

    // get pnl over various points in time
    virtual vector<SnapshotPNL> get_pnl_snapshots(const vector<Trade>& trades, vector<TimeDelta> timestamps = DEFAULT_SAMPLES);

    // returns the earliest dates user could sell each of his cryptos for
    // all of them to be considered long term cap gains
    // Trade::bought_amount is meaningless as the future price is unknown
    virtual vector<Trade> get_earliest_long_term_sells(const vector<Trade>& trades);

private:
    // get whether this is a short or long term trade
    static constexpr Term get_term(Timestamp buy, Timestamp sell);

    // This is the "conversion rate" between STCG and LTCG
    // when we have to decide  whether to take a small STCG
    // or a large LTCG
    // it compares the amount saved by deferring a STCG into a LTCG
    // versus the amount saved by deferring a LTCG
    // so saving 1 STCG dollar is worth saving ST_FIXING LTCG dollars
    // assumptions: STCG rate = 35%, LTCG rate = 20%, risk free rate = 3%
    // and the obvious assumption that not realizing a STCG will mean waiting
    // until it becomes a LTCG
    // In the future, the assumptions or this value can be set by a user
    static constexpr double ST_FIXING = (0.35 - (0.20 / (1.00 + 0.03))) /
        (0.20 - (0.20 / (1.00 + 0.03)));
};
