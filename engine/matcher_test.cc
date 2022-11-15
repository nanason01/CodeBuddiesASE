// Copyright 2022 CodeBuddies ASE Group
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
CRSH = 1000 begin, 25 in 7/1/2017-1/1/2018, +100/mo in 2/1/2018-10/1/2018, +10/mo 11/1 and 12/1

*/

const Timestamp BEGIN = from_usa_date(1, 1, 2017);
const Timestamp END = from_usa_date(1, 1, 2019);

using testing::Return;

class MatcherFixture : public ::testing::Test {
protected:
    ::testing::NiceMock<MockPricer> mock_pricer;
    Matcher* matcher;
    void SetUp() override {
        matcher = new Matcher(&mock_pricer);

        auto cur_month = get_month(BEGIN);
        double val = 1000.0;
        for (Timestamp i = BEGIN; i != END; i = i + from_cal(0, 1, 0)) {
            if (cur_month != get_month(i)) {
                cur_month = get_month(i);
                val += 10.0;
            }
            ON_CALL(mock_pricer, get_usd_price("INCR", i))
                .WillByDefault(Return(val));
        }

        cur_month = get_month(BEGIN);
        val = 1000.0;
        for (Timestamp i = BEGIN; i != END; i = i + from_cal(0, 1, 0)) {
            if (cur_month != get_month(i)) {
                cur_month = get_month(i);
                val -= 10.0;
            }
            ON_CALL(mock_pricer, get_usd_price("DECR", i))
                .WillByDefault(Return(val));
        }

        auto cur_year = get_year(BEGIN);
        double delta = -10.0;
        cur_month = get_month(BEGIN);
        val = 1000.0;
        for (Timestamp i = BEGIN; i != END; i = i + from_cal(0, 1, 0)) {
            if (cur_year != get_year(i)) {
                cur_year = get_year(i);
                delta *= -1;
            }
            if (cur_month != get_month(i)) {
                cur_month = get_month(i);
                val += delta;
            }
            ON_CALL(mock_pricer, get_usd_price("VBOT", i))
                .WillByDefault(Return(val));
        }

        cur_year = get_year(BEGIN);
        delta = 10.0;
        cur_month = get_month(BEGIN);
        val = 1000.0;
        for (Timestamp i = BEGIN; i != END; i = i + from_cal(0, 1, 0)) {
            if (cur_year != get_year(i)) {
                cur_year = get_year(i);
                delta *= -1;
            }
            if (cur_month != get_month(i)) {
                cur_month = get_month(i);
                val += delta;
            }
            ON_CALL(mock_pricer, get_usd_price("VTOP", i))
                .WillByDefault(Return(val));
        }

        ON_CALL(mock_pricer, get_usd_price("FLAT", ::testing::_))
            .WillByDefault(Return(1000.0));

        auto crash_date = from_usa_date(7, 1, 2017);
        for (Timestamp i = BEGIN; i != crash_date; i = i + from_cal(0, 1, 0)) {
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillByDefault(Return(1000.0));
        }
        auto improve_date = from_usa_date(1, 1, 2018);
        for (Timestamp i = crash_date; i != improve_date; i = i + from_cal(0, 1, 0)) {
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillByDefault(Return(25.0));
        }
        auto slow_down_date = from_usa_date(11, 1, 2018);
        cur_month = get_month(improve_date);
        val = 25.0;
        for (Timestamp i = improve_date; i != slow_down_date; i = i + from_cal(0, 1, 0)) {
            if (cur_month != get_month(i)) {
                cur_month = get_month(i);
                val += 100.0;
            }
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillByDefault(Return(val));
        }
        for (Timestamp i = slow_down_date; i != END; i = i + from_cal(0, 1, 0)) {
            if (cur_month != get_month(i)) {
                cur_month = get_month(i);
                val += 10.0;
            }
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillByDefault(Return(val));
        }
    }
    void TearDown() override {
        delete matcher;
    }
};

// Timestamp tests

