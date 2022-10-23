#include "../common/helpers.h"
#include "../common/base64.h"
#include "coinbase.h"
#include "kraken.h"

#include <curl/curl.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <string>

API_key krn_api_token = "7b1mCpNdAdGiWkMkDJRtUfyZVdJX8b071miozWYvPci2SH7+qgQWbqjv";
API_key krn_secret_api= "BP0KzHjwhcaS+AuBSKA+TQf1oRYXjmJ5M3U42k/1i3xmJf4VgQY2cQSh+hQEGhExcsx3dKcodEPnaqqdTc/1yw==";
std::string uri_path  = "https://api.kraken.com/0/private/TradesHistory";

extern "C" {
    int Base64encode_len(int);
    int Base64encode(char *, const char *, int);
    int Base64decode_len(const char *);
    int Base64decode(char *, const char *);
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

    std::cout << "Concat nonce and request_data: " << request_nonce + request_data << "\n"; 
    std::vector<unsigned char> hashed_nonce_data = sha256_wrapper(request_nonce + request_data);

    std::cout<< "Hash of the above: " << convert_vec_to_str(hashed_nonce_data) << "\n";
    uri_path += convert_vec_to_str(hashed_nonce_data);

    std::cout<< "URI path + hash: " << uri_path << "\n";

    // HMAC the data and key, get the signature
    std::vector<unsigned char> signature = hmac_sha512_wrapper(uri_path, api_key_secret);

    // Encode the signature in base64 and return it as string
    std::string decoded_data_str = convert_vec_to_str(signature);

    std::cout<< "HMAC'd signature: " << decoded_data_str << "\n";

    const char *decoded_data = decoded_data_str.c_str();
    int encoded_data_length = Base64encode_len(decoded_data_str.length());
    char *encoded_data = (char *) malloc(encoded_data_length);
    if (encoded_data == NULL) {
        throw "Malloc failed";
    }

    Base64encode(encoded_data, decoded_data, decoded_data_str.length());

    std::string encoded_signature (encoded_data);
    free(encoded_data);

    std::cout<< "Encoded HMAC'd signature: " << encoded_signature << "\n";

    return encoded_signature;
}

// Send request for info to Kraken
std::string KrakenDriver::query_for_trades() {
    // Part 1: Generate the required request headers
    ///////////////////////////////////////////////////////////////////////////

    std::cout << "API key: " << this->_ukey << "\n";
    std::cout << "Secret API key: " << this->_uprivatekey << "\n";

    if (this->_ukey == "" || this->_uprivatekey == "") {
        throw "API Creds Missing";
    }

    // Decode secret API key
    char* encoded_api_secret = const_cast<char *> (this->_uprivatekey.c_str());
    int decoded_api_secret_length = Base64decode_len(encoded_api_secret);

    char* decoded_api_secret_c = (char *) malloc(decoded_api_secret_length);
    if (decoded_api_secret_c == NULL) {
        throw "Malloc failed";
    }

    Base64decode(decoded_api_secret_c, encoded_api_secret);
    API_key decoded_api_secret (decoded_api_secret_c);
    free(decoded_api_secret_c);

    std::cout << "Decoded Secret API key: " << decoded_api_secret << "\n";

    std::string request_nonce = generate_nonce();
    std::cout << "Nonce: " << request_nonce << "\n";

    std::string request_data  = generate_payload(request_nonce);
    std::cout << "Request data: " << request_data << "\n";

    std::string request_sig   = generate_signature(uri_path, request_data, request_nonce, decoded_api_secret);
    std::cout << "Request signature " << request_sig << "\n";

    // Part 2: Send the formatted request to api.kraken.com
    ///////////////////////////////////////////////////////////////////////////
    // Using curllib, wrapper for cURL
    CURL *curl;
    CURLcode res;
    std::string response_buffer;
    curl_slist* request_headers = NULL;

    curl = curl_easy_init();
    if (curl) {
        std::cout << "Curl easy init successful" << "\n";

        // Configure the cURL agent
        curl_easy_setopt(curl, CURLOPT_URL, uri_path.c_str());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "CodeBuddies Crypto Portfolio Tracker");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, kraken_write_callback);

        // Create the request headers and add them to the cURL agent
        request_headers = curl_slist_append(request_headers, ("API-Key: " + this->_ukey).c_str());
        request_headers = curl_slist_append(request_headers, ("API-Sign: " + request_sig).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request_headers);

        // Set a place for cURL to write a response to, once one is received
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&response_buffer));

        // Execute the request
        res = curl_easy_perform(curl);
        std::cout << "cURL response: " << res << "\n";
    } else {
        throw "Curl setup failed";
    }

    // Check return value of cURL query
    if (!(res == CURLE_OK)) {
       throw "Curl request failed";
    }

    std::cout << "Response: " << response_buffer << "\n";
 
    // Deallocate resources
    curl_easy_cleanup(curl);
    curl_slist_free_all(request_headers);

    return response_buffer;
}

std::vector<Trade> KrakenDriver::get_trades() {
    std::vector<Trade> processed_txs;
    std::string string_txs = this->query_for_trades();

    // Processing //

    return processed_txs;
}

int main () {
    KrakenDriver kraken_client("client", krn_api_token, krn_secret_api);

    for (auto x : kraken_client.get_trades()) {
        std::cout << "Trade record"      << "\n";
        std::cout << "Currency sold: "   << x.sold_currency   << "\n";
        std::cout << "Currency bought: " << x.bought_currency << "\n";
        std::cout << "Sold amount: "     << x.sold_amount     << "\n";
        std::cout << "Bought amountL "   << x.bought_amount   << "\n";
    }

    return 0;
}
