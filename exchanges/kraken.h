//
// Kraken exchange driver
//

#pragma once

#include "../common/types.h"
#include "driver.h"

class KrakenDriver final : ExchangeDriver {
public:
    KrakenDriver(User _user, API_key _key) : ExchangeDriver(_user, _key) {
        _uname = _user;
        _ukey = _key;
    }

    // std::vector<Trade> get_trades() final;

// private:

    User _uname;
    API_key _ukey;

    /*
     * Get trades of user, throw exception on error
     * On success, processes the trades, checks that
     * new trades have been added since the last time
     * function was called, and sends their info to
     * db.
     */
    void query_for_trades(User user);

    /*
     * Retrieve user API key and the secret API key from DB
     * Returns: pair of strings
     */
    std::pair<API_key, API_key> get_api_key(User user);

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
