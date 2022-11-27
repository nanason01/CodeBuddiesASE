// Copyright 2022 CodeBuddies ASE Group
//
// Testing suite for exchanges
//

#include <gtest/gtest.h>

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
        for (auto x : trades) {
            std::cout << x.sold_currency << " " << x.bought_currency << " " << x.sold_amount << " " << x.bought_amount << std::endl;
        }
    });

    EXPECT_GE(trades.size(), 0);
}

TEST_F(KrakenFixture, get_trades_invalidkey_mia1) {
    EXPECT_THROW(kr.get_trades("",
                    "VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg=="),
                 APICredsMissing);
}

TEST_F(KrakenFixture, get_trades_invalidkey_mia2) {
    EXPECT_THROW(kr.get_trades("J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU",
                  ""), APICredsMissing);
}