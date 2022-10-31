//
// Testing suite for pricer.h
//

#include "pricer.h"
#include <gtest/gtest.h>

class PricerFixture : public ::testing::Test {
protected:
    // define any variables you want to use in tests here
    int num;
    Pricer p;
    std::string get_asset_id(std::string currency){
     return p.get_asset_id(currency);
    }
    std::string format_timestamp(Timestamp tstamp){
     return p.format_timestamp(tstamp);
    }
    double get_asset_price(std::string currency_id, Timestamp tstamp) {
     return p.get_asset_price(currency_id,tstamp);
    }
    void SetUp() override {
        num = 420;

        // @TODO
    }
    void TearDown() override {
	//delete p;
        // @TODO
        // you could put expects in here
        // but please don't
    }
};
/*
TEST_F(PricerFixture, Example) {
    EXPECT_NE("hello", "world");
    EXPECT_EQ(num, 420);
}
*/
TEST_F(PricerFixture,getAssertID){
    EXPECT_EQ(get_asset_id("algohalf"),"0-5x-long-algorand-token");
    //why is throwing an runtime error (not going to affect the correct cases)
    //test whether get the first currency symbol with cms (there are more than one)
    EXPECT_EQ(get_asset_id("cms"),"comsa");
    
    EXPECT_EQ(get_asset_id("zch"),"zilchess");
    EXPECT_EQ(get_asset_id("infp"),"infinitypad");
    //EXPECT_THROW(get_asset_id("*"),std::runtime_error);
}

TEST_F(PricerFixture,formatTime){
    Timestamp t1 = from_usa_date(5,8,2020);
    Timestamp t2 = from_usa_date(6,1,2018);
    EXPECT_EQ(format_timestamp(t1),"8-5-2020");
    EXPECT_EQ(format_timestamp(t2),"1-6-2018");
}

TEST_F(PricerFixture,assetPrice){
    Timestamp t3 = from_usa_date(1,2,2018);
    EXPECT_THROW(get_asset_price("infp",t3),std::runtime_error);
		    }
