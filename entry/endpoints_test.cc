//
// Testing suite for endpoints.h
//

#include "endpoints.h"
#include "engine/mock_matcher.h"
#include "data/mock_data.h"
#include <gtest/gtest.h>


using ::testing::Return;

class EndpointsFixture: public ::testing::Test {
protected:
    // define any variables you want to use in tests here
    int num;

    // use this to call endpoints functions with mocked exchange calls
    Endpoints* endpoints;

    // use this to mock data, matcher functions
    MockData data;
    MockMatcher matcher;

    void SetUp() override {
        endpoints = new Endpoints(&data, &matcher);

        AuthenticUser nick{ "nick", "nick_key" };

        ON_CALL(data, check_user(nick))
            .WillByDefault(Return());

        // EXPECT_CALL can do more fine grained expectation checking
        // ie how many times it should be called
        // but that should be used in a test, not here

        num = 420;
        // @TODO
    }
    void TearDown() override {
        delete endpoints;
        // @TODO
        // you could put expects in here
        // but please don't
    }
};

TEST_F(EndpointsFixture, Example) {
    EXPECT_NE("hello", "world");
    // there's no methods for me to test yet
    // but if I could, it would look like
    // EXPECT_??(endpoints->??, ??);
    // or
    // EXPECT_THROW(endpoints->??, exception);
    EXPECT_EQ(num, 420);
}
