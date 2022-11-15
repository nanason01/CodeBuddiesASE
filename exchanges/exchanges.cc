// Copyright 2022 CodeBuddies ASE Group

#include <curl/curl.h>
#include <crow.h>

#include <iostream>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "exchanges/coinbase.h"
#include "exchanges/helpers.h"
#include "exchanges/kraken.h"

///////////////////////////////////////////////////////////////////////////////
// Debugging functions for printing std::vector<unsigined char> as hex to
// stdout.
// From StackOverflow, link in exchanges/README.md
struct HexCharStruct {
    unsigned char c;
    explicit HexCharStruct(unsigned char _c) : c(_c) {}
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs) {
    return (o << std::hex << static_cast<int> (hs.c));
}

inline HexCharStruct hex(unsigned char _c) {
    return HexCharStruct(_c);
}

void print_vec_as_hex(std::vector<unsigned char> temp) {
    std::cout << "Printing vec of size " << temp.size() << ": [ ";
    for (auto x : temp) {
        std::cout << hex(x) << " ";
    }

    std::cout << "]" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// Coinbase API                                                              //
///////////////////////////////////////////////////////////////////////////////
/*
 *
 */
std::string CoinbaseDriver::generate_timestamp() {
    boost::posix_time::ptime recorded_time = boost::posix_time::
        microsec_clock::universal_time();
    return to_iso_extended_string(recorded_time);
}

/*
 *
 */
std::string CoinbaseDriver::generate_url(std::string account_id) {
    return ("https://api.coinbase.com/v2/accounts/" + account_id +
        "/transactions");
}

/*
 *
 */
std::string CoinbaseDriver::generate_path(std::string account_id) {
    return ("/v2/accounts/" + account_id + "/transactions");
}

/*
 *
 */
std::string CoinbaseDriver::generate_signature(std::string timestamp,
    std::string method, std::string path, API_key private_key) {
    std::vector<unsigned char> body(timestamp.begin(), timestamp.end());
    body.insert(body.end(), method.begin(), method.end());
    body.insert(body.end(), path.begin(), path.end());

    std::vector<unsigned char> key(private_key.begin(), private_key.end());

    // HMAC the data and key, get the signature
    std::vector<unsigned char> signature = hmac_sha256_wrapper(body, key);
    std::string str_signature = convert_vec_to_str(signature);
    // std::string encoded_signature = crow::utility::
    //                base64encode(str_signature, str_signature.size());

    // Test
    // std::vector<unsigned char> vect_encoded_signature
    //            (encoded_signature.begin(), encoded_signature.end());
    // std::cout << "[B64ENCODED (SIGNATURE)] ";
    // print_vec_as_hex(vect_encoded_signature);

    return str_signature;
}

/*
 *
 */
std::string CoinbaseDriver::query_for_trades(API_key public_key,
    API_key private_key) {
    // "CB-ACCESS-KEY: " -- api key
    // "CB-ACCESS-SIGN: " -- message signature, [HMAC, SHA256]
    //        (body = timestamp + method + requestPath, key = cnb_secret_token)
    // "CB-ACCESS-TIMESTAMP: " -- timestamp for this request, seconds
    //                                          since UNIX epoch in UTC

    if (public_key == "" || private_key == "") {
        throw InvalidAPIKey{};
    }

    // Get the user account id
    std::string account_id;

    // Generate a timestamp
    std::string request_timestamp = this->generate_timestamp();
    std::cout << "[TIMESTAMP] " << request_timestamp << std::endl;

    // Generate the URL to query
    std::string cnb_query_url = this->generate_url(account_id);
    std::cout << "[CNB QUERY URL] " << cnb_query_url << std::endl;

    // Generate the URI
    std::string request_path = this->generate_path(account_id);
    std::cout << "[CNB QUERY URI] " << request_path << std::endl;

    // Generate signature
    std::string request_sig = this->generate_signature(request_timestamp,
        "GET", request_path, private_key);
    std::cout << "[REQUEST SIGNATURE] " << request_sig << std::endl;

    ///////////////////////////////////////////////////////////////////////////
    // Part 2: Send the formatted request to api.coinbase.com                //
    ///////////////////////////////////////////////////////////////////////////
    // Using curllib, wrapper for cURL
    CURL* curl;
    CURLcode res;
    std::string response_buffer;
    curl_slist* request_headers = NULL;

    // Configure the cURL agent
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, cnb_query_url.c_str());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT,
            "CodeBuddies Crypto Portfolio Tracker");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, kraken_write_callback);

        // Prepare the headers
        std::string api_key_header = "CB-ACCESS-KEY: " + public_key;
        std::string api_secret_header = "CB-ACCESS-SIGN: " + request_sig;
        std::string api_timestamp_header = "CB-ACCESS-TIMESTAMP: " +
            request_timestamp;

        // Create the request headers and add them to the cURL agent
        request_headers = curl_slist_append(request_headers,
            api_key_header.c_str());
        request_headers = curl_slist_append(request_headers,
            api_secret_header.c_str());
        request_headers = curl_slist_append(request_headers,
            api_timestamp_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request_headers);

        // Set a place for cURL to write a response to, once one is received
        curl_easy_setopt(curl, CURLOPT_WRITEDATA,
            static_cast<void*>(&response_buffer));

        // Execute the request
        res = curl_easy_perform(curl);
        std::cout << "[cURL RESPONSE] " << res << std::endl;
    } else {
        throw CurlSetupFailed{};
    }

    // Check return value of cURL query
    if (!(res == CURLE_OK)) {
        throw CurlRequestFailed{};
    }

    // Deallocate resources
    curl_slist_free_all(request_headers);
    curl_easy_cleanup(curl);

    return response_buffer;
}

