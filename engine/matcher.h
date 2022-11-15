// Copyright 2022 CodeBuddies ASE Group
//
// The header for all data processing
//

#pragma once

#include "base_matcher.h"
#include "common/types.h"
#include "pricer/pricer.h"

#include <vector>

using std::vector;

class Matcher: public BaseMatcher {
    Pricer _pricer;
    PricerBase* pricer;
public:

    Matcher(): pricer(&_pricer) {}

    // only for testing
    Matcher(PricerBase* _pricer_in): pricer(_pricer_in) {}

    // get all matched trades for user
    // note: no contract on condensing results,
    // a trade may be split into an arbitrary number of matched trades as long as they account for exactly the trade volume
    vector<MatchedTrade> get_matched_trades(const vector<Trade>& trades_in, const Timestamp end_time = now()) final;

    // get pnl of trade
    // internally, this assumes the trade was net-0 value the first day
    // then calculated the present value of both legs to get the result
    PNL get_pnl_from(Trade trade, Timestamp end_time = now()) final;

    // get net pnl of a user up to end_date
    PNL get_net_pnl(const vector<Trade>& trades, Timestamp end_time = now()) final;

    // get year end stats for a year
    YearEndPNL get_year_end_pnl(const vector<Trade>& trades, Timestamp year = now()) final;

    // get pnl over various points in time
    vector<SnapshotPNL> get_pnl_snapshots(const vector<Trade>& trades, vector<TimeDelta> timestamps = DEFAULT_SAMPLES) final;

    // returns the earliest dates user could sell each of his cryptos for
    // all of them to be considered long term cap gains
    // Trade::bought_amount is meaningless as the future price is unknown
    vector<Trade> get_earliest_long_term_sells(const vector<Trade>& trades, const Timestamp end_time = now()) final;

private:
    // get whether this is a short or long term trade
    static Term get_term(Timestamp buy, Timestamp sell);

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
