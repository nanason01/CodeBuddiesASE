//
// Coinbase exchange driver
//

#pragma once

#include "../common/types.h"
#include "driver.h"

#include <vector>

class CoinbaseDriver final : public ExchangeDriver {
public:
    CoinbaseDriver(User _user, API_key _key, API_key _privatekey) : ExchangeDriver(_user, _key, _privatekey) {
        _uname = _user;
        _ukey = _key;
        _uprivatekey = _privatekey;
    }

    std::vector<Trade> get_trades(User user, API_key key) final;

private:
    User _uname;
    API_key _ukey;
    API_key _uprivatekey;
};
