//
// Mock class of matcher for use by testers
//

#pragma once

#include "base_matcher.h"
#include "gmock/gmock.h"

using std::vector;

class MockMatcher : public BaseMatcher {
    MOCK_METHOD(vector<MatchedTrade>, get_matched_trades, (const vector<Trade>& trades_in), (override));
    MOCK_METHOD(PNL, get_pnl_from, (Trade trade, Timestamp end_time), (override));
    MOCK_METHOD(PNL, get_net_pnl, (const vector<Trade>& trades_in, Timestamp end_time), (override));
    MOCK_METHOD(YearEndPNL, get_year_end_pnl, (const vector<Trade>& trades_in, Timestamp year), (override));
    MOCK_METHOD(vector<SnapshotPNL>, get_pnl_snapshots, (const vector<Trade>& trades_in, vector<TimeDelta> timestamps), (override));
    MOCK_METHOD(vector<Trade>, get_earliest_long_term_sells, (const vector<Trade>& trades), (override));
};