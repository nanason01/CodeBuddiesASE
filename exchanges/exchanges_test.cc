// Copyright 2022 CodeBuddies ASE Group
//
// Testing suite for exchanges
//

#include <gtest/gtest.h>

#include "exchanges/coinbase.h"
#include "exchanges/kraken.h"

class CoinbaseFixture : public ::testing::Test {
protected:
    // define any variables you want to use in tests here
    int num;
    void SetUp() override {
        num = 420;
        // @TODO
    }
    void TearDown() override {
        // @TODO
        // you could put expects in here
        // but please don't
    }
};

class CryptoComFixture : public ::testing::Test {
protected:
    // define any variables you want to use in tests here
    int num;
    void SetUp() override {
        num = 421;
        // @TODO
    }
    void TearDown() override {
        // @TODO
        // you could put expects in here
        // but please don't
    }
};

TEST_F(CoinbaseFixture, Example) {
    EXPECT_NE("hello", "world");
    EXPECT_EQ(num, 420);
}
TEST_F(CryptoComFixture, Example) {
    EXPECT_NE("hello", "world");
    EXPECT_EQ(num, 421);
}
