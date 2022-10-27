//
// API endpoints
//

#pragma once

#include <memory>

#include <openssl/sha.h>

#include "common/types.h"
#include "data/data.h"
#include "data/mock_data.h"
#include "engine/matcher.h"
#include "engine/mock_matcher.h"
#include "crow.h"

class Endpoints {

    static std::unique_ptr<BaseData> data;
    static std::unique_ptr<BaseMatcher> matcher;

public:
    // default to production mode: actual backing not mocks
    static void set_mode_test(MockData& mock_data, MockMatcher& mock_matcher);

    static crow::response validate_credentials(const crow::request& req);
    static crow::response generate_credentials(const crow::request& req);
    static crow::response refresh_credentials(const crow::request& req);
    static crow::response upload_trade(const crow::request& req);
    static crow::response upload_exchange_key(const crow::request& req);
    static crow::response remove_exchange_key(const crow::request& req);
    static crow::response get_annotated_trades(const crow::request& req);
    static crow::response get_year_end_stats(const crow::request& req);
    static crow::response calc_trade_pnl(const crow::request& req);
    static crow::response get_net_pnl(const crow::request& req);

};
