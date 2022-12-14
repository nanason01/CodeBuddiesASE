// Copyright 2022 CodeBuddies ASE Group
//
// API endpoints
//

#pragma once

#include <openssl/sha.h>
#include <crow.h>

#include <memory>
#include <string>

#include "common/types.h"
#include "data/data.h"
#include "data/mock_data.h"
#include "engine/matcher.h"
#include "engine/mock_matcher.h"

// errors
struct NoAuthHeader : std::exception {
    const char* what() const noexcept override {
        return "No Authorization header found.";
    }
};

class Endpoints {
    static std::unique_ptr<BaseData> data;
    static std::unique_ptr<BaseMatcher> matcher;

public:
    static void set_mode_prod(const std::string& db_filename, const std::string& schema_filename);
    static void set_mode_mock();

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
