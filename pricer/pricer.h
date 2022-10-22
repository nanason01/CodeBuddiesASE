//
// The interface to query the price of a security on a day
//

#pragma once

#include <chrono>
#include <string>

#include "common/types.h"

class Pricer {
public:
    virtual ~Pricer() = default;
    // returns the price per unit of currency on date
    virtual double get_usd_price(std::string currency, Timestamp timestamp = now());
};