TEST(Timestamp, TSSubtraction) {
    const Timestamp ts1 = from_usa_date(5, 10, 2022);
    Timestamp ts2 = from_usa_date(5, 1, 2022);
    Timestamp ts3 = from_usa_date(4, 14, 2022);
    Timestamp ts4 = from_usa_date(6, 12, 2021);

    TimeDelta diff1 = ts1 - ts2;
    TimeDelta diff2 = ts1 - ts3;
    TimeDelta diff3 = ts1 - ts4;

    EXPECT_EQ(diff1, from_cal(0, 9, 0));

    EXPECT_EQ(ts1, ts2 + from_cal(0, 9, 0));
    EXPECT_EQ(ts2 + diff1, ts1);
    EXPECT_EQ(ts3 + diff2, ts1);
    EXPECT_EQ(ts4 + diff3, ts1);
}

// get_pnl_from

TEST_F(MatcherFixture, GetPnlFromUSD) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp exit_time = from_usa_date(1, 1, 2018);

    Trade buy_incr = {
        entry_time,
        "USD",
        "INCR",
        1000.0,
        1.0,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(buy_incr, exit_time), 120.0);
    Trade buy_decr = {
        entry_time,
        "USD",
        "DECR",
        2000.0,
        2.0,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(buy_decr, exit_time), -240.0);
    Trade sell_incr = {
        entry_time,
        "INCR",
        "USD",
        3.0,
        3000.0,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(sell_incr, exit_time), -360.0);
    Trade sell_decr = {
        entry_time,
        "DECR",
        "USD",
        4.0,
        4000.0,
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
        2.0,
        2.0,
    };
    EXPECT_FLOAT_EQ(matcher->get_pnl_from(decr_to_incr, exit_time), 160.0);
    Trade incr_to_decr = {
        entry_time,
        "INCR",
        "DECR",
        1.0,
        1.0,
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
        2.0,
        2.0,
    };
    Trade usd_to_incr = {
        entry_time,
        "USD",
        "INCR",
        1000.0,
        1.0,
    };
    Trade late_incr_to_usd = {
        entry_time2,
        "INCR",
        "USD",
        2.0,
        2040.0,
    };

    EXPECT_FLOAT_EQ(matcher->get_net_pnl({ decr_to_incr, usd_to_incr, late_incr_to_usd }, exit_time), 160.0);
}
TEST_F(MatcherFixture, GetNetPnlFlat) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(3, 1, 2017);
    Timestamp exit_time = from_usa_date(5, 1, 2017);

    Trade decr_to_incr = {
        entry_time,
        "DECR",
        "INCR",
        2.0,
        2.0,
    };
    Trade incr_to_decr = {
        entry_time2,
        "INCR",
        "DECR",
        4.0 * 980.0 / 1000.0,
        4.0 * 1020.0 / 1000.0,
    };

    EXPECT_NEAR(matcher->get_net_pnl({ decr_to_incr, incr_to_decr }, exit_time), 0.0, 0.00001);
}
TEST_F(MatcherFixture, GetNetPnlLoss) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(3, 1, 2017);
    Timestamp exit_time = from_usa_date(8, 1, 2017);

    Trade flat_to_crsh = {
        entry_time,
        "FLAT",
        "CRSH",
        1.0,
        1.0,
    };
    Trade decr_to_incr = {
        entry_time2,
        "DECR",
        "INCR",
        2.0 * 1020.0 / 1000.0,
        2.0 * 980.0 / 1000.0,
    };

    EXPECT_FLOAT_EQ(matcher->get_net_pnl({ flat_to_crsh, decr_to_incr }, exit_time), -775.0);
}

// get_pnl_snapshots

