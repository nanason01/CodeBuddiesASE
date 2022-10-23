//
// Testing suite for data.h
//
// I can't build this as data.cc isn't implemented
// so there may be some build errors in here
//

#include "data.h"
#include "exchanges/mock_driver.h"
#include <gtest/gtest.h>

using ::testing::Return;

class DataFixture : public ::testing::Test {
protected:
    // define any variables you want to use in tests here
    int num;

    // use this to call data functions with mocked exchange calls
    Data* data;

    // use this to mock cb, cc functions
    MockExchangeDriver cb, cc;

    void SetUp() override {
        data = new Data(&cb, &cc);

        ON_CALL(cb, get_trades("nick", "nick_key"))
            .WillByDefault(Return(std::vector<Trade>()));

        // EXPECT_CALL can do more fine grained expectation checking
        // ie how many times it should be called
        // but that should be used in a test, not here

        num = 420;
        // @TODO
    }
    void TearDown() override {
        delete data;
        // @TODO
        // you could put expects in here
        // but please don't
    }
};

TEST_F(DataFixture, Example) {
    EXPECT_NE("hello", "world");
    data->add_user({ "nick", "creds" });
    data->register_exchange(
        { "nick", "creds" },
        Exchange::Coinbase,
        "nick_key",
        );
    EXPECT_TRUE(data->get_trades({ "nick", "nick_key" }).empty());
    EXPECT_THROW(data->check_creds({ "nick", "not_nicks_creds" }), InvalidCreds);
    EXPECT_NO_THROW(data->check_creds({ "nick", "creds" }));
    EXPECT_EQ(num, 420);
}