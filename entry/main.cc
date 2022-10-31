// Copyright 2022 CodingBuddies
//
// The main instance of our service
//

#include <iostream>
#include "entry/endpoints.h"

constexpr int PORT_NUM = 18420;

int main() {
    // seed our rng
    srand((unsigned)time(NULL) * getpid());

    /*MockData data;
    MockMatcher matcher;
    set_mode_mock();*/

    //set_up_mock_mode();
    set_mode_prod();


    crow::SimpleApp app;

    CROW_ROUTE(app, "/")(Endpoints::validate_credentials);
    CROW_ROUTE(app, "/getcredentials")(Endpoints::generate_credentials);
    CROW_ROUTE(app, "/refreshcredentials")(Endpoints::refresh_credentials);
    CROW_ROUTE(app, "/trade").methods("POST"_method)(Endpoints::upload_trade);
    CROW_ROUTE(app, "/exchangekey").methods("POST"_method)(Endpoints::upload_exchange_key);
    CROW_ROUTE(app, "/removekey").methods("POST"_method)(Endpoints::remove_exchange_key);
    CROW_ROUTE(app, "/get_annotated_trades")(Endpoints::get_annotated_trades);
    CROW_ROUTE(app, "/year_end_stats")(Endpoints::get_year_end_stats);
    CROW_ROUTE(app, "/trade_pnl").methods("POST"_method)(Endpoints::calc_trade_pnl);
    CROW_ROUTE(app, "/portfolio_pnl")(Endpoints::get_net_pnl);

    app.port(PORT_NUM).multithreaded().run();
}
