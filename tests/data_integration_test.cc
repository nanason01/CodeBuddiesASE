// Copyright 2022 CodeBuddies ASE Group
//
// Integration tests for data, using real exchanges
//

#include <gtest/gtest.h>

#include <filesystem>

#include "data/data.h"
#include "exchanges/kraken.h"

constexpr auto TEST_DB_FILENAME = "data/test_db";
constexpr auto SCHEMA_FILENAME = "data/schema.sql";

class DataIntegration : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        if (!data) {
            std::filesystem::remove(TEST_DB_FILENAME);

            k = new KrakenDriver();

            data = new Data(k, TEST_DB_FILENAME);
            data->exec_sql_file(SCHEMA_FILENAME);
        }
    }

    static void TearDownTestSuite() {
        if (data) {
            delete data;

            delete k;

            std::filesystem::remove(TEST_DB_FILENAME);
        }
    }
    // use this to call data functions with mocked exchange calls
    static Data* data;
    static ExchangeDriver* k;
};

Data* DataIntegration::data = nullptr;
ExchangeDriver* DataIntegration::k = nullptr;

TEST_F(DataIntegration, CheckExchanges) {
    // Add user
    const AuthenticUser user{
        "nick",
        "pass",
        "refr",
    };
    data->add_user(user);

    // Upload trade
    const Trade man_trade{
        from_usa_date(11, 5, 2021),
        "ETH",
        "BTC",
        20.0,
        1.0,  // not verified exchange rate, not using pricer
    };
    data->upload_trade(user, man_trade);

    // Add exchange for user
    const auto pub_key = "J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU";
    const auto pvt_key = "VzP5UOZzOcsjJOw/9gU"
        "1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg==";

    data->register_exchange(user, Exchange::Kraken, pub_key, pvt_key);

    // Check get_trades
    const auto res = data->get_trades(user);
    EXPECT_EQ(res.size(), 1);
    EXPECT_EQ(res.front(), man_trade);

    // Check get_exchanges
    const auto exchs = data->get_exchanges(user);
    EXPECT_EQ(exchs.size(), 1);
    EXPECT_EQ(exchs[ 0 ], Exchange::Kraken);

    // Remove exchange
    data->delete_exchange(user, Exchange::Kraken);

    // Check get_trades
    const auto res2 = data->get_trades(user);
    EXPECT_EQ(res2.size(), 1);
    EXPECT_EQ(res2.front(), man_trade);

    // Check get_exchanges
    EXPECT_TRUE(data->get_exchanges(user).empty());
}
