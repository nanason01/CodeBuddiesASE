//
// Interface for inputting new data and retrieving user data
//

#pragma once

#include "base_data.h"
#include "gmock/gmock.h"

#include <exception>
#include <chrono>

class MockData final : public BaseData {
public:
    MOCK_METHOD(void, add_user, (AuthenticUser user), (override));
    MOCK_METHOD(void, remove_user, (AuthenticUser user), (override));
    MOCK_METHOD(void, register_exchange, (AuthenticUser user, Exchange exch, API_key key), (override));
    MOCK_METHOD(void, upload_trade, (AuthenticUser user, Trade trade), (override));
    MOCK_METHOD(std::vector<Trade>, get_trades, (AuthenticUser user, Exchange e), (override));
    MOCK_METHOD(std::vector<Exchange>, get_exchanges, (AuthenticUser user), (override));
    MOCK_METHOD(void, check_user, (AuthenticUser user), (override));
    MOCK_METHOD(void, check_creds, (AuthenticUser user), (override));
    MOCK_METHOD(Timestamp, get_last_update, (AuthenticUser user, Exchange e), (override));
};