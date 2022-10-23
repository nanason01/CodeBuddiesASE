//
// Testing suite for matcher.h
//

#include "matcher.h"
#include "pricer/mock_pricer.h"
#include <gtest/gtest.h>

/*
mocked pricer will return between 1/1/2017 and 1/1/2019

Currencies are mocked as follows:

INCR = 1000 @ begin, +$10 every month
DECR = 1000 @ begin, -$10 every month
VBOT = 1000 @ begin, -$10/mo in 2017, +$10/mo in 2018
VTOP = 1000 @ begin, +$10/mo in 2017, -$10/mo in 2018
FLAT = 1000 all times
CRSH = 1000 begin, 25 in 7/1/2017-1/1/2018, +100/mo in 2018

*/

constexpr Timestamp BEGIN = from_usa_date(1, 1, 2017);
constexpr Timestamp END = from_usa_date(1, 1, 2019);

using testing::Return;

class MatcherFixture : public ::testing::Test {
protected:
    MockPricer mock_pricer;
    Matcher* matcher;
    void SetUp() override {
        matcher = new Matcher(&mock_pricer);

        auto cur_month = BEGIN.month();
        double val = 1000.0;
        for (Timestamp i = BEGIN; i != END; i = normalize(i + TimeDelta{ 1 })) {
            if (cur_month != i.month()) {
                cur_month = i.month();
                val += 10.0;
            }
            ON_CALL(mock_pricer, get_usd_price("INCR", i))
                .WillByDefault(Return(val));
        }

        cur_month = BEGIN.month();
        val = 1000.0;
        for (Timestamp i = BEGIN; i != END; i = normalize(i + TimeDelta{ 1 })) {
            if (cur_month != i.month()) {
                cur_month = i.month();
                val -= 10.0;
            }
            ON_CALL(mock_pricer, get_usd_price("DECR", i))
                .WillByDefault(Return(val));
        }

        auto cur_year = BEGIN.year();
        double delta = -10.0;
        cur_month = BEGIN.month();
        val = 1000.0;
        for (Timestamp i = BEGIN; i != END; i = normalize(i + TimeDelta{ 1 })) {
            if (cur_year != i.year()) {
                cur_year = i.year();
                delta *= -1;
            }
            if (cur_month != i.month()) {
                cur_month = i.month();
                val += delta;
            }
            ON_CALL(mock_pricer, get_usd_price("VBOT", i))
                .WillByDefault(Return(val));
        }

        cur_year = BEGIN.year();
        delta = 10.0;
        cur_month = BEGIN.month();
        val = 1000.0;
        for (Timestamp i = BEGIN; i != END; i = normalize(i + TimeDelta{ 1 })) {
            if (cur_year != i.year()) {
                cur_year = i.year();
                delta *= -1;
            }
            if (cur_month != i.month()) {
                cur_month = i.month();
                val += delta;
            }
            ON_CALL(mock_pricer, get_usd_price("VTOP", i))
                .WillByDefault(Return(val));
        }

        ON_CALL(mock_pricer, get_usd_price("FLAT", ::testing::_))
            .WillByDefault(Return(1000.0));

        auto crash_date = from_usa_date(7, 1, 2017);
        for (Timestamp i = BEGIN; i != crash_date; i = normalize(i + TimeDelta{ 1 })) {
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillByDefault(Return(1000.0));
        }
        auto improve_date = from_usa_date(1, 1, 2018);
        for (Timestamp i = crash_date; i != improve_date; i = normalize(i + TimeDelta{ 1 })) {
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillByDefault(Return(25.0));
        }
        cur_month = improve_date.month();
        val = 25.0;
        for (Timestamp i = improve_date; i != END; i = normalize(i + TimeDelta{ 1 })) {
            if (cur_month != i.month()) {
                cur_month = i.month();
                val += 100.0;
            }
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillByDefault(Return(val));
        }
    }
    void TearDown() override {
        delete matcher;
    }
};

// get_pnl_from

