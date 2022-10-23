#include "../common/crypto_helpers.h"
#include "../common/base64.h"
#include "coinbase.h"
#include "kraken.h"

#include <cstdlib>
#include <chrono>
#include <iostream>
#include <string>

API_key krn_api_token = "7b1mCpNdAdGiWkMkDJRtUfyZVdJX8b071miozWYvPci2SH7+qgQWbqjv";
API_key krn_secret_api= "BP0KzHjwhcaS+AuBSKA+TQf1oRYXjmJ5M3U42k/1i3xmJf4VgQY2cQSh+hQEGhExcsx3dKcodEPnaqqdTc/1yw==";
std::string uri_path  = "http://api.kraken.com/0/private/TradesHistory";

extern "C" {
    int Base64encode_len(int);
    int Base64encode(char *, const char *, int);
    int Base64decode_len(const char *);
    int Base64decode(char *, const char *);
}

std::pair<API_key, API_key> KrakenDriver::get_api_key(User user) {
    // Query the database for a key, otherwise return ""
    return make_pair(krn_api_token, krn_secret_api);
}

// Generate a nonce
std::string KrakenDriver::generate_nonce() {
    using namespace std::chrono;
    milliseconds time_in_ms = duration_cast <milliseconds>(system_clock::now().time_since_epoch());
    return std::to_string(time_in_ms.count());
}

// Generate payload to send for query
std::string KrakenDriver::generate_payload(std::string nonce) {
    return ("nonce=" + nonce + "&trades=true");
}

// Generate a signature
std::string KrakenDriver::generate_signature(std::string uri_path, std::string request_data, std::string request_nonce, API_key api_key_secret) {
    // Formula: HMAC using SHA-512 of (URI path + SHA-256 of (nonce + POST data)) and secret API key
    // Hash the nonce and the request data, create data for HMAC
    std::vector<unsigned char> hashed_nonce_data = sha256_wrapper(request_nonce + request_data);
    uri_path += convert_vec_to_str(hashed_nonce_data);

    // HMAC the data and key, get the signature
    std::vector<unsigned char> signature = hmac_sha512_wrapper(uri_path, api_key_secret);

    // Encode the signature in base64 and return it as string
    std::string decoded_data_str = convert_vec_to_str(signature);
    
    const char *decoded_data = decoded_data_str.c_str();
    int encoded_data_length = Base64encode_len(decoded_data_str.length());
    char *encoded_data = (char *) malloc(encoded_data_length);
    if (encoded_data == NULL) {
        throw "Malloc failed";
    }

    Base64encode(encoded_data, decoded_data, decoded_data_str.length());
    
    std::string encoded_signature (encoded_data);
    free(encoded_data);
    
    return encoded_signature;
}

// Send request for info to Kraken
void KrakenDriver::query_for_trades(User user) {
    // .first == api_key, .second == (b64 encoded) api_private_key
    std::pair<API_key, API_key> api_creds = get_api_key(user);

    if (api_creds.first == "" || api_creds.second == "") {
        throw "API Creds Missing";
    }

    // Decode secret API key
    char* encoded_api_secret = const_cast<char *> (api_creds.second.c_str());
    int decoded_api_secret_length = Base64decode_len(encoded_api_secret);

    char* decoded_api_secret_c = (char *) malloc(decoded_api_secret_length);
    if (decoded_api_secret_c == NULL) {
        throw "Malloc failed";
    }

    Base64decode(decoded_api_secret_c, encoded_api_secret);
    API_key decoded_api_secret (decoded_api_secret_c);
    free(decoded_api_secret_c);

    std::string request_nonce = generate_nonce();
    std::string request_data  = generate_payload(request_nonce);
    std::string request_sig   = generate_signature(uri_path, request_data, request_nonce, decoded_api_secret); 

    std::cout << request_sig + "\n"; 
}

int main () {
    return 0;
}
