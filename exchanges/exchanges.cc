#include "../common/crypto_helpers.h"
#include "coinbase.h"
#include "kraken.h"

#include <chrono>

API_key krn_api_token = "7b1mCpNdAdGiWkMkDJRtUfyZVdJX8b071miozWYvPci2SH7+qgQWbqjv";
std::string uri_path  = "http://api.kraken.com/0/private/TradesHistory";
std::string post_data = "";

API_key KrakenDriver::get_api_key(User user) {
    // Query the database for a key, otherwise return ""
    return krn_api_token;
}

// Send request for info to Kraken
void KrakenDriver::query_for_trades(User user) {
    API_key api_key = get_api_key(user);
    if (api_key == "") {
        throw "API Key Missing";
    }

    std::string request_nonce = generate_nonce();
    std::string request_sig   = generate_signature(uri_path, post_data, request_nonce, api_key);
}

// Generate a nonce
std::string generate_nonce() {
    using namespace std::chrono;
    milliseconds time_in_ms = duration_cast <milliseconds>(system_clock::now().time_since_epoch()).count();
    return std::to_string(time_in_ms);
}

// Generate a signature
std::string generate_signature(std::string uri_path, std::string post_data, std::string request_nonce, API_key api_key) {
    // Formula: HMAC using SHA-512 of (URI path + SHA-256 of (nonce + POST data)) and API key
     
}