TEST_F(MatcherFixture, GetPnlFromUSD) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp exit_time = from_usa_date(1, 1, 2018);

    Trade buy_incr = {
        entry_time,
        "USD",
        "INCR",
        1000,
        1,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(buy_incr, exit_time), 120.0);
    Trade buy_decr = {
        entry_time,
        "USD",
        "DECR",
        2000,
        2,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(buy_decr, exit_time), -240.0);
    Trade sell_incr = {
        entry_time,
        "INCR",
        "USD",
        3,
        3000,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(sell_incr, exit_time), -360.0);
    Trade sell_decr = {
        entry_time,
        "DECR",
        "USD",
        4,
        4000,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(sell_decr, exit_time), 480.0);
}
TEST_F(MatcherFixture, GetPnlFromSwap) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp exit_time = from_usa_date(5, 1, 2017);

    Trade decr_to_incr = {
        entry_time,
        "DECR",
        "INCR",
        2,
        2,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(decr_to_incr, exit_time), 160.0);
    Trade incr_to_decr = {
        entry_time,
        "INCR",
        "DECR",
        1,
        1,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(incr_to_decr, exit_time), -80.0);
}

// get_net_pnl

TEST_F(MatcherFixture, GetNetPnlEmpty) {
    Timestamp exit_time = from_usa_date(8, 1, 2017);
    EXPECT_FLOAT_EQ(matcher->get_net_pnl({}, exit_time), 0.0);
}
TEST_F(MatcherFixture, GetNetPnlNormal) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(3, 1, 2017);
    Timestamp exit_time = from_usa_date(5, 1, 2017);

    Trade decr_to_incr = {
        entry_time,
        "DECR",
        "INCR",
        2,
        2,
    };
    Trade usd_to_incr = {
        entry_time,
        "USD",
        "INCR",
        1000,
        1,
    };
    Trade late_incr_to_usd = {
        entry_time2,
        "INCR",
        "USD",
        2,
        2040,
    };

    EXPECT_FLOAT_EQ(matcher->get_net_pnl({ decr_to_incr, usd_to_incr, late_incr_to_usd }, exit_time), 60.0);
}
TEST_F(MatcherFixture, GetNetPnlFlat) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(3, 1, 2017);
    Timestamp exit_time = from_usa_date(5, 1, 2017);

    Trade decr_to_incr = {
        entry_time,
        "DECR",
        "INCR",
        2,
        2,
    };
    Trade incr_to_decr = {
        entry_time2,
        "INCR",
        "DECR",
        4,
        4,
    };

    EXPECT_FLOAT_EQ(matcher->get_net_pnl({ decr_to_incr, incr_to_decr }, exit_time), 0.0);
}
TEST_F(MatcherFixture, GetNetPnlLoss) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(3, 1, 2017);
    Timestamp exit_time = from_usa_date(8, 1, 2017);

    Trade flat_to_crsh = {
        entry_time,
        "FLAT",
        "CRSH",
        1, // +0
        1, // -975
    };
    Trade decr_to_incr = {
        entry_time2,
        "DECR",
        "INCR",
        2, // +100
        2, // +100
    };

    EXPECT_FLOAT_EQ(matcher->get_net_pnl({ decr_to_incr, incr_to_decr }, exit_time), -775.0);
}

// get_pnl_snapshots

TEST_F(MatcherFixture, GetPnlSnapshotsEmpty) {
    const vector<Timestamp> samples = {
        from_usa_date(3,1,2017),
        from_usa_date(6,1,2017),
        from_usa_date(1,1,2018),
    };

    const auto ret = matcher->get_pnl_snapshots({}, samples);

    ASSERT_EQ(ret.size(), 3);

    for (int i = 0; i < samples.size(); i++) {
        EXPECT_EQ(ret[i].timestamp, samples[i]);
        EXPECT_FLOAT_EQ(ret[i].pnl, 0.0);
    }
}
TEST_F(MatcherFixture, GetPnlSnapshotsNormal) {
    const vector<Timestamp> samples = {
        from_usa_date(3,1,2017),
        from_usa_date(6,1,2017),
        from_usa_date(1,1,2018),
    };

    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(4, 1, 2017);
    Timestamp exit_time = from_usa_date(8, 1, 2017);

    Trade usd_to_crsh = {
        entry_time,
        "USD",
        "CRSH",
        1000,
        1,
    };
    Trade usd_to_incr = {
        entry_time,
        "USD",
        "INCR",
        1000,
        1,
    };
    Trade late_incr_to_decr = {
        entry_time2,
        "INCR",
        "DECR",
        1,
        1,
    };

    const auto ret = matcher->get_pnl_snapshots({ usd_to_crsh, usd_to_incr, late_incr_to_decr }, samples);

    ASSERT_EQ(ret.size(), 3);

    EXPECT_EQ(ret[0].timestamp, samples[0]);
    EXPECT_EQ(ret[1].timestamp, samples[1]);
    EXPECT_EQ(ret[2].timestamp, samples[2]);
    EXPECT_FLOAT_EQ(ret[0].pnl, 20.0);
    EXPECT_FLOAT_EQ(ret[1].pnl, 10.0);
    EXPECT_FLOAT_EQ(ret[2].pnl, -1035.0);
}

