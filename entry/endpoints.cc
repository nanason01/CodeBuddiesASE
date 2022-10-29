//
// Implementation of API entpoints
//
// the methods do what they say they do
//

#include "endpoints.h"

constexpr int CLIENTIDLEN = 4;
constexpr int APIKEYLEN = 4;

using crow::response;
using crow::request;
using std::string;
using std::cerr;
using std::endl;

std::unique_ptr<BaseData> data;
std::unique_ptr<BaseMatcher> matcher;

void set_mode_prod() {
    data = std::make_unique<Data>();
    matcher = std::make_unique<Matcher>();
}

void set_mode_mock(MockData& data, MockMatcher& matcher) {
    ::data.reset(&data);
    ::matcher.reset(&matcher);
}

static string gen_random_str(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string ret(sizeof(len), 'a');

    for (int i = 0; i < len; ++i)
        ret[i] = alphanum[rand() % sizeof(alphanum)];

    return ret;
}

static string hash_str(string key) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, key.c_str(), key.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}

static AuthenticUser parse_user(const request& req) {
    //std::cout << req.get_header_value("Authorization")
    //           .substr(7).substr(CLIENTIDLEN, APIKEYLEN) << endl;
    return AuthenticUser{
           req.get_header_value("Authorization")
               .substr(7).substr(0, CLIENTIDLEN),
           hash_str(req.get_header_value("Authorization")
               .substr(7).substr(CLIENTIDLEN, APIKEYLEN))
    };

    
}

response Endpoints::validate_credentials(const request& req) {
    const auto user = parse_user(req);
    std::cout << user.user << std::endl;
    std::cout << user.creds << std::endl;

    try {
        data->check_user(user);
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }

    return response(200);
}

response Endpoints::generate_credentials(const request& req) {
    crow::json::wvalue ret_val;
    static int created = 0;
    /*if(created == 0){
        // table not yet created. Create table
        
        try {
            data->create_table();
        } catch (UserNotFound* e) {
            cerr << "validate_credentials: " << e->what() << endl;
            return response(401);
        } catch (InvalidCreds* e) {
            cerr << "validate_credentials: " << e->what() << endl;
            return response(401);
        }
        created++;
    }*/
    std::cout << "in generate_credentials" << std::endl;
    std::cout << data << std::endl;

    string client_id = gen_random_str(CLIENTIDLEN);
    string api_key = gen_random_str(APIKEYLEN);
    string refresh_key = gen_random_str(APIKEYLEN);

    AuthenticUser newuser{
        client_id,
        hash_str(client_id + api_key),
        hash_str(client_id + refresh_key)
    };
    std::cout << "in generate_credentials2" << std::endl;
    ret_val["client_id"] = client_id;
    ret_val["api_key"] = client_id + api_key;
    ret_val["refresh_token"] = client_id + refresh_key;
    std::cout << "in generate_credentials3" << std::endl;
    // TODO : test if this works
    try {
        data->add_user(newuser);
        std::cout << "in generate_credentials4" << std::endl;
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }
    std::cout << "in generate_credentials5" << std::endl;



    return ret_val;
}

response Endpoints::refresh_credentials(const request& req) {
    crow::json::wvalue resp;

    const auto user = parse_user(req);

    // TODO : implement check_user_refresh_key()
    
    try {
        data->check_refr(user.user, user.creds);
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }

    string client_id = user.user;
    string api_key = gen_random_str(APIKEYLEN);
    string refresh_key = gen_random_str(APIKEYLEN);

    resp["client_id"] = client_id;
    resp["api_key"] = client_id + api_key;
    resp["refresh_token"] = client_id + refresh_key;

    AuthenticUser newcreds{
        client_id,
        hash_str(client_id + api_key),
        hash_str(client_id + refresh_key)
    };


    // TODO : test if this works
    try {
        data->update_user_creds(user);
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }

    return resp;
}

static Timestamp field_to_ts(string ts_str) {
    return from_usa_date(4, 20, 2021);
}

static double field_to_double(string amount_str) {
    return -1.0;
}