TEST_F(MatcherFixture, GetPnlSnapshotsEmpty) {
    const vector<TimeDelta> samples = {
        now() - from_usa_date(3,1,2017),
        now() - from_usa_date(6,1,2017),
        now() - from_usa_date(1,1,2018),
    };

    EXPECT_EQ(now() - samples[0], from_usa_date(3, 1, 2017));

    const auto ret = matcher->get_pnl_snapshots({}, samples);

    ASSERT_EQ(ret.size(), 3);

    EXPECT_EQ(ret[0].timestamp, from_usa_date(3, 1, 2017));
    EXPECT_FLOAT_EQ(ret[0].pnl, 0.0);

    EXPECT_EQ(ret[1].timestamp, from_usa_date(6, 1, 2017));
    EXPECT_FLOAT_EQ(ret[1].pnl, 0.0);

    EXPECT_EQ(ret[2].timestamp, from_usa_date(1, 1, 2018));
    EXPECT_FLOAT_EQ(ret[2].pnl, 0.0);
}
TEST_F(MatcherFixture, GetPnlSnapshotsNormal) {
    const vector<TimeDelta> samples = {
        now() - from_usa_date(3,1,2017),
        now() - from_usa_date(6,1,2017),
        now() - from_usa_date(1,1,2018)
    };

    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(4, 1, 2017);

    Trade usd_to_crsh = {
        entry_time,
        "USD",
        "CRSH",
        1000.0,
        1.0,
    };
    Trade usd_to_incr = {
        entry_time,
        "USD",
        "INCR",
        1000.0,
        1.0,
    };
    Trade late_incr_to_decr = {
        entry_time2,
        "INCR",
        "DECR",
        1.0,
        1.0,
    };

    const auto ret = matcher->get_pnl_snapshots({ usd_to_crsh, usd_to_incr, late_incr_to_decr }, samples);

    ASSERT_EQ(ret.size(), 3);

    EXPECT_EQ(ret[0].timestamp, from_usa_date(3, 1, 2017));
    EXPECT_FLOAT_EQ(ret[0].pnl, 20.0);

    EXPECT_EQ(ret[1].timestamp, from_usa_date(6, 1, 2017));
    EXPECT_FLOAT_EQ(ret[1].pnl, -50.0);

    EXPECT_EQ(ret[2].timestamp, from_usa_date(1, 1, 2018));
    EXPECT_FLOAT_EQ(ret[2].pnl, -1095.0);
}

// get_matched_trades

static std::vector<MatchedTrade> filter_term(const std::vector<MatchedTrade>& mts, Term term) {
    std::vector<MatchedTrade> ret;

    for (const auto& mt : mts) {
        if (mt.term == term)
            ret.push_back(mt);
    }

    return ret;
}

