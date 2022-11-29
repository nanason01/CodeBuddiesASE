// Copyright 2022 CodeBuddies ASE Group
//
// Implementation of API entpoints
//
// the methods do what they say they do
//

#include "entry/endpoints.h"

#include <string>

constexpr int CLIENTIDLEN = 4;
constexpr int APIKEYLEN = 4;

using crow::response;
using crow::request;
using std::string;
using std::cerr;
using std::endl;

std::unique_ptr<BaseData> Endpoints::data = std::unique_ptr<BaseData>();
std::unique_ptr<BaseMatcher> Endpoints::matcher = std::unique_ptr<BaseMatcher>();;

void Endpoints::set_mode_prod(const std::string& db_filename, const std::string& schema_filename) {
    data = std::make_unique<Data>(db_filename);
    data->exec_sql_file(schema_filename);
    matcher = std::make_unique<Matcher>();
}

void Endpoints::set_mode_mock() {
    data = std::make_unique<MockData>();
    matcher = std::make_unique<MockMatcher>();
}

static string gen_random_str(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string ret(sizeof(len), 'a');

    for (int i = 0; i < len; ++i)
        ret[ i ] = alphanum[ rand() % sizeof(alphanum) ];

    return ret;
}

static string hash_str(string key) {
    unsigned char hash[ SHA256_DIGEST_LENGTH ];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, key.c_str(), key.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[ i ]);
    return ss.str();
}

static AuthenticUser parse_user(const request& req) {
    return AuthenticUser{
           req.get_header_value("Authorization")
               .substr(7).substr(0, CLIENTIDLEN),
           hash_str(req.get_header_value("Authorization")
               .substr(7).substr(CLIENTIDLEN, APIKEYLEN))
    };
}

static AuthenticUser parse_user_refr_creds(const request& req) {
    return AuthenticUser{
        req.get_header_value("Authorization")
            .substr(7).substr(0, CLIENTIDLEN),
        "",
        hash_str(req.get_header_value("Authorization")
            .substr(7).substr(CLIENTIDLEN, APIKEYLEN))
    };
}

response Endpoints::validate_credentials(const request& req) {
    const auto user = parse_user(req);

    try {
        data->check_user(user);
    } catch (UserNotFound& e) {
        cerr << "validate_credentials: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "validate_credentials: " << e.what() << endl;
        return response(401);
    }

    return response(200);
}

response Endpoints::generate_credentials(const request& req) {
    crow::json::wvalue ret_val;

    string client_id = gen_random_str(CLIENTIDLEN);
    string api_key = gen_random_str(APIKEYLEN);
    string refresh_key = gen_random_str(APIKEYLEN);

    AuthenticUser new_user{
        client_id,
        hash_str(api_key),
        hash_str(refresh_key)
    };

    ret_val[ "client_id" ] = client_id;
    ret_val[ "api_key" ] = client_id + api_key;
    ret_val[ "refresh_token" ] = client_id + refresh_key;

    // FIXME: I think it should be catch UserExists
    try {
        data->add_user(new_user);
    } catch (UserNotFound& e) {
        cerr << "generate_credentials: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "generate_credentials: " << e.what() << endl;
        return response(401);
    }
    crow::response res(200, ret_val);
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
}

response Endpoints::refresh_credentials(const request& req) {
    crow::json::wvalue resp;

    const auto user = parse_user_refr_creds(req);

    string new_api_key = gen_random_str(APIKEYLEN);
    string new_refresh_key = gen_random_str(APIKEYLEN);

    resp[ "client_id" ] = user.user;
    resp[ "api_key" ] = user.user + new_api_key;
    resp[ "refresh_token" ] = user.user + new_refresh_key;

    try {
        data->update_user_creds(user, hash_str(new_api_key), hash_str(new_refresh_key));
    } catch (UserNotFound& e) {
        cerr << "refresh_credentials: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "refresh_credentials: " << e.what() << endl;
        return response(401);
    }

    crow::response res(200, resp);
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
}

static Timestamp field_to_ts(string ts_str) {
    std::cout << "field_to_ts got " << ts_str << std::endl;

    int first = -1;
    int second = -1;
    for (int i = 0; i < static_cast<int>(ts_str.size()); i++) {
        if (ts_str[i] == '/') {
            if (first == -1)
                first = i;
            else if (second == -1)
                second = i;
            else
                break;
        }
    }
    int month = std::stoi(ts_str.substr(0, first));
    int day = std::stoi(ts_str.substr(first + 1, second));
    int year = std::stoi(ts_str.substr(second + 1, second + 5));

    /* std::cout << month << std::endl;
    std::cout << day << std::endl;
    std::cout << year << std::endl; */

    if (day < 0 || day > 31 || month < 0 || month > 12)
        throw std::invalid_argument("Invalid Date");

    return from_usa_date(month, day, year);
}

static double field_to_double(string double_str) {
    std::cout << "field_to_double got " << double_str << std::endl;
    return std::stod(double_str);
    // return -1.0;
}

response Endpoints::upload_trade(const request& req) {
    const AuthenticUser user = parse_user(req);
    crow::json::wvalue resp;

    auto body = crow::json::load(req.body);

    try {
        const Trade trade_in{
            field_to_ts(string(body[ "timestamp" ])),
            string(body[ "sold_currency" ]),
            string(body[ "bought_currency" ]),
            field_to_double(string(body[ "sold_amount" ])),
            field_to_double(string(body[ "bought_amount" ]))
        };
        data->upload_trade(user, trade_in);
    } catch (UserNotFound& e) {
        cerr << "validate_credentials: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "validate_credentials: " << e.what() << endl;
        return response(401);
    } catch (...) {
        cerr << "Invalid timestamp" << endl;
        return response(400);
    }

    resp[ "status" ] = "SUCCESS";
    crow::response res(200, resp);
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
}

