//
// Mock exchange driver
//

#pragma once

#include "common/types.h"
#include "exchanges/driver.h"
#include "gmock/gmock.h"

#include <vector>

class MockExchangeDriver final : public ExchangeDriver {
public:
    MOCK_METHOD(std::vector<Trade>, get_trades, (User user, API_key key), (override));
    MOCK_METHOD(void, check_api_key, (User user, API_key key), (override));
};