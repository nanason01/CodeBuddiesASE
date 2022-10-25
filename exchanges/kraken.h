//
// Kraken exchange driver
// Copyright 2022 CodingBuddies

#pragma once

#include <vector>
#include <string>

#include "common/types.h"
#include "exchanges/driver.h"

class KrakenDriver final : ExchangeDriver {
 public:
    KrakenDriver() : ExchangeDriver() {
    }

    /*
     * On success, processes the trades, checks that
     * new trades have been added since the last time
     * function was called, and sends their info to
     * db.
     */
    std::vector<Trade> get_trades(API_key public_key,
                                  API_key private_key) final;

 private:
    /*
     * Get trades of user as a string, throw exception on error
     */
    std::string query_for_trades(API_key public_key, API_key private_key);

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
                                   std::vector<unsigned char> api_key_secret);

    /*
     * Function that returns formatted payload to be sent
     * with the query to kraken.
     *
     * For now, just returns a formatted nonce but in the
     * future can be extended with functionality to also
     * specify target trades and exchanges.
     */
    std::string generate_payload(std::string nonce);

    /*
     * Generates the URL to query.
     */
    std::string generate_url();

    /*
     * Generates the URI
     */
    std::string generate_path();
};
