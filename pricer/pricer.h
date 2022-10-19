//
// The interface to query the price of a security on a day
//

#pragma once

#include <chrono>
#include <string>

#include "common/types.h"

// returns the price per unit of currency on date
double get_usd_price(std::string currency, Timestamp timestamp = std::chrono::system_clock::now());
