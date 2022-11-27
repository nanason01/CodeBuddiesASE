// Copyright 2022 CodeBuddies ASE Group
//
// Testing suite for pricer.h
//

#include "pricer/pricer.h"

#include <gtest/gtest.h>

class PricerFixture : public ::testing::Test {
protected:
    Pricer p;
};

TEST_F(PricerFixture, assetPrice_btc_1) {
    Timestamp ts = from_usa_date(3, 3, 2017);
    double ans;

    try {
        ans = p.get_usd_price("BTC", ts);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }

    EXPECT_NEAR(ans, 1289.54, 0.01);
}

TEST_F(PricerFixture, assetPrice_btc_2) {
    Timestamp ts = from_usa_date(3, 3, 2017);
    double ans;

    try {
        ans = p.get_usd_price("BtC", ts);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }

    EXPECT_NEAR(ans, 1289.54, 0.01);
}

TEST_F(PricerFixture, assetPrice_btc_3) {
    Timestamp ts = from_usa_date(3, 3, 2017);
    double ans;

    try {
        ans = p.get_usd_price("btc", ts);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }

    EXPECT_NEAR(ans, 1289.54, 0.01);
}

TEST_F(PricerFixture, assetPrice_btc_4) {
    Timestamp ts = from_usa_date(12, 30, 2017);
    double ans;

    try {
        ans = p.get_usd_price("BTC", ts);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }

    EXPECT_NEAR(ans, 13620.36, 0.01);
}

TEST_F(PricerFixture, assetPrice_all) {
    Timestamp ts = from_usa_date(3, 3, 2021);
    double ans;
    for (const auto& [key, value] : p.token_name_map) {
        try {
            ans = p.get_usd_price(key, ts);
        } catch (std::exception const &e) {
            EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
        }

        EXPECT_GT(ans, 0);
    }
}

TEST_F(PricerFixture, assetPrice_bad) {
    Timestamp ts = from_usa_date(12, 30, 2017);
    EXPECT_THROW(p.get_usd_price("Alejandro", ts), NoRecordsFound);
}
