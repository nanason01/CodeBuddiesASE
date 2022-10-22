//
// Mock class of pricer for use by testers
//

#include "pricer.h"
#include "gmock/gmock.h"

class MockPricer : public Pricer {
public:
    MOCK_METHOD(double, get_usd_price, (std::string, Timestamp), (override));
};
