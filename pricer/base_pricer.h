// Copyright 2022 CodeBuddies ASE Group
//
// The interface to query the price of a security on a day
//

#pragma once

#include <string>

#include "common/types.h"

class PricerBase {
public:
    virtual ~PricerBase() = default;
    // returns the price per unit of currency on date
    virtual double get_usd_price(std::string currency_pair,
        Timestamp tstamp = now()) = 0;
};
