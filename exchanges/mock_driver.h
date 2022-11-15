// Copyright 2022 CodeBuddies ASE Group
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
    MOCK_METHOD(std::vector<Trade>, get_trades, (API_key, API_key), (override));
};