/*
 *
 */
std::vector<Trade> CoinbaseDriver::get_trades(API_key public_key,
    API_key private_key) {
    std::vector<Trade> processed_txs;
    std::string string_txs = this->query_for_trades(public_key, private_key);
    std::cout << "[COINBASE QUERIED TRADES] " << string_txs << std::endl;

    //////////////////////////////
    // Process the transactions //
    //////////////////////////////

    return processed_txs;
}

///////////////////////////////////////////////////////////////////////////////
// Kraken API
/*
 * Generate a nonce
 */
std::string KrakenDriver::generate_nonce() {
    std::chrono::milliseconds time_in_ms =
        std::chrono::duration_cast <std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch());
    return std::to_string(time_in_ms.count());
}

/*
 * Generate payload to send for query
 */
std::string KrakenDriver::generate_payload(std::string nonce) {
    return ("nonce=" + nonce + "&trades=true");
}

/*
 *  Generate a signature
 */
std::string KrakenDriver::generate_signature(std::string uri_path,
    std::string request_data,
    std::string request_nonce,
    std::vector<unsigned char> api_key_secret) {
    ///////////////////////////////////////////////////////////////////////////
    // Formula: HMAC using SHA-512 of (URI path + SHA-256 of (nonce +
    //                      POST data)) and b64decoded secret API key

    // Hash the nonce and the request data, create data for HMAC
    std::vector<unsigned char> hashed_nonce_data =
        sha256_wrapper(request_nonce + request_data);

    // Concatenate the hash with URI Path
    hashed_nonce_data.insert(hashed_nonce_data.begin(),
        uri_path.begin(), uri_path.end());

    // HMAC the data and key, get the signature
    std::vector<unsigned char> signature =
        hmac_sha512_wrapper(hashed_nonce_data, api_key_secret);

    std::string str_signature = convert_vec_to_str(signature);
    std::string encoded_signature =
        crow::utility::base64encode(str_signature, str_signature.size());

    return encoded_signature;
}

/*
 *
 */
std::string KrakenDriver::generate_url() {
    return "https://api.kraken.com/0/private/TradesHistory";
}

/*
 *
 */
std::string KrakenDriver::generate_path() {
    return "/0/private/TradesHistory";
}

/*
 *  Send request for info to Kraken
 */
