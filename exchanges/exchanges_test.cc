//
// Testing suite for exchanges
//

#include "coinbase.h"
#include "crypto_com.h"
#include <gtest/gtest.h>

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