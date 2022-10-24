#include <iostream>
#include <unistd.h>
#include <openssl/sha.h>
#include "crow.h"


#define CLIENTIDLEN 4
#define APIKEYLEN 4

class auth{
public:
	std::string gen_random(const int len);
	
	// get hash of string
	std::string get_hash(std::string key);
	
	// get key - apikey or refresh token from request
	std::string get_key(const crow::request& req);
	
	// get client id from request
	std::string get_client_id(const crow::request& req);
	
	// validate credentials of client : apikey
	bool validate_credentials(const crow::request& req);
	
	// validate the refresh token of the client
	bool validate_refresh_token(const crow::request& req);
	
	// generate credentials for first time client
	crow::json::wvalue generate_credentials(void);
	
	// generate new credentials for already existing client
	crow::json::wvalue refresh_credentials(const crow::request& req);

	// convert const crow::json::rvalue to a std::string
	std::string convert_to_string(const crow::json::rvalue jrvalue);

	// parse const crow::request& and convert into struct trade
	// For now return pointer is void
	// actual definition will be struct trade parse_trade_from_json(auto x)
	void parse_trade_from_json(const crow::request& req);
};
