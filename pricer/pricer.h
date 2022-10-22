//
// The interface to query the price of a security on a day
//

#pragma once

#include <chrono>
#include <string>

#include "common/types.h"
#include "base_pricer.h"

class Pricer final : public PricerBase {
public:
    // returns the price per unit of currency on date
    virtual double get_usd_price(std::string currency, Timestamp timestamp = now()) final;
};
