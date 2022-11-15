// Copyright 2022 CodeBuddies ASE Group
//
// Testing suite for data.h
//

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <vector>

#include "data/data.h"
#include "exchanges/coinbase.h"
#include "exchanges/helpers.h"
#include "exchanges/kraken.h"
#include "exchanges/mock_driver.h"


using ::testing::Return;

constexpr auto TEST_DB_FILENAME = "data/test_db";
constexpr auto SCHEMA_FILENAME = "data/schema.sql";

class DataFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        if (!data) {
            std::filesystem::remove(TEST_DB_FILENAME);

            cb = new MockExchangeDriver();
            k = new MockExchangeDriver();

            data = new Data(cb, k, TEST_DB_FILENAME);
            data->exec_sql_file(SCHEMA_FILENAME);
        }
    }

    static void TearDownTestSuite() {
        if (data) {
            delete data;

            delete cb;
            delete k;

            std::filesystem::remove(TEST_DB_FILENAME);
        }
    }

    // use this to call data functions with mocked exchange calls
    static Data* data;

    // use this to mock cb, cc functions
    static MockExchangeDriver* cb, * k;
};

Data* DataFixture::data = nullptr;
MockExchangeDriver* DataFixture::cb = nullptr;
MockExchangeDriver* DataFixture::k = nullptr;

TEST_F(DataFixture, AddUserTest) {
    // data->create_table();
    // data->remove_user({"nick","creds"});
    data->add_user({ "nick", "creds", "refrs" });
    EXPECT_THROW(data->add_user({ "nick", "creds", "refrs" }), UserExists);
    data->remove_user({ "nick", "creds", "refrs" });
}
TEST_F(DataFixture, UpdateCreds) {
    data->add_user({ "jennice1", "creds66", "refrs66" });
    data->update_user_creds({ "jennice1", "creds66", "refrs66" }, "creds77", "refrs77");
    EXPECT_THROW(data->check_user({ "jennice1", "creds66", "refrs66" }), InvalidCreds);
    data->remove_user({ "jennice1", "creds77", "refrs77" });
}

TEST_F(DataFixture, UploadGetTrades) {
    // data->create_table();
    Timestamp t1 = from_usa_date(1, 2, 2018);
    Trade s1{
        t1,
        "USD",
        "INCR",
        1000.0,
        1.0,
    };
    Timestamp t2 = from_usa_date(5, 8, 2019);
    Trade s2{
        t2,
        "INCR",
        "DECR",
        900.0,
        2.0,
    };
    data->add_user({ "urvee", "creds1", "refrs1" });
    data->upload_trade({ "urvee", "creds1", "refrs1" }, s1);

    data->upload_trade({ "urvee", "creds1", "refrs1" }, s2);

    EXPECT_THROW(data->upload_trade({ "urvee", "creds2", "refrs2" }, s2), InvalidCreds);
    EXPECT_THROW(data->upload_trade({ "Alek", "creds3", "refrs3" }, s2), UserNotFound);

    std::vector<Trade> res = data->get_trades({ "urvee", "creds1", "refrs1" });
    Trade s2_res = res.back();
    res.pop_back();

    Trade s1_res = res.back();
    res.pop_back();

    // I assume that I cannot change any struct in type.h to set operator ==
    EXPECT_EQ(s1, s1_res);
    EXPECT_EQ(s2, s2_res);
    data->remove_user({ "urvee", "creds1", "refrs1" });
}

TEST_F(DataFixture, emptyTrade) {
    EXPECT_THROW(data->get_trades({ "nico", "cred4", "refrs4" }), UserNotFound);
    data->add_user({ "nico", "cred4", "refrs4" });
    EXPECT_TRUE(data->get_trades({ "nico", "cred4", "refrs4" }).empty());
    data->remove_user({ "nico", "cred4", "refrs4" });
}

TEST_F(DataFixture, RegisterExchange) {
    data->add_user({ "Franck", "creds6", "refrs6" });
    EXPECT_THROW(data->register_exchange(
        { "jennice", "creds5", "refrs5" },
        Exchange::Coinbase, "jennice_key1", "jennice_prikey1"),
        UserNotFound);
    data->register_exchange({ "Franck", "creds6", "refrs3" }, Exchange::Coinbase, "alek_key1", "alek_prikey1");
    data->register_exchange({ "Franck", "creds6", "refrs3" }, Exchange::Kraken, "alek_key2", "alek_prikey2");

    std::vector<Exchange> res = data->get_exchanges({ "Franck", "creds6", "refrs6" });
    Exchange coinV = Exchange::Coinbase;
    Exchange kraV = Exchange::Kraken;

    Exchange eK = res.back();
    res.pop_back();
    EXPECT_EQ(kraV, eK);

    Exchange eC = res.back();
    res.pop_back();
    EXPECT_EQ(coinV, eC);
    data->remove_user({ "Franck", "creds6", "refrs6" });
}

TEST_F(DataFixture, LastUpdate) {
    data->add_user({ "n1", "c1", "r1" });
    data->register_exchange({ "n1", "c1", "r1" }, Exchange::Kraken, "n1_key", "n1_prikey");
    Timestamp tn = data->get_last_update({ "n1", "c1", "r1" }, Exchange::Kraken);
    std::tm last = *std::localtime(&tn);
    std::cout << std::put_time(&last, "%c %Z") << std::endl;
    data->remove_user({ "n1", "c1", "r1" });
}
