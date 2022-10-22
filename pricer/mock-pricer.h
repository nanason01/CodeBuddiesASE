//
// Mock class of pricer for use by testers
//

#include "base-pricer.h"
#include "gmock/gmock.h"

class MockPricerME : public PricerBase {
public:
    MOCK_METHOD(double, get_usd_price, (std::string, Timestamp), (override));
};
