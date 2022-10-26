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
    MOCK_METHOD(void, add_user, (const AuthenticUser&), (override));
    MOCK_METHOD(void, remove_user, (const AuthenticUser&)), (override));
    MOCK_METHOD(void, register_exchange, (const AuthenticUser&, Exchange, API_key, API_key), (override));
    MOCK_METHOD(void, upload_trade, (const AuthenticUser&, const Trade&), (override));
    MOCK_METHOD(std::vector<Trade>, get_trades, (const AuthenticUser&), (override));
    MOCK_METHOD(std::vector<Exchange>, get_exchanges, (const AuthenticUser&), (override));
    MOCK_METHOD(void, check_user, (const AuthenticUser&), (const, override));
    MOCK_METHOD(Timestamp, get_last_update, (const AuthenticUser&, Exchange), (const, override));
};