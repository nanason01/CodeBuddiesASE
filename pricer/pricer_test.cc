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

TEST_F(PricerFixture, assetPrice) {
    Timestamp ts = from_usa_date(3, 3, 2017);
    EXPECT_NEAR(p.get_usd_price("BTC", ts), 1289.54, 0.01);
}
