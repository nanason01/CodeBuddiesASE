
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

class MatchingFixture : public ::testing::Test {
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

TEST_F(MatchingFixture, GetPnlFromUSD) {
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
TEST_F(MatchingFixture, GetPnlFromSwap) {
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
