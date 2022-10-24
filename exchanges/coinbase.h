//
// Coinbase exchange driver
//

#pragma once

#include "../common/types.h"
#include "driver.h"

#include <vector>

class CoinbaseDriver final : ExchangeDriver {
public:
    CoinbaseDriver(User _user, API_key _key, API_key _privatekey, std::string _password, std::string _uid) : ExchangeDriver(_user, _key, _privatekey) {
        _uname = _user;
        _upassword = _password;
        _ukey = _key;
        _uprivatekey = _privatekey;
        _uaccountid = _uid;
    }

    std::vector<Trade> get_trades() final;

private:
    User _uname; 
    std::string _upassword;
    std::string _uaccountid;

    API_key _ukey;
    API_key _uprivatekey;

    /*
     * Get trades of a user, return as string.
     */
    std::string query_for_trades();

    /*
     * Generates a timestamp for request expiration/validation.
     */
    std::string generate_timestamp();

    /*
     * Generate a full
     */
    std::string generate_url();

    /*
     * Generate a URI
     */
    std::string generate_path();

    /*
     * Generate a signature for the message, using HMAC SHA256.
     */
    std::string generate_signature(std::string timestamp, std::string method, std::string path);
};