TEST_F(MatcherFixture, GetMatchedTradesEmpty) {
    const auto res = matcher->get_matched_trades({});
    EXPECT_TRUE(res.empty());
}
TEST_F(MatcherFixture, GetMatchedTradesSellThenBuy) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(4, 1, 2017);

    Trade incr_to_usd = {
        entry_time,
        "INCR",
        "USD",
        1.0,
        1000.0,
    };
    Trade usd_to_incr = {
        entry_time2,
        "USD",
        "INCR",
        2060.0,
        2.0,
    };

    const auto res = matcher->get_matched_trades({ incr_to_usd, usd_to_incr });

    EXPECT_TRUE(filter_term(res, Term::Short).empty());
    EXPECT_TRUE(filter_term(res, Term::Long).empty());
}
TEST_F(MatcherFixture, GetMatchedTradesUnmatched) {
    Timestamp entry_time = from_usa_date(1, 1, 2017);
    Timestamp entry_time2 = from_usa_date(4, 1, 2017);
    Timestamp entry_time3 = from_usa_date(1, 1, 2018);

    Trade usd_to_incr = {
        entry_time,
        "USD",
        "INCR",
        1000.0,
        1.0,
    };
    Trade usd_to_incr2 = {
        entry_time2,
        "USD",
        "INCR",
        2060.0,
        2.0,
    };
    Trade usd_to_incr3 = {
        entry_time3,
        "USD",
        "INCR",
        3360.0,
        3.0,
    };

    double sum_held = 0.0, sum_unmatched_sell = 0.0;
    for (const auto& matched : matcher->get_matched_trades({ usd_to_incr, usd_to_incr2, usd_to_incr3 })) {
        switch (matched.term) {
        case (Term::Held):
            sum_held += matched.sz;
            EXPECT_EQ(matched.currency, "INCR");
            EXPECT_THAT(matched.bought_timestamp, ::testing::AnyOf(entry_time, entry_time2, entry_time3));
            break;
        case (Term::UnmatchedSell):
            sum_unmatched_sell += matched.sz;
            EXPECT_EQ(matched.currency, "USD");
            EXPECT_THAT(matched.sold_timestamp, ::testing::AnyOf(entry_time, entry_time2, entry_time3));
            break;
        default:
            ADD_FAILURE();
            break;
        }
    }

    EXPECT_FLOAT_EQ(sum_held, 6.0);
    EXPECT_FLOAT_EQ(sum_unmatched_sell, 6420.0);
}
TEST_F(MatcherFixture, GetMatchedTradesLossOverGain) {
    Timestamp vtop80up = from_usa_date(9, 1, 2017);
    Timestamp vtop100up = from_usa_date(11, 1, 2017);
    Timestamp vtop90down = from_usa_date(2, 1, 2018);

    Timestamp vtop20up = from_usa_date(3, 1, 2017);
    Timestamp vtop40up = from_usa_date(5, 1, 2017);
    Timestamp vtop30down = from_usa_date(8, 1, 2018);

    Trade vtopbuy80up{
        vtop80up,
        "USD",
        "VTOP",
        1080.0,
        1.0,
    };
    Trade vtopbuy100up{
        vtop100up,
        "USD",
        "VTOP",
        1100.0,
        1.0,
    };
    Trade vtopsell90down{
        vtop90down,
        "VTOP",
        "USD",
        1.0,
        1090.0,
    };

    Trade vtopbuy20up{
        vtop20up,
        "USD",
        "VTOP",
        1080.0,
        1.0,
    };
    Trade vtopbuy40up{
        vtop40up,
        "USD",
        "VTOP",
        1100.0,
        1.0,
    };
    Trade vtopsell30down{
        vtop30down,
        "VTOP",
        "USD",
        1.0,
        1090.0,
    };

    const auto st_res = matcher->get_matched_trades({ vtopbuy80up, vtopbuy100up, vtopsell90down });
    EXPECT_TRUE(filter_term(st_res, Term::Long).empty());
    ASSERT_FALSE(filter_term(st_res, Term::Short).empty());
    MatchedTrade stmt = filter_term(st_res, Term::Short).front();
    EXPECT_EQ(stmt.bought_timestamp, vtop100up);
    EXPECT_EQ(stmt.sold_timestamp, vtop90down);
    EXPECT_EQ(stmt.currency, "VTOP");
    EXPECT_FLOAT_EQ(stmt.sz, 1.0);
    EXPECT_FLOAT_EQ(stmt.pnl, -10.0);

    const auto lt_res = matcher->get_matched_trades({ vtopbuy20up, vtopbuy40up, vtopsell30down });
    EXPECT_TRUE(filter_term(lt_res, Term::Short).empty());
    ASSERT_FALSE(filter_term(lt_res, Term::Long).empty());
    MatchedTrade ltmt = filter_term(lt_res, Term::Long).front();
    EXPECT_EQ(ltmt.bought_timestamp, vtop40up);
    EXPECT_EQ(ltmt.sold_timestamp, vtop30down);
    EXPECT_EQ(ltmt.currency, "VTOP");
    EXPECT_FLOAT_EQ(ltmt.sz, 1.0);
    EXPECT_FLOAT_EQ(ltmt.pnl, -10.0);
}
TEST_F(MatcherFixture, GetMatchedTradesMatchMultiple) {
    Timestamp vtop40up = from_usa_date(5, 1, 2017);
    Timestamp vtop40down = from_usa_date(9, 1, 2018);
    Timestamp vtop30down = from_usa_date(10, 1, 2018);

    Trade vtopbuy40up{
        vtop40up,
        "USD",
        "VTOP",
        1040.0,
        1.0,
    };
    Trade vtopbuy40down{
        vtop40down,
        "USD",
        "VTOP",
        1040.0,
        1.0,
    };
    Trade vtopsell30down{
        vtop30down,
        "VTOP",
        "USD",
        2.0,
        2060.0,
    };

    const auto res = matcher->get_matched_trades({ vtopbuy40up, vtopbuy40down, vtopsell30down });

    ASSERT_EQ(filter_term(res, Term::Short).size(), 1);
    ASSERT_EQ(filter_term(res, Term::Long).size(), 1);

    EXPECT_FLOAT_EQ(filter_term(res, Term::Short)[0].sz, 1.0);
    EXPECT_FLOAT_EQ(filter_term(res, Term::Short)[0].pnl, -10.0);
    EXPECT_FLOAT_EQ(filter_term(res, Term::Long)[0].sz, 1.0);
    EXPECT_FLOAT_EQ(filter_term(res, Term::Long)[0].pnl, -10.0);
}
TEST_F(MatcherFixture, GetMatchedTradesSTCLOverLTCL) {
    Timestamp vtop40up = from_usa_date(5, 1, 2017);
    Timestamp vtop40down = from_usa_date(9, 1, 2018);
    Timestamp vtop30down = from_usa_date(10, 1, 2018);

    Trade vtopbuy40up{
        vtop40up,
        "USD",
        "VTOP",
        1040.0,
        1.0,
    };
    Trade vtopbuy40down{
        vtop40down,
        "USD",
        "VTOP",
        1040.0,
        1.0,
    };
    Trade vtopsell30down{
        vtop30down,
        "VTOP",
        "USD",
        1.0,
        1030.0,
    };

    const auto res = matcher->get_matched_trades({ vtopbuy40up, vtopbuy40down, vtopsell30down });

    ASSERT_EQ(filter_term(res, Term::Short).size(), 1);
    EXPECT_EQ(filter_term(res, Term::Long).size(), 0);

    EXPECT_EQ(filter_term(res, Term::Short)[0].bought_timestamp, vtop40down);
    EXPECT_FLOAT_EQ(filter_term(res, Term::Short)[0].sz, 1.0);
    EXPECT_FLOAT_EQ(filter_term(res, Term::Short)[0].pnl, -10.0);
}
TEST_F(MatcherFixture, GetMatchedTradesLTCGOverSTCG) {
    Timestamp vbot40up = from_usa_date(5, 1, 2017);
    Timestamp vbot40down = from_usa_date(9, 1, 2018);
    Timestamp vbot30down = from_usa_date(10, 1, 2018);

    Trade vbotbuy40up{
        vbot40up,
        "USD",
        "VBOT",
        960.0,
        1.0,
    };
    Trade vbotbuy40down{
        vbot40down,
        "USD",
        "VBOT",
        960.0,
        1.0,
    };
    Trade vbotsell30down{
        vbot30down,
        "VBOT",
        "USD",
        1.0,
        970.0,
    };

    const auto res = matcher->get_matched_trades({ vbotbuy40up, vbotbuy40down, vbotsell30down });

    ASSERT_EQ(filter_term(res, Term::Long).size(), 1);
    EXPECT_EQ(filter_term(res, Term::Short).size(), 0);

    EXPECT_EQ(filter_term(res, Term::Long)[0].bought_timestamp, vbot40up);
    EXPECT_FLOAT_EQ(filter_term(res, Term::Long)[0].sz, 1.0);
    EXPECT_FLOAT_EQ(filter_term(res, Term::Long)[0].pnl, 10.0);
}
TEST_F(MatcherFixture, GetMatchedTradesSTCGOverLTCG) {
    Timestamp crsh25 = from_usa_date(7, 1, 2017);
    Timestamp crsh935 = from_usa_date(11, 1, 2018);
    Timestamp crsh945 = from_usa_date(12, 1, 2018);

    Trade crsh25buy{
        crsh25,
        "USD",
        "CRSH",
        25.0,
        1.0,
    };
    Trade crsh935buy{
        crsh935,
        "USD",
        "CRSH",
        935.0,
        1.0,
    };
    Trade crsh945sell{
        crsh945,
        "CRSH",
        "USD",
        1.0,
        945.0,
    };

    const auto res = matcher->get_matched_trades({ crsh25buy, crsh935buy, crsh945sell });

    ASSERT_EQ(filter_term(res, Term::Short).size(), 1);
    EXPECT_EQ(filter_term(res, Term::Long).size(), 0);

    EXPECT_EQ(filter_term(res, Term::Short)[0].bought_timestamp, crsh935);
    EXPECT_FLOAT_EQ(filter_term(res, Term::Short)[0].sz, 1.0);
    EXPECT_FLOAT_EQ(filter_term(res, Term::Short)[0].pnl, 10.0);
}
TEST_F(MatcherFixture, GetMatchedTradesFlatOverGain) {
    Timestamp vbot30up = from_usa_date(4, 1, 2017);
    Timestamp vbot40down = from_usa_date(9, 1, 2018);
    Timestamp vbot30down = from_usa_date(10, 1, 2018);

    Trade vbotbuy30up{
        vbot30up,
        "USD",
        "VBOT",
        970.0,
        1.0,
    };
    Trade vbotbuy40down{
        vbot40down,
        "USD",
        "VBOT",
        960.0,
        1.0,
    };
    Trade vbotsell30down{
        vbot30down,
        "VBOT",
        "USD",
        1.0,
        970.0,
    };

    const auto res1 = matcher->get_matched_trades({ vbotbuy30up, vbotbuy40down, vbotsell30down });

    ASSERT_EQ(filter_term(res1, Term::Long).size(), 1);
    EXPECT_EQ(filter_term(res1, Term::Short).size(), 0);

    EXPECT_EQ(filter_term(res1, Term::Long)[0].bought_timestamp, vbot30up);
    EXPECT_FLOAT_EQ(filter_term(res1, Term::Long)[0].sz, 1.0);
    EXPECT_FLOAT_EQ(filter_term(res1, Term::Long)[0].pnl, 0.0);

    Timestamp vbot70up = from_usa_date(8, 1, 2017);
    Timestamp vbot80up = from_usa_date(9, 1, 2018);
    Timestamp vbot70down = from_usa_date(4, 1, 2018);

    Trade vbotbuy70up{
        vbot70up,
        "USD",
        "VBOT",
        930.0,
        1.0,
    };
    Trade vbotbuy80up{
        vbot80up,
        "USD",
        "VBOT",
        920.0,
        1.0,
    };
    Trade vbotsell70up{
        vbot70down,
        "VBOT",
        "USD",
        1.0,
        930.0,
    };

    const auto res2 = matcher->get_matched_trades({ vbotbuy70up, vbotbuy80up, vbotsell70up });

    ASSERT_EQ(filter_term(res2, Term::Short).size(), 1);
    EXPECT_EQ(filter_term(res2, Term::Long).size(), 0);

    EXPECT_EQ(filter_term(res2, Term::Short)[0].bought_timestamp, vbot70up);
    EXPECT_FLOAT_EQ(filter_term(res2, Term::Short)[0].sz, 1.0);
    EXPECT_FLOAT_EQ(filter_term(res2, Term::Short)[0].pnl, 0.0);
}
TEST_F(MatcherFixture, GetMatchedTradesCalcEachYear) {
    Timestamp crsh2017buy = from_usa_date(3, 1, 2017);
    Timestamp crsh2017sell = from_usa_date(6, 1, 2017);
    Timestamp crsh2018buy = from_usa_date(1, 1, 2018);
    Timestamp crsh2018sell = from_usa_date(1, 27, 2018);

    Trade earlybuy{
        crsh2017buy,
        "USD",
        "CRSH",
        1000.0,
        1.0,
    };
    Trade earlysell{
        crsh2017sell,
        "CRSH",
        "USD",
        1.0,
        1000.0,
    };
    Trade latebuy{
        crsh2018buy,
        "USD",
        "CRSH",
        25.0,
        1.0,
    };
    Trade latesell{
        crsh2018sell,
        "CRSH",
        "USD",
        1.0,
        25.0,
    };

    auto res = matcher->get_matched_trades({ earlybuy, earlysell, latebuy, latesell });
    auto mts = filter_term(res, Term::Short);

    ASSERT_EQ(mts.size(), 2);

    // technically allowed to return in any order
    if (mts[0].bought_timestamp == crsh2018buy)
        std::swap(mts[0], mts[1]);

    EXPECT_EQ(mts[0].bought_timestamp, crsh2017buy);
    EXPECT_EQ(mts[0].sold_timestamp, crsh2017sell);
    EXPECT_EQ(mts[0].currency, "CRSH");
    EXPECT_FLOAT_EQ(mts[0].pnl, 0.0);
    EXPECT_FLOAT_EQ(mts[0].sz, 1.0);

    EXPECT_EQ(mts[1].bought_timestamp, crsh2018buy);
    EXPECT_EQ(mts[1].sold_timestamp, crsh2018sell);
    EXPECT_EQ(mts[1].currency, "CRSH");
    EXPECT_FLOAT_EQ(mts[1].pnl, 0.0);
    EXPECT_FLOAT_EQ(mts[1].sz, 1.0);
}
TEST_F(MatcherFixture, GetMatchedTradesNormal) {
    Timestamp ts3_1_2017 = from_usa_date(3, 1, 2017);
    Timestamp ts6_1_2017 = from_usa_date(6, 1, 2017);
    Timestamp ts1_1_2018 = from_usa_date(1, 1, 2018);
    Timestamp ts7_1_2018 = from_usa_date(7, 1, 2018);

    std::vector<Trade> input = {
        {
            ts3_1_2017,
            "USD",
            "INCR",
            3060.0,
            3.0,
        },
        {
            ts3_1_2017,
            "USD",
            "DECR",
            1960.0,
            2.0,
        },
        {
            ts6_1_2017,
            "DECR",
            "USD",
            1.0,
            950.0,
        },
        {
            ts6_1_2017,
            "INCR",
            "VBOT",
            950.0 / 1050.0,
            1.0,
        },
        {
            ts6_1_2017,
            "INCR",
            "USD",
            100.0 / 1050.0,
            100.0,
        },
        {
            ts1_1_2018,
            "USD",
            "DECR",
            880.0,
            1.0,
        },
        {
            ts1_1_2018,
            "INCR",
            "USD",
            1.0,
            1120.0,
        },
        {
            ts7_1_2018,
            "DECR",
            "USD",
            1.0,
            820.0,
        },
        {
            ts7_1_2018,
            "INCR",
            "USD",
            1.0,
            1180.0,
        },
        {
            ts7_1_2018,
            "VBOT",
            "USD",
            1.0,
            960.0,
        },
    };

    const auto res = matcher->get_matched_trades(input);

    const auto st = filter_term(res, Term::Short);
    const auto lt = filter_term(res, Term::Long);
    const auto ums = filter_term(res, Term::UnmatchedSell);
    const auto hld = filter_term(res, Term::Held);

    EXPECT_THAT(st, ::testing::UnorderedElementsAre(
        MatchedTrade{
            ts3_1_2017,
            ts6_1_2017,
            Term::Short,
            "DECR",
            1.0,
            -30.0
        },
        MatchedTrade{
            ts3_1_2017,
            ts6_1_2017,
            Term::Short,
            "INCR",
            1.0,
            30.0
        },
        MatchedTrade{
            ts3_1_2017,
            ts1_1_2018,
            Term::Short,
            "INCR",
            1.0,
            100.0
        },
        MatchedTrade{
            ts1_1_2018,
            ts7_1_2018,
            Term::Short,
            "DECR",
            1.0,
            -60.0
        }
    ));
    EXPECT_THAT(lt, ::testing::UnorderedElementsAre(
        MatchedTrade{
            ts6_1_2017,
            ts7_1_2018,
            Term::Long,
            "VBOT",
            1.0,
            10.0
        },
        MatchedTrade{
            ts3_1_2017,
            ts7_1_2018,
            Term::Long,
            "INCR",
            1.0,
            160.0
        }
    ));
    EXPECT_THAT(hld, ::testing::Contains(
        ::testing::AllOf(
            ::testing::Field(&MatchedTrade::bought_timestamp, ts3_1_2017),
            ::testing::Field(&MatchedTrade::currency, "DECR"),
            ::testing::Field(&MatchedTrade::sz, 1.0)
        )
    ));

    double usd_hld = 0.0;
    for (const auto& mt : hld) {
        if (mt.currency == "USD")
            usd_hld += mt.sz;
    }
    EXPECT_FLOAT_EQ(usd_hld, 4250.0);

    double usd_had = 0.0;
    for (const auto& mt : ums) {
        if (mt.currency == "USD")
            usd_had += mt.sz;
    }
    EXPECT_FLOAT_EQ(usd_had, 5020.0);

    EXPECT_THAT(ums, ::testing::Each(
        ::testing::Field(&MatchedTrade::currency, "USD")
    ));
}

