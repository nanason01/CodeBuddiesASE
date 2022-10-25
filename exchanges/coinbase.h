//
// Coinbase exchange driver
// Copyright 2022 CodingBuddies

#pragma once

#include <vector>
#include <string>

#include "common/types.h"
#include "exchanges/driver.h"

class CoinbaseDriver final : public ExchangeDriver {
 public:
    CoinbaseDriver() : ExchangeDriver() {}

    std::vector<Trade> get_trades(API_key public_key,
                                          API_key private_key) final;

 private:
    /*
     * Get trades of a user, return as string.
     */
    std::string query_for_trades(API_key public_key, API_key private_key);

    /*
     * Generates a timestamp for request expiration/validation.
     */
    std::string generate_timestamp();

    /*
     * Generate a full
     */
    std::string generate_url(std::string account_id);

    /*
     * Generate a URI
     */
    std::string generate_path(std::string account_id);

    /*
     * Generate a signature for the message, using HMAC SHA256.
     */
    std::string generate_signature(std::string timestamp, std::string method,
                                   std::string path, API_key private_key);
};
