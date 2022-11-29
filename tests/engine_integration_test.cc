// Copyright 2022 CodeBuddies ASE Group
//
// Integration tests for engine, using real pricer
//

#include <gtest/gtest.h>

#include <typeinfo>
#include <iostream>

#include "common/types.h"
#include "engine/matcher.h"

const std::vector<Trade> sample_trades = {
    {
        from_usa_date(8, 10, 2018),
        "ETH",
        "BTC",
        19.608,
        1.0,
    },
    {
        from_usa_date(9, 11, 2020),
        "BTC",
        "ETH",
        0.5,
        13.514,
    },
};

// so the expected output shouldn't change if you run on different dates
const Timestamp sample_end = from_usa_date(11, 18, 2022);

TEST(EngineIntegration, GetMatchedTrades) {
    Matcher m;
    std::vector<MatchedTrade> res;

    try {
        res = m.get_matched_trades(sample_trades);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }

    for (const auto& mt : res)
        std::cout << mt << std::endl;
}

TEST(EngineIntegration, GetPNLFrom) {
    Matcher m;
    PNL res1, res2;

    try {
        res1 = m.get_pnl_from(sample_trades[ 0 ], sample_end);
        res2 = m.get_pnl_from(sample_trades[ 1 ], sample_end);
        std::cout << "res1: " << res1 << " res2: " << res2 << std::endl;
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }
}

TEST(EngineIntegration, GetNetPNL) {
    Matcher m;
    PNL res;

    try {
        res = m.get_net_pnl(sample_trades, sample_end);
        std::cout << "res: " << res << std::endl;
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }
}

TEST(EngineIntegration, GetYearEndPNL) {
    Matcher m;
    YearEndPNL res;

    try {
        res = m.get_year_end_pnl(sample_trades, sample_end);
        std::cout << "actual: " << res.actual << " lt: " << res.lt_realized << " st: " << res.st_realized << std::endl;
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }
}

TEST(EngineIntegration, GetPNLSnapshots) {
    Matcher m;
    std::vector<SnapshotPNL> res;

    try {
        res = m.get_pnl_snapshots(sample_trades, sample_end);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }

    for (const auto& snapshot : res) {
        std::cout << "pnl: " << snapshot.pnl << " ts: " << snapshot.timestamp << std::endl;
    }
}

TEST(EngineIntegration, GetEarliestLongTermSells) {
    Matcher m;
    std::vector<Trade> res;

    try {
        res = m.get_earliest_long_term_sells(sample_trades, sample_end);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }

    for (const auto& lt_sell : res) {
        std::cout << lt_sell << std::endl;
    }
}
