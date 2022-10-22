//
// Mock class of pricer for use by testers
//

#pragma once

#include "base_pricer.h"
#include "gmock/gmock.h"

class MockPricer final : public PricerBase {
public:
    MOCK_METHOD(double, get_usd_price, (std::string, Timestamp), (override));
};