// get_year_end_pnl
TEST_F(MatcherFixture, GetYearEndPnl) {
    Timestamp ts3_1_2017 = from_usa_date(3, 1, 2017);
    Timestamp ts6_1_2017 = from_usa_date(6, 1, 2017);
    Timestamp ts1_1_2018 = from_usa_date(1, 1, 2018);
    Timestamp ts7_1_2018 = from_usa_date(7, 1, 2018);

    std::vector<Trade> input = {
        {
            ts3_1_2017,
            "USD",
            "INCR",
            3060.0,
            3.0,
        },
        {
            ts3_1_2017,
            "USD",
            "DECR",
            1960.0,
            2.0,
        },
        {
            ts6_1_2017,
            "DECR",
            "USD",
            1.0,
            950.0,
        },
        {
            ts6_1_2017,
            "INCR",
            "VBOT",
            950.0 / 1050.0,
            1.0,
        },
        {
            ts6_1_2017,
            "INCR",
            "USD",
            100.0 / 1050.0,
            100.0,
        },
        {
            ts1_1_2018,
            "USD",
            "DECR",
            900.0,
            1.0,
        },
        {
            ts1_1_2018,
            "INCR",
            "USD",
            1.0,
            1120.0,
        },
        {
            ts7_1_2018,
            "DECR",
            "USD",
            1.0,
            820.0,
        },
        {
            ts7_1_2018,
            "INCR",
            "USD",
            1.0,
            1180.0,
        },
        {
            ts7_1_2018,
            "VBOT",
            "USD",
            1.0,
            960.0,
        },
    };

    const auto res2017 = matcher->get_year_end_pnl(input, from_usa_date(5, 5, 2017));

    EXPECT_EQ(res2017.actual, 0.0);
    EXPECT_EQ(res2017.lt_realized, 0.0);
    EXPECT_EQ(res2017.st_realized, 0.0);

    const auto res2018 = matcher->get_year_end_pnl(input, from_usa_date(5, 5, 2018));

    EXPECT_EQ(res2018.actual, 190.0);
    EXPECT_EQ(res2018.lt_realized, 170.0);
    EXPECT_EQ(res2018.st_realized, 20.0);
}


