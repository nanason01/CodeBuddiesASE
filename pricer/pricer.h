// Copyright 2022 CodeBuddies ASE Group
//
// The interface to query the price of a security on a day
//

#pragma once

#include <crow.h>

#include <unordered_map>
#include <string>

#include "pricer/base_pricer.h"
#include "common/types.h"

/*
 * Write callback function for cURL
 */
size_t pricer_write_callback(char* ptr, size_t size,
    size_t nmemb, void* userdata);

class Pricer final : public PricerBase {
public:
    friend class PricerFixture;
    // returns the price per unit of currency on date
    double get_usd_price(std::string currency_pair,
        Timestamp tstamp = now()) final;

    /*
     * Unordered map of coin id-symbol mappings for
     * get_asset_id. symbols are key, ids are values.
     */
    std::unordered_map<std::string, std::string> token_name_map = {
        {"matic", "matic-network"},
        {"link", "chainlink"},
        {"algo", "algorand"},
        {"ltc",  "litecoin"},
        {"eth",  "ethereum"},
        {"dot",  "polkadot"},
        {"btc",  "bitcoin"},
        {"uni",  "uniswap"},
        {"xrp",  "ripple"},
        {"sol",  "solana"}
    };

private:
    std::string get_asset_id(std::string currency);

    std::string perform_curl_request(std::string url);

    double get_asset_price(std::string currency_id, Timestamp tstamp);

    std::string format_timestamp(Timestamp tstamp);
};
