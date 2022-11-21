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

TEST_F(PricerFixture, assetPrice_10) {
    Timestamp ts = from_usa_date(3, 3, 2017);
    EXPECT_NEAR(p.get_usd_price("BTC", ts), 1289.54, 0.01);
}

TEST_F(PricerFixture, assetPrice_11) {
    Timestamp ts = from_usa_date(3, 3, 2017);
    EXPECT_NEAR(p.get_usd_price("BtC", ts), 1289.54, 0.01);
}

TEST_F(PricerFixture, assetPrice_11) {
    Timestamp ts = from_usa_date(3, 3, 2017);
    EXPECT_NEAR(p.get_usd_price("btc", ts), 1289.54, 0.01);
}

TEST_F(PricerFixture, assetPrice_20) {
    Timestamp ts = from_usa_date(12, 30, 2017);
    EXPECT_NEAR(p.get_usd_price("BTC", ts), 13620.36, 0.01);
}

TEST_F(PricerFixture, bad_test) {
    Timestamp ts = from_usa_date(12, 30, 2017);
    EXPECT_EQ(p.get_usd_price("Alejandro", ts), 0);
}
