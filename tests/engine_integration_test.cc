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
    } catch (const RateLimitedQuery& rq) {
        std::cerr << "rate limited, redo later\n";
        return;
    }

    EXPECT_EQ(res.size(), 9);

    for (const auto& mt : res) {
        switch (mt.term) {
        case Term::UnmatchedSell:
            EXPECT_EQ(mt.sold_timestamp, from_usa_date(8, 10, 2018));
            EXPECT_EQ(mt.currency, "ETH");
            EXPECT_NEAR(mt.sz, 19.608, 0.1);
            EXPECT_NEAR(mt.pnl, 7131.14, 0.1);
            break;
        case Term::Held:
            if (mt.bought_timestamp == from_usa_date(9, 11, 2020)) {
                EXPECT_EQ(mt.currency, "ETH");
                EXPECT_NEAR(mt.sz, 0.69, 0.1);
                EXPECT_NEAR(mt.pnl, 556.11, 0.1);
            } else if (mt.bought_timestamp == from_usa_date(6, 2, 2021)) {
                EXPECT_EQ(mt.currency, "ETH");
                EXPECT_NEAR(mt.sz, 7.81, 0.1);
                EXPECT_NEAR(mt.pnl, -11426.8, 0.1);
            } else if (mt.bought_timestamp == from_usa_date(5, 20, 2022)) {
                EXPECT_EQ(mt.currency, "ETH");
                EXPECT_NEAR(mt.sz, 14.925, 0.1);
                EXPECT_NEAR(mt.pnl, -12742, 0.1);
            } else {
                ADD_FAILURE();
            }
            break;
        case Term::Short:
            if (mt.bought_timestamp == from_usa_date(9, 11, 2020)) {
                EXPECT_EQ(mt.sold_timestamp, from_usa_date(5, 14, 2021));
                EXPECT_EQ(mt.currency, "ETH");
                EXPECT_NEAR(mt.sz, 12.82, 0.1);
                EXPECT_NEAR(mt.pnl, 43369.6, 0.1);
            } else if (mt.bought_timestamp == from_usa_date(5, 14, 2021)) {
                EXPECT_EQ(mt.sold_timestamp, from_usa_date(6, 2, 2021));
                EXPECT_EQ(mt.currency, "BTC");
                EXPECT_NEAR(mt.sz, 0.5, 0.1);
                EXPECT_NEAR(mt.pnl, -6646.36, 0.1);
            } else {
                ADD_FAILURE();
            }
            break;
        case Term::Long:
            if (mt.bought_timestamp == from_usa_date(5, 14, 2021)) {
                EXPECT_EQ(mt.sold_timestamp, from_usa_date(5, 20, 2022));
                EXPECT_EQ(mt.currency, "BTC");
                EXPECT_NEAR(mt.sz, 0.5, 0.1);
                EXPECT_NEAR(mt.pnl, -9795.19, 0.1);
            } else if (mt.sold_timestamp == from_usa_date(9, 11, 2020)) {
                EXPECT_EQ(mt.bought_timestamp, from_usa_date(8, 10, 2018));
                EXPECT_EQ(mt.currency, "BTC");
                EXPECT_NEAR(mt.sz, 0.5, 0.1);
                EXPECT_NEAR(mt.pnl, 1905.45, 0.1);
            } else if (mt.sold_timestamp == from_usa_date(5, 20, 2022)) {
                EXPECT_EQ(mt.bought_timestamp, from_usa_date(8, 10, 2018));
                EXPECT_EQ(mt.currency, "BTC");
                EXPECT_NEAR(mt.sz, 0.5, 0.1);
                EXPECT_NEAR(mt.pnl, 11925.6, 0.1);
            } else {
                ADD_FAILURE();
            }
            break;
        }
    }
}

TEST(EngineIntegration, GetPNLFrom) {
    Matcher m;
    PNL res1, res2;

    try {
        res1 = m.get_pnl_from(sample_trades[ 2 ], sample_end);
        res2 = m.get_pnl_from(sample_trades[ 3 ], sample_end);
    } catch (const RateLimitedQuery& rq) {
        std::cerr << "rate limited, redo later\n";
        return;
    }

    EXPECT_NEAR(res1, 1310.21, 0.1);
    EXPECT_NEAR(res2, 1030.4, 0.1);
}

TEST(EngineIntegration, GetNetPNL) {
    Matcher m;
    PNL res;

    try {
        res = m.get_net_pnl(sample_trades, sample_end);
    } catch (const RateLimitedQuery& rq) {
        std::cerr << "rate limited, redo later\n";
        return;
    }

    EXPECT_NEAR(res, 4594.44, 0.1);
}

TEST(EngineIntegration, GetYearEndPNL) {
    Matcher m;
    YearEndPNL res;

    try {
        res = m.get_year_end_pnl(sample_trades, sample_end);
    } catch (std::exception const& e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }

    EXPECT_NEAR(res.actual, 0.0, 0.1);
    EXPECT_NEAR(res.lt_realized, 0.0, 0.1);
    EXPECT_NEAR(res.st_realized, 0.0, 0.1);
}

TEST(EngineIntegration, GetPNLSnapshots) {
    Matcher m;
    std::vector<SnapshotPNL> res;

    try {
        res = m.get_pnl_snapshots(sample_trades, sample_end);
    } catch (const RateLimitedQuery& rq) {
        std::cerr << "rate limited, redo later\n";
        return;
    }

    SUCCEED();
}

TEST(EngineIntegration, GetEarliestLongTermSells) {
    Matcher m;
    std::vector<Trade> res;

    try {
        res = m.get_earliest_long_term_sells(sample_trades, sample_end);
    } catch (const RateLimitedQuery& rq) {
        std::cerr << "rate limited, redo later\n";
        return;
    }

    SUCCEED();
}
