//
// Kraken exchange driver
//

#pragma once

#include "../common/types.h"
#include "driver.h"

#include <vector>

class KrakenDriver final : ExchangeDriver {
public:
    KrakenDriver(User _user, API_key _key) : ExchangeDriver(_user, _key) {
        check_api_key(_user, _key);
    }

    // throws exceptions if the key is invalid
    // @TODO: is user param needed?
    void check_api_key(User user, API_key key) final;
};