// get_earliest_long_term_sells

TEST_F(MatcherFixture, GetEarliestLongTermSells) {
    Timestamp ts3_1_2017 = from_usa_date(3, 1, 2017);
    Timestamp ts6_1_2017 = from_usa_date(6, 1, 2017);
    Timestamp ts3_1_2018 = from_usa_date(3, 1, 2018);
    Timestamp ts3_2_2018 = from_usa_date(3, 2, 2018);
    Timestamp ts3_3_2018 = from_usa_date(3, 3, 2018);
    Timestamp ts6_2_2018 = from_usa_date(6, 2, 2018);

    const std::vector<Trade> input = {
        {
            ts3_1_2017,
            "USD",
            "DECR",
            980.0,
            1.0,
        },
        {
            ts3_1_2017,
            "USD",
            "INCR",
            1020.0,
            1.0,
        },
        {
            ts6_1_2017,
            "USD",
            "DECR",
            950.0,
            1.0,
        },
        {
            ts6_1_2017,
            "USD",
            "INCR",
            1050.0,
            1.0,
        }
    };

    // this is to check off-by-one errors
    const auto res1 = matcher->get_earliest_long_term_sells(input, ts3_1_2018);

    EXPECT_THAT(res1, ::testing::Each(
        ::testing::AnyOf(
            ::testing::Field(&Trade::timestamp, ts3_2_2018),
            ::testing::Field(&Trade::timestamp, ts6_2_2018)
        )
    ));

    const auto res2 = matcher->get_earliest_long_term_sells(input, ts3_2_2018);

    EXPECT_THAT(res2, ::testing::Each(
        ::testing::AnyOf(
            ::testing::Field(&Trade::timestamp, ts3_2_2018),
            ::testing::Field(&Trade::timestamp, ts6_2_2018)
        )
    ));

    const auto res3 = matcher->get_earliest_long_term_sells(input, ts3_3_2018);

    EXPECT_THAT(res3, ::testing::Each(
        ::testing::AnyOf(
            ::testing::Field(&Trade::timestamp, ts3_2_2018),
            ::testing::Field(&Trade::timestamp, ts6_2_2018)
        )
    ));
}
