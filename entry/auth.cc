#include "auth.h"

std::string auth::gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}



std::string auth::get_hash(std::string key){
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, key.c_str(), key.size());
	SHA256_Final(hash, &sha256);
	
	std::stringstream ss;
	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}
	return ss.str();
}



std::string auth::get_key(const crow::request& req){
	std::string auth_header = req.get_header_value("Authorization");
	std::string api_key_whole = auth_header.substr(7);
	//std::string client_id = api_key_whole.substr(0, CLIENTIDLEN);
	std::string api_key = api_key_whole.substr(CLIENTIDLEN, APIKEYLEN);
	std::cout << api_key << std::endl;
	return api_key;
}



std::string auth::get_client_id(const crow::request& req){
	std::string auth_header = req.get_header_value("Authorization");
	std::string api_key_whole = auth_header.substr(7);
	std::string client_id = api_key_whole.substr(0, CLIENTIDLEN);
	std::cout << client_id << std::endl;
	return client_id;
}



bool auth::validate_credentials(const crow::request& req){
	
	std::string client_id = get_client_id(req);
	std::string api_key = get_key(req);
	
	// get hash of apikey
        std::string api_key_hash = get_hash(api_key);
	
	// TODO: Validate the credentials from db
	
	return true;
}

bool auth::validate_refresh_token(const crow::request& req){
	
	std::string client_id = get_client_id(req);
	std::string refresh_key = get_key(req);
	
	// get hash of refresh key
	std::string r_key_hash = get_hash(refresh_key);
	
	// TODO : Validate the refresh token from db
	
	
	return true;
}

crow::json::wvalue auth::generate_credentials(void){
	srand((unsigned)time(NULL) * getpid());     
        // std::cout << gen_random(12) << "\n";
        std::string client_id = gen_random(CLIENTIDLEN);
        std::string api_key = gen_random(APIKEYLEN);
        std::string refresh_key = gen_random(APIKEYLEN);
        
        std::string api_key_whole = client_id + api_key;
        std::string refresh_token = client_id + refresh_key;
        
        // get hash of apikey and refresh key
        std::string api_key_hash = get_hash(api_key);
        std::string r_key_hash = get_hash(refresh_key);
        

        /*
         * TODO
         * 1) Store client_id, api_key_hash and r_token_hash in Database
        */
        
        crow::json::wvalue credentials;
        credentials["client_id"] = client_id;
        credentials["api_key"] = api_key_whole;
        credentials["refresh_token"] = refresh_token;
        //return  client_id + " " + api_key_whole;
        std::cout << client_id << std::endl;
        std::cout << api_key_whole << std::endl;
        std::cout << refresh_token << std::endl;
        return credentials;
}

crow::json::wvalue auth::refresh_credentials(const crow::request& req){
	crow::json::wvalue credentials;
	std::string client_id = get_client_id(req);
	std::string api_key = gen_random(APIKEYLEN);
	std::string refresh_key = gen_random(APIKEYLEN);
	
	std::string api_key_whole = client_id + api_key;
	std::string refresh_token = client_id + refresh_key;
	
	// get hash of apikey and refresh token
	std::string api_key_hash = get_hash(api_key);
	std::string r_key_hash = get_hash(refresh_key);
	

	/*
	 * TODO
	 * 1) Update api_key_hash and r_token_hash in Database for client_id
*/
	
	credentials["client_id"] = client_id;
	credentials["api_key"] = api_key_whole;
	credentials["refresh_token"] = refresh_token;
	return credentials;
}

