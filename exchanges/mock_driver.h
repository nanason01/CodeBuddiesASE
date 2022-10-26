//
// Mock exchange driver
//

#pragma once

#include "common/types.h"
#include "exchanges/driver.h"
#include "gmock/gmock.h"

#include <vector>

class MockExchangeDriver final: public ExchangeDriver {
public:
    MOCK_METHOD(std::vector<Trade>, get_trades, (User, API_key, API_key), (const, override));
};
