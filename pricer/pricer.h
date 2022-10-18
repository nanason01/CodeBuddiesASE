//
// The interface to query the price of a security on a day
//

#pragma once

#include <chrono>
#include <string>

#include "common/types.h"

using namespace std::chrono_literals;
using Timestamp = std::chrono::time_point<std::chrono::system_clock>;

// returns the price per unit of currency on date
double get_usd_price(std::string currency, Timestamp timestamp);