// get_matched_trades

static inline bool isSameMatchedMeta(const MatchedTrade& a, const MatchedTrade& b) {
    if (a.term != b.term || a.currency != b.currency)
        return false;

    switch (a.term) {
    case (Term::Held):
        return a.bought_timestamp == b.bought_timestamp;
    case (Term::UnmatchedSell):
        return a.sold_timestamp == b.sold_timestamp;
    default:
        return a.sold_timestamp == b.sold_timestamp && a.bought_timestamp == b.bought_timestamp;
    }
}

static std::vector<MatchedTrade> condense(const std::vector<MatchedTrade>& mts) {
    std::vector<MatchedTrade> ret;

    // simple O(n2) algo
    for (int i = 0; i < mts.size(); i++) {
        if (mts[i].sz == 0)
            continue;
        // look for duplicates
        for (int j = i + 1; j < mts.size(); j++) {
            if (isSameMatchedMeta(mts[i], mts[j])) {
                mts[i].sz += mts[j].sz;
                mts[i].pnl += mts[j].pnl;
                mts[j].sz = 0;
            }
        }

        ret.push_back(mts[i]);
    }

    return ret;
}

TEST_F(MatcherFixture, GetMatchedTradesEmpty) {
    const auto res = matcher->get_matched_trades({});
    EXPECT_TRUE(res.empty());
}
TEST_F(MatcherFixture, GetMatchedTradesUnmatched) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(4, 1, 2017);
    Timestamp entry_time3 = from_usa_date(1, 1, 2018);

    Trade usd_to_incr = {
        entry_time,
        "USD",
        "INCR",
        1000,
        1,
    };
    Trade usd_to_incr2 = {
        entry_time2,
        "USD",
        "INCR",
        2060,
        2,
    };
    Trade usd_to_incr3 = {
        entry_time3,
        "USD",
        "INCR",
        3360,
        3,
    };

    double sum_held = 0.0, sum_unmatched_sell = 0.0;
    for (const auto& matched : matcher->get_matched_trades({ usd_to_incr, usd_to_incr2, usd_to_incr3 })) {
        switch (matched.term) {
        case (Term::Held):
            sum_held += matched.sz;
            EXPECT_EQ(matched.currency, "INCR");
            EXPECT_EQ(matched.bought_timestamp, ::testing::AnyOf(entry_time, entry_time2, entry_time3));
            break;
        case (Term::UnmatchedSell):
            sum_unmatched_sell += matched.sz;
            EXPECT_EQ(matched.currency, "USD");
            EXPECT_EQ(matched.sold_timestamp, ::testing::AnyOf(entry_time, entry_time2, entry_time3));
            break;
        default:
            ADD_FAILURE();
            break;
        }
    }

    EXPECT_FLOAT_EQ(sum_held, 6.0);
    EXPECT_FLOAT_EQ(sum_unmatched_sell, 6420.0);
}
TEST_F(MatcherFixture, GetMatchedTradesLossOverGain) {}
TEST_F(MatcherFixture, GetMatchedTradesSTCLOverLTCL) {}
TEST_F(MatcherFixture, GetMatchedTradesLTCGOverSTCG) {}
TEST_F(MatcherFixture, GetMatchedTradesSTCGOverLTCG) {}
TEST_F(MatcherFixture, GetMatchedTradesCalcEachYear) {}
TEST_F(MatcherFixture, GetMatchedTradesNormal) {}