static int field_to_int(string exch_str) {
    return -1;
}

response Endpoints::upload_trade(const request& req) {
    const AuthenticUser user = parse_user(req);

    auto body = crow::json::load(req.body);

    const Trade trade_in{
        field_to_ts(string(body["timestamp"])),
        string(body["sold_currency"]),
        string(body["bought_currency"]),
        field_to_double(string(body["sold_amount"])),
        field_to_double(string(body["bought_amount"]))
    };

    try {
        data->upload_trade(user, trade_in);
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }

    return response(200);
}

response Endpoints::upload_exchange_key(const request& req) {
    AuthenticUser user = parse_user(req);

    auto body = crow::json::load(req.body);

    Exchange exch = from_string(string(body["exchange"]));
    // the names in the json are consistent with legacy
    API_key pub_key = string(body["readkey"]);
    API_key pvt_key = string(body["secretkey"]);

    try {
        data->register_exchange(user, exch, pub_key, pvt_key);
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } // should we catch exchanges level errors ??

    return response(200);
}

response Endpoints::remove_exchange_key(const request& req) {
    AuthenticUser user = parse_user(req);

    auto body = crow::json::load(req.body);

    Exchange exch = from_string(string(body["exchange"]));
    // the to-be-deleted fields are unnecessary, readme updated.

    try {
        data->delete_exchange(user, exch);
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }

    return response(200);
}

response Endpoints::get_annotated_trades(const request& req) {
    AuthenticUser user = parse_user(req);

    try {
        const auto user_trades = data->get_trades(user);
        const auto mts = matcher->get_matched_trades(user_trades);

        //vector<crow::json::wvalue> ret;

        crow::json::wvalue ret;
        int i = 0;

        for (const MatchedTrade& mt : mts) {
            //crow::json::wvalue wv;

            ret[i]["bought_timestamp"] = to_string(mt.bought_timestamp);
            ret[i]["sold_timestamp"] = to_string(mt.sold_timestamp);
            ret[i]["term"] = to_string(mt.term);
            ret[i]["currency"] = mt.currency;
            ret[i]["size"] = to_string(mt.sz);
            ret[i]["pnl"] = to_string(mt.pnl);

            i = i + 1;

            //ret.emplace_back(std::move(wv));
        }

        return crow::response(ret);
        // @TODO: how to return a list of values as a crow json
        // return response(std::move(ret));
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }
}

response Endpoints::get_year_end_stats(const request& req) {
    AuthenticUser user = parse_user(req);

    try {
        const auto user_trades = data->get_trades(user);
        const auto ye_pnl = matcher->get_year_end_pnl(user_trades);

        crow::json::wvalue ye_pnl_crow;

        ye_pnl_crow["lt_realized_pnl"] = ye_pnl.lt_realized;
        ye_pnl_crow["st_realized_pnl"] = ye_pnl.st_realized;
        ye_pnl_crow["actual_pnl"] = ye_pnl.actual;

        return ye_pnl_crow;
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }
}

response Endpoints::calc_trade_pnl(const request& req) {
    AuthenticUser user = parse_user(req);
    auto body = crow::json::load(req.body);

    const Trade trade_in{
        field_to_ts(string(body["timestamp"])),
        string(body["sold_currency"]),
        string(body["bought_currency"]),
        field_to_double(string(body["sold_amount"])),
        field_to_double(string(body["bought_amount"]))
    };

    try {
        // maybe this isn't necessary, but rules are rules
        data->check_user(user);
        const auto pnl = matcher->get_pnl_from(trade_in);

        return response(pnl);
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }
}

response Endpoints::get_net_pnl(const request& req) {
    AuthenticUser user = parse_user(req);

    try {
        // maybe this isn't necessary, but rules are rules
        const auto user_trades = data->get_trades(user);
        const auto pnl = matcher->get_net_pnl(user_trades);

        return response(pnl);
    } catch (UserNotFound* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    } catch (InvalidCreds* e) {
        cerr << "validate_credentials: " << e->what() << endl;
        return response(401);
    }

}
