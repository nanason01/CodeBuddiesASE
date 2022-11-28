// Copyright 2022 CodeBuddies ASE Group
//
// Testing suite for exchanges
//

#include <gtest/gtest.h>
#include <crow.h>

#include "exchanges/helpers.h"
#include "exchanges/kraken.h"

class KrakenFixture : public ::testing::Test {
protected:
    KrakenDriver kr;
};

TEST_F(KrakenFixture, get_trades_valid) {
    std::vector<Trade> trades;
    EXPECT_NO_THROW({
        trades = kr.get_trades("J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU",
           "VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg==");
    });

    EXPECT_GE(trades.size(), 0);
}

TEST_F(KrakenFixture, get_trades_invalidkey_mia1) {
    try {
        kr.get_trades("", "VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg==");
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), APICredsMissing{}.what());
    }
}

TEST_F(KrakenFixture, get_trades_invalidkey_mia2) {
    try {
        kr.get_trades("J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU", "");
    } catch (std::exception const &e) {
        EXPECT_EQ(e.what(), APICredsMissing{}.what());
    }
}

TEST_F(KrakenFixture, get_res_conversion_to_string) {
    auto jsonified_txs   = crow::json::load("{\"result\": {},\"error\": [\"EGeneral:Invalid arguments\"]}");
    std::string expected = "\"result\":{},\"error\":[\"EGeneral:Invalid arguments\"]";
    EXPECT_EQ(convert_to_string(jsonified_txs), expected);
}
