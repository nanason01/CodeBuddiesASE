
#include "matching.h"
#include "pricer/mock-pricer.h"
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

using std::chrono::days;
using std::chrono::months;

constexpr Timestamp begin = days{ 1 } / 1 / 2017;
constexpr Timestamp end = days{ 1 } / 1 / 2019;

class MatchingFixture : public ::testing::Test {
protected:
    void SetUp() override {
        MockPricer mock_pricer;
        Matcher matcher(&mock_pricer);
        double val;

        auto cur_month = begin.month();
        val = 1000.0;
        for (Timestamp i = begin; i != end; i = normalize(i + days{ 1 })) {
            if (cur_month != i.month()) {
                cur_month = i.month();
                val += 10.0;
            }
            ON_CALL(mock_pricer, get_usd_price("INCR", i))
                .WillRepeatedly(Return(val));
        }

        cur_month = begin.month();
        val = 1000.0;
        for (Timestamp i = begin; i != end; i = normalize(i + days{ 1 })) {
            if (cur_month != i.month()) {
                cur_month = i.month();
                val -= 10.0;
            }
            ON_CALL(mock_pricer, get_usd_price("DECR", i))
                .WillRepeatedly(Return(val));
        }

        auto cur_year = begin.year();
        double delta = -10.0;
        cur_month = begin.month();
        val = 1000.0;
        for (Timestamp i = begin; i != end; i = normalize(i + days{ 1 })) {
            if (cur_year != i.year()) {
                cur_year = i.year();
                delta *= -1;
            }
            if (cur_month != i.month()) {
                cur_month = i.month();
                val += delta;
            }
            ON_CALL(mock_pricer, get_usd_price("VBOT", i))
                .WillRepeatedly(Return(val));
        }

        cur_year = begin.year();
        delta = 10.0;
        cur_month = begin.month();
        val = 1000.0;
        for (Timestamp i = begin; i != end; i = normalize(i + days{ 1 })) {
            if (cur_year != i.year()) {
                cur_year = i.year();
                delta *= -1;
            }
            if (cur_month != i.month()) {
                cur_month = i.month();
                val += delta;
            }
            ON_CALL(mock_pricer, get_usd_price("VTOP", i))
                .WillRepeatedly(Return(val));
        }

        ON_CALL(mock_pricer, get_usd_price("FLAT", ::testing::_))
            .WillRepeatedly(Return(1000.0));

        auto crash_date = days{ 1 } / 7 / 2017;
        for (Timestamp i = begin; i != crash_date; i = normalize(i + days{ 1 })) {
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillRepeatedly(Return(1000.0));
        }
        auto improve_date = days{ 1 } / 1 / 2018;
        for (Timestamp i = crash_date; i != improve_date; i = normalize(i + days{ 1 })) {
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillRepeatedly(Return(25.0));
        }
        cur_month = improve_date.month();
        val = 25.0;
        for (Timestamp i = improve_date; i != end; i = normalize(i + days{ 1 })) {
            if (cur_month != i.month()) {
                cur_month = i.month();
                val += 100.0;
            }
            ON_CALL(mock_pricer, get_usd_price("CRSH", i))
                .WillRepeatedly(Return(val));
        }
    }
};

// get_pnl_from

TEST_F(MatchingFixture, GetPnlFromUSD) {
    Timestamp entry_time = months{ 1 } / 1 / 2017;
    Timestamp exit_time = months{ 1 } / 1 / 2018;

    Trade buy_incr = {
        entry_time,
        "USD",
        "INCR",
        1000,
        1,
    };
    EXPECT_NEAR(matcher.get_pnl_from(buy_incr, exit_time), 120.0);
    Trade buy_decr = {
        entry_time,
        "USD",
        "DECR",
        2000,
        2,
    };
    EXPECT_NEAR(matcher.get_pnl_from(buy_decr, exit_time), -240.0);
    Trade sell_incr = {
        entry_time,
        "DECR",
        "USD",
        3,
        3000,
    };
    EXPECT_NEAR(matcher.get_pnl_from(sell_incr, exit_time), -360.0);
    Trade sell_decr = {
        entry_time,
        "USD",
        "DECR",
        4000,
        4,
    };
    EXPECT_NEAR(matcher.get_pnl_from(sell_decr, exit_time), 480.0);
}
TEST_F(MatchingFixture, GetPnlFromSwap) {
    Timestamp entry_time = months{ 1 } / 1 / 2021;
    Timestamp exit_time = months{ 5 } / 1 / 2021;

    Trade decr_to_incr = {
        entry_time,
        "DECR",
        "INCR",
        2,
        2,
    };
    EXPECT_NEAR(matcher.get_pnl_from(decr_to_incr, exit_time), 480.0);
    Trade incr_to_decr = {
        entry_time,
        "INCR",
        "DECR",
        1,
        1,
    };
    EXPECT_NEAR(matcher.get_pnl_from(incr_to_decr, exit_time), -240.0);
}

// get_net_pnl
