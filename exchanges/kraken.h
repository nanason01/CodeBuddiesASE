//
// Kraken exchange driver
//

#pragma once

#include "../common/types.h"
#include "driver.h"

#include <vector>

class KrakenDriver final : ExchangeDriver {
public:
    KrakenDriver(User _user, API_key _key, API_key _privatekey) : ExchangeDriver(_user, _key, _privatekey) {
        _uname = _user;
        _ukey = _key;
        _uprivatekey = _privatekey;
    }

    /*
     * On success, processes the trades, checks that
     * new trades have been added since the last time
     * function was called, and sends their info to
     * db.
     */
    std::vector<Trade> get_trades() final;

private:
    User _uname;
    API_key _ukey;
    API_key _uprivatekey;

    /*
     * Get trades of user as a string, throw exception on error
     */
    std::string query_for_trades();

    /*
     * Generate a nonce for requests to Kraken
     * Returns: string nonce
     */
    std::string generate_nonce();

    /*
     * Generate API-sign for request header, message signature
     * Returns: string signature
     */
    std::string generate_signature(std::string uri_path, 
                                   std::string post_data, 
                                   std::string nonce,
                                   API_key api_key_secret);

    /*
     * Function that returns formatted payload to be sent
     * with the query to kraken.
     *
     * For now, just returns a formatted nonce but in the
     * future can be extended with functionality to also
     * specify target trades and exchanges.
     */
    std::string generate_payload(std::string nonce);
};
