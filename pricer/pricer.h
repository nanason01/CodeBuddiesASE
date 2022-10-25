//
// The interface to query the price of a security on a day
//

#pragma once

#include <chrono>
#include <string>

#include "common/types.h"
#include "base_pricer.h"

/*
 * Convert a crow::json element to a string.
 */
std::string convert_to_string(const crow::json::rvalue jrvalue);

/*
 * Write callback function for cURL
 */
size_t pricer_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

class Pricer final : public PricerBase {
public:
    // returns the price per unit of currency on date
    virtual double get_usd_price(std::string currency_pair) final;

private:
    double process_response(std::string response, std::string currency_pair);
};
