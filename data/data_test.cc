//
// Testing suite for data.h
//

#include "data.h"
#include "exchanges/mock_driver.h"
#include <gtest/gtest.h>

using ::testing::Return;

const std::string TEST_DB_FILENAME = "db";

class DataFixture: public ::testing::Test {
protected:
    // define any variables you want to use in tests here
    int num;

    // use this to call data functions with mocked exchange calls
    Data* data;

    // use this to mock cb, cc functions
    MockExchangeDriver cb, k;

    void SetUp() override {
        data = new Data(&cb, &k, TEST_DB_FILENAME);

        ON_CALL(cb, get_trades("nick", "nick_key"))
            .WillByDefault(Return(std::vector<Trade>()));
	/*
        ON_CALL(cb, get_exchanges("urvee","urvee_key"))
	    .WillByDefault(Return(std::vector<Exchange>()));
	*/
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

TEST_F(DataFixture,AddUserTest) {
	//data->create_table();
	data->add_user({"nick","creds"});
	EXPECT_THROW(data->add_user({"nick","creds"}), UserExists);
	data->remove_user({"nick","creds"});	
}

TEST_F(DataFixture,UploadGetTrades){
	/*data->remove_user({"urvee","creds1"});*/
	//data->create_table();
	Timestamp t1 = from_usa_date(1,2,2018);
	Trade s1{
		t1,
		"USD",
		"INCR",
		1000.0,
		1.0,
	};
	Timestamp t2 = from_usa_date(5,8,2019);
	Trade s2{
		t2,
		"INCR",
		"DECR",
		900.0,
		2.0,
	};
	data->add_user({"urvee","creds1"});	
	data->upload_trade({"urvee","creds1"},s1);
	
	data->upload_trade({"urvee","creds1"},s2);
	
	EXPECT_THROW(data->upload_trade({"urvee","creds2"},s2),InvalidCreds);
	EXPECT_THROW(data->upload_trade({"Alek","creds3"},s2),UserNotFound);
	
	std::vector<Trade> res = data->get_trades({"urvee","creds1"});
	Trade s2_res = res.back();
	res.pop_back();
	
	
	Trade s1_res = res.back();
	res.pop_back();
	

	//I assume that I cannot change any struct in type.h to set operator ==
	EXPECT_EQ(s1,s1_res);
	EXPECT_EQ(s2,s2_res);
	data->remove_user({"urvee","creds1"});
}


/*
TEST_F(DataFixture, Example) {
    EXPECT_NE("hello", "world");
    data->add_user({ "nick", "creds" });
    data->register_exchange(
        { "nick", "creds" },
        Exchange::Coinbase,
        "nick_key",
        "nick_pri_key"
    );
    EXPECT_TRUE(data->get_trades({ "nick", "nick_key" }).empty());
    EXPECT_THROW(data->check_user({ "nick", "not_nicks_creds" }), InvalidCreds);
    EXPECT_NO_THROW(data->check_user({ "nick", "creds" }));
    EXPECT_EQ(num, 420);
}*/
