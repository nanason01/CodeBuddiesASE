//
// Testing suite for pricer.h
//

#include "pricer.h"
#include <gtest/gtest.h>

class PricerFixture : public ::testing::Test {
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

TEST_F(PricerFixture, Example) {
    EXPECT_NE("hello", "world");
    EXPECT_EQ(num, 420);
}