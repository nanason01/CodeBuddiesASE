//
// The interface to query the price of a security on a day
// Copyright 2022 CodingBuddies

#pragma once

#include <chrono>
#include <string>

#include "pricer/base_pricer.h"
#include "common/types.h"

/*
 * Convert a crow::json element to a string.
 */
std::string convert_to_string(const crow::json::rvalue jrvalue);

/*
 * Write callback function for cURL
 */
size_t pricer_write_callback(char *ptr, size_t size,
                            size_t nmemb, void *userdata);

class Pricer final : public PricerBase {
 public:
    // returns the price per unit of currency on date
    double get_usd_price(std::string currency_pair,
                            Timestamp tstamp = now()) final;

 private:
    std::string get_asset_id(std::string currency);

    std::string perform_curl_request(std::string url);

    double get_asset_price(std::string currency_id, Timestamp tstamp);

    std::string format_timestamp(Timestamp tstamp);
};
