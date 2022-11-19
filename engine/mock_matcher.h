// Copyright 2022 CodeBuddies ASE Group
//
// Mock class of matcher for use by testers
//

#pragma once

#include <vector>

#include "gmock/gmock.h"

#include "engine/base_matcher.h"

using std::vector;

class MockMatcher : public BaseMatcher {
    MOCK_METHOD(vector<MatchedTrade>, get_matched_trades, (const vector<Trade>&, const Timestamp), (override));
    MOCK_METHOD(PNL, get_pnl_from, (Trade, Timestamp), (override));
    MOCK_METHOD(PNL, get_net_pnl, (const vector<Trade>&, Timestamp), (override));
    MOCK_METHOD(YearEndPNL, get_year_end_pnl, (const vector<Trade>&, Timestamp), (override));
    MOCK_METHOD(
        vector<SnapshotPNL>, get_pnl_snapshots, (const vector<Trade>&, Timestamp, vector<TimeDelta>), (override));
    MOCK_METHOD(vector<Trade>, get_earliest_long_term_sells, (const vector<Trade>&, const Timestamp), (override));
};
