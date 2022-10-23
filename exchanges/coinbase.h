//
// Coinbase exchange driver
//

#pragma once

#include "common/types.h"
#include "exchanges/driver.h"

#include <vector>

class CoinbaseDriver final : public ExchangeDriver {
public:

    std::vector<Trade> get_trades(User user, API_key key) final;

    // throws exceptions if the key is invalid
    // @TODO: is user necessary here?
    void check_api_key(User user, API_key key) final;
};