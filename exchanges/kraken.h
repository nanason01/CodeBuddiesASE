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

    std::vector<Trade> get_trades() final;

private:
    /*
     * Get trades of user, throw exception on error
     * On success, processes the trades, checks that
     * new trades have been added since the last time
     * function was called, and sends their info to
     * db.
     */
    void query_for_trades(User user);

    /*
     * Retrieve user API key from DB
     * Returns: const string key
     */
    API_key get_api_key(User user);

    /*
     * Generate a nonce for requests to Kraken
     * Returns: string nonce
     */
    std::string generate_nonce();

    /*
     * Generate API-sign for request header, message signature
     * Returns: const string signature
     */
    std::string generate_signature(std::string uri_path, 
                                   std::string post_data, 
                                   std::string nonce,
                                   API_key api_key);

};
