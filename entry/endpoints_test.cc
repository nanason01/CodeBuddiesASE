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

    MockData data;
    MockMatcher matcher;

    void SetUp() override {
        set_mode_mock(data, matcher);

        AuthenticUser nick{ "nick", "nick_key" };

        ON_CALL(data, check_user(nick))
            .WillByDefault(Return());

        // EXPECT_CALL can do more fine grained expectation checking
        // ie how many times it should be called
        // but that should be used in a test, not here

        num = 420;
        // @TODO
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