response Endpoints::upload_exchange_key(const request& req) {
    AuthenticUser user = parse_user(req);
    crow::json::wvalue resp;
    auto body = crow::json::load(req.body);

    Exchange exch = from_string(string(body[ "exchange" ]));
    // the names in the json are consistent with legacy
    API_key pub_key = string(body[ "readkey" ]);
    API_key pvt_key = string(body[ "secretkey" ]);

    try {
        data->register_exchange(user, exch, pub_key, pvt_key);
    } catch (UserNotFound& e) {
        cerr << "upload_exchange_key: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "upload_exchange_key: " << e.what() << endl;
        return response(401);
    } catch (std::exception& e) {
        //  should we catch exchanges level errors ?? (yes, we should)
        cerr << "upload_exchange_key (unknown error): " << e.what() << endl;
        return response(401);
    }

    resp[ "status" ] = "SUCCESS";
    crow::response res(200, resp);
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
}

response Endpoints::remove_exchange_key(const request& req) {
    AuthenticUser user = parse_user(req);
    crow::json::wvalue resp;
    auto body = crow::json::load(req.body);

    Exchange exch = from_string(string(body[ "exchange" ]));

    try {
        data->delete_exchange(user, exch);
    } catch (UserNotFound& e) {
        cerr << "remove_exchange_key: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "remove_exchange_key: " << e.what() << endl;
        return response(401);
    }

    resp[ "status" ] = "SUCCESS";
    crow::response res(200, resp);
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
}

response Endpoints::get_annotated_trades(const request& req) {
    AuthenticUser user = parse_user(req);

    try {
        const auto user_trades = data->get_trades(user);
        const auto mts = matcher->get_matched_trades(user_trades);

        crow::json::wvalue ret;

        for (int i = 0; i < static_cast<int>(mts.size()); i++) {
            const auto& mt = mts[ i ];

            ret[ i ][ "bought_timestamp" ] = to_string(mt.bought_timestamp);
            ret[ i ][ "sold_timestamp" ] = to_string(mt.sold_timestamp);
            ret[ i ][ "term" ] = to_string(mt.term);
            ret[ i ][ "currency" ] = mt.currency;
            ret[ i ][ "size" ] = to_string(mt.sz);
            ret[ i ][ "pnl" ] = to_string(mt.pnl);
        }

        crow::response res(200, ret);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    } catch (UserNotFound& e) {
        cerr << "get_annotated_trades: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "get_annotated_trades: " << e.what() << endl;
        return response(401);
    } catch (RateLimitedQuery& e) {
        cerr << "get_annotated_trades: " << e.what() << endl;
        return response(503);
    } catch (NoRecordsFound& e) {
        cerr << "get_annotated_trades: " << e.what() << endl;
        crow::json::wvalue resp;
        resp[ "status" ] = "NO RECORDS FOUND";
        crow::response res(200, resp);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    }
}

response Endpoints::get_year_end_stats(const request& req) {
    AuthenticUser user = parse_user(req);

    try {
        const auto user_trades = data->get_trades(user);
        const auto ye_pnl = matcher->get_year_end_pnl(user_trades);

        crow::json::wvalue ye_pnl_crow;

        ye_pnl_crow[ "lt_realized_pnl" ] = ye_pnl.lt_realized;
        ye_pnl_crow[ "st_realized_pnl" ] = ye_pnl.st_realized;
        ye_pnl_crow[ "actual_pnl" ] = ye_pnl.actual;

        crow::response res(200, ye_pnl_crow);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    } catch (UserNotFound& e) {
        cerr << "get_year_end_stats: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "get_year_end_stats: " << e.what() << endl;
        return response(401);
    } catch (RateLimitedQuery& e) {
        cerr << "get_year_end_stats: " << e.what() << endl;
        return response(503);
    } catch (NoRecordsFound& e) {
        cerr << "get_year_end_stats: " << e.what() << endl;
        crow::json::wvalue resp;
        resp[ "status" ] = "NO RECORDS FOUND";
        crow::response res(200, resp);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    }
}

response Endpoints::calc_trade_pnl(const request& req) {
    AuthenticUser user = parse_user(req);
    auto body = crow::json::load(req.body);

    try {
        const Trade trade_in{
            field_to_ts(string(body[ "timestamp" ])),
            string(body[ "sold_currency" ]),
            string(body[ "bought_currency" ]),
            field_to_double(string(body[ "sold_amount" ])),
            field_to_double(string(body[ "bought_amount" ]))
        };

        // maybe this isn't necessary, but rules are rules
        data->check_user(user);
        const auto pnl = matcher->get_pnl_from(trade_in);
        crow::json::wvalue pnl_crow;
        pnl_crow["pnl"] = pnl;

        crow::response res(200, pnl_crow);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
        // return pnl_crow;
    } catch (UserNotFound& e) {
        cerr << "calc_trade_pnl: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "calc_trade_pnl: " << e.what() << endl;
        return response(401);
    } catch (...) {
        cerr << "Invalid timestamp" << endl;
        return response(400);
    }
}

response Endpoints::get_net_pnl(const request& req) {
    AuthenticUser user = parse_user(req);

    try {
        const auto user_trades = data->get_trades(user);
        const auto pnl = matcher->get_net_pnl(user_trades);
        crow::json::wvalue net_pnl_crow;
        net_pnl_crow["pnl"] = pnl;
        crow::response res(200, net_pnl_crow);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
        // return response(net_pnl_crow);
    } catch (UserNotFound& e) {
        cerr << "get_net_pnl: " << e.what() << endl;
        return response(401);
    } catch (InvalidCreds& e) {
        cerr << "get_net_pnl: " << e.what() << endl;
        return response(401);
    }
}