std::string KrakenDriver::query_for_trades(API_key public_key,
    API_key private_key) {
    ///////////////////////////////////////////////////////////////////////////
    // Part 1: Generate the required request headers                         //
    ///////////////////////////////////////////////////////////////////////////
    if (public_key == "" || private_key == "") {
        throw APICredsMissing{};
    }

    // Decode secret api key
    std::string decoded_api_secret =
        crow::utility::base64decode(private_key, private_key.size());

    std::vector<unsigned char> vec_decoded_api_secret
    (decoded_api_secret.begin(), decoded_api_secret.end());

    // Generate nonce
    std::string request_nonce = this->generate_nonce();

    // Generate payload
    std::string request_data = this->generate_payload(request_nonce);

    // Generate the URI path
    std::string krn_uri_path = this->generate_path();

    // Generate signature
    std::string request_sig = this->generate_signature(krn_uri_path,
        request_data,
        request_nonce,
        vec_decoded_api_secret);

    // Generate the url to query
    std::string krn_url = this->generate_url();

    ///////////////////////////////////////////////////////////////////////////
    // Part 2: Send the formatted request to api.kraken.com                  //
    ///////////////////////////////////////////////////////////////////////////
    // Using curllib, wrapper for cURL
    CURL* curl;
    CURLcode res;
    std::string response_buffer;
    curl_slist* request_headers = NULL;

    // Configure the cURL agent
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, krn_url.c_str());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT,
            "CodeBuddies Crypto Portfolio Tracker");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, kraken_write_callback);

        // Prepare the headers
        std::string api_key_header = "API-Key: " + public_key;
        std::string api_secret_header = "API-Sign: " + request_sig;

        // Create the request headers and add them to the cURL agent
        request_headers = curl_slist_append(request_headers,
            api_key_header.c_str());
        request_headers = curl_slist_append(request_headers,
            api_secret_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request_headers);

        // Set a place for cURL to write a response to, once one is received
        curl_easy_setopt(curl, CURLOPT_WRITEDATA,
            static_cast<void*>(&response_buffer));

        // Execute the request
        res = curl_easy_perform(curl);
    } else {
        throw CurlSetupFailed{};
    }

    // Check return value of cURL query
    if (!(res == CURLE_OK)) {
        throw CurlRequestFailed{};
    }

    // Deallocate resources
    curl_slist_free_all(request_headers);
    curl_easy_cleanup(curl);

    return response_buffer;
}

std::vector<Trade> KrakenDriver::get_trades(API_key public_key,
    API_key private_key) {
    std::vector<Trade> processed_txs;
    std::string string_txs = this->query_for_trades(public_key, private_key);

    auto jsonified_txs = crow::json::load(string_txs);
    if (!jsonified_txs[ "result" ] ||
        !jsonified_txs[ "result" ][ "count" ] ||
        !jsonified_txs[ "result" ][ "trades" ]) {
        return processed_txs;
    }

    auto tx_record = jsonified_txs[ "result" ][ "trades" ];
    for (auto tx : tx_record) {
        // Now we're looking at individual txs
        Trade single_trade = {
            .timestamp = now(),

            // Currency pair
            .sold_currency = convert_to_string(tx[ "pair" ]),

            // Type of transaction, buy/sell
           .bought_currency = convert_to_string(tx[ "type" ]),
           .sold_amount = tx[ "vol" ].d(),
           .bought_amount = tx[ "cost" ].d()
        };

        processed_txs.push_back(single_trade);
    }

    return processed_txs;
}

/*
int main () {
    // Kraken
    KrakenDriver kraken_client;
    std::vector<Trade> trade_record = kraken_client.get_trades("J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU", "VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg==");

    for (auto x : trade_record) {
        std::cout << "Trade record"      << "\n";
        std::cout << "Currency sold: "   << x.sold_currency   << "\n";
        std::cout << "Currency bought: " << x.bought_currency << "\n";
        std::cout << "Sold amount: "     << x.sold_amount     << "\n";
        std::cout << "Bought amountL "   << x.bought_amount   << "\n";
    }

    return 0;
}
*/
