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
        EXPECT_NEAR(ans, 1289.54, 0.01);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }
}

TEST_F(PricerFixture, assetPrice_btc_2) {
    Timestamp ts = from_usa_date(3, 3, 2017);
    double ans;

    try {
        ans = p.get_usd_price("BtC", ts);
        EXPECT_NEAR(ans, 1289.54, 0.01);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }
}

TEST_F(PricerFixture, assetPrice_btc_3) {
    Timestamp ts = from_usa_date(3, 3, 2017);
    double ans;

    try {
        ans = p.get_usd_price("btc", ts);
        EXPECT_NEAR(ans, 1289.54, 0.01);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }
}

TEST_F(PricerFixture, assetPrice_btc_4) {
    Timestamp ts = from_usa_date(12, 30, 2017);
    double ans;

    try {
        ans = p.get_usd_price("BTC", ts);
        EXPECT_NEAR(ans, 13620.36, 0.01);
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
    }
}

TEST_F(PricerFixture, assetPrice_all) {
    Timestamp ts = from_usa_date(3, 3, 2021);
    double ans;
    for (const auto& [key, value] : p.token_name_map) {
        try {
            ans = p.get_usd_price(key, ts);
            EXPECT_GT(ans, 0);
        } catch (std::exception const &e) {
            EXPECT_EQ(e.what(), RateLimitedQuery{}.what());
        }
    }
}

TEST_F(PricerFixture, assetPrice_bad) {
    Timestamp ts = from_usa_date(12, 30, 2017);
    EXPECT_THROW(p.get_usd_price("Alejandro", ts), NoRecordsFound);
}

TEST_F(PricerFixture, assetPrice_eth_invalidDate) {
    Timestamp ts = from_usa_date(12, 30, 2010);

    try {
        p.get_usd_price("eth", ts);
    }

    catch (RateLimitedQuery const &e) { }
    catch (NoRecordsFound const &e) { }
}

/*
{"matic", "matic-network"},
{"link", "chainlink"},
{"algo", "algorand"},
{"ltc",  "litecoin"},
{"eth",  "ethereum"},
{"dot",  "polkadot"},
{"btc",  "bitcoin"},
{"uni",  "uniswap"},
{"xrp",  "ripple"},
{"sol",  "solana"}
*/
