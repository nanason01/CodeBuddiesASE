//
// Coinbase exchange driver
//

#pragma once

#include "common/types.h"
#include "exchanges/driver.h"

#include <vector>

class CoinbaseDriver final : ExchangeDriver {
public:
    CoinbaseDriver(User _user, API_key _key) : ExchangeDriver(_user, _key) {
        check_api_key(_user, _key);
    }

    std::vector<Trade> get_trades() final;

private:
    // throws exceptions if the key is invalid
    void check_api_key(User, API_key);
};