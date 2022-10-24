#include "../common/helpers.h"
#include "coinbase.h"
#include "kraken.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <curl/curl.h>
#include <iostream>
#include <chrono>
#include <string>
#include <crow.h>

// TODO:
// - Wrap up Coinbase query
// - Process return from cURL into Trades and return them
// - Pricer (KRN/CNB public API for a given pair)
// - Testing

///////////////////////////////////////////////////////////////////////////////
struct HexCharStruct
{
  unsigned char c;
  HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
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
    using namespace boost::posix_time;
    ptime recorded_time = microsec_clock::universal_time();
    return to_iso_extended_string(recorded_time);
}
/*
 *
 */
std::string CoinbaseDriver::generate_url() {
    return ("https://api.coinbase.com/v2/accounts/" + this->_uaccountid + "/transactions");
}

/*
 *
 */
std::string CoinbaseDriver::generate_path() {
    return ("/v2/accounts/" + this->_uaccountid + "/transactions");
}

/*
 *
 */
std::string CoinbaseDriver::generate_signature(std::string timestamp, std::string method, std::string path) {
    // Create 2 vectors: data + key
    std::vector<unsigned char> body (timestamp.begin(), timestamp.end());
    body.insert(body.end(), method.begin(), method.end());
    body.insert(body.end(), path.begin(), path.end());

    std::vector<unsigned char> key (this->_uprivatekey.begin(), this->_uprivatekey.end());
    
    // HMAC the data and key, get the signature
    std::vector<unsigned char> signature = hmac_sha256_wrapper(body, key);
    std::string str_signature = convert_vec_to_str(signature);
    // std::string encoded_signature = crow::utility::base64encode(str_signature, str_signature.size());

    // Test
    // std::vector<unsigned char> vect_encoded_signature (encoded_signature.begin(), encoded_signature.end());
    // std::cout << "[B64ENCODED (SIGNATURE)] ";
    // print_vec_as_hex(vect_encoded_signature);

    return str_signature; 
}

/*
 *
 */
std::string CoinbaseDriver::query_for_trades() {
    // "CB-ACCESS-KEY: " -- api key
    // "CB-ACCESS-SIGN: " -- message signature, [HMAC, SHA256] (body = timestamp + method + requestPath, key = cnb_secret_token)
    // "CB-ACCESS-TIMESTAMP: " -- timestamp for this request, seconds since UNIX epoch in UTC

    if (this->_ukey == "" || this->_uprivatekey == "" || this->_upassword == "") {
        throw "API Creds Missing";
    }

    // Get the user account id
    // std::string account_id = ...



    // Generate a timestamp
    std::string request_timestamp = this->generate_timestamp();
    std::cout << "[TIMESTAMP] " << request_timestamp << std::endl;

    // Generate the URL to query
    std::string cnb_query_url = this->generate_url();
    std::cout << "[CNB QUERY URL] " << cnb_query_url << std::endl;

    // Generate the URI
    std::string request_path = this->generate_path();
    std::cout << "[CNB QUERY URI] " << request_path << std::endl;

    // Generate signature
    std::string request_sig = this->generate_signature(request_timestamp, "GET", request_path);
    std::cout << "[REQUEST SIGNATURE] " << request_sig << std::endl; 

    ///////////////////////////////////////////////////////////////////////////
    // Part 2: Send the formatted request to api.coinbase.com                //
    ///////////////////////////////////////////////////////////////////////////
    // Using curllib, wrapper for cURL
    CURL *curl;
    CURLcode res;
    std::string response_buffer;
    curl_slist* request_headers = NULL;

    // Configure the cURL agent
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, cnb_query_url.c_str());                        // Set path to query
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);                                       // Set verbosity level
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);                                // Verify certificate name against host
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "CodeBuddies Crypto Portfolio Tracker"); // Specify user agent
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, kraken_write_callback);              // Specify the write callback function

        // Prepare the headers
        std::string api_key_header = "CB-ACCESS-KEY: " + this->_ukey;
        std::string api_secret_header = "CB-ACCESS-SIGN: " + request_sig;
        std::string api_timestamp_header = "CB-ACCESS-TIMESTAMP: " + request_timestamp;

        // Create the request headers and add them to the cURL agent
        request_headers = curl_slist_append(request_headers, api_key_header.c_str());      // Add API key header
        request_headers = curl_slist_append(request_headers, api_secret_header.c_str());   // Add signature header
        request_headers = curl_slist_append(request_headers, api_timestamp_header.c_str());// Add timestamp header
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request_headers);                       // Add the headers to curl object

        // Set a place for cURL to write a response to, once one is received
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&response_buffer));  // Buffer to which the write callback will write

        // Execute the request
        res = curl_easy_perform(curl);
        std::cout << "[cURL RESPONSE] " << res << std::endl;
    } else {
        throw "Curl setup failed";
    }

    // Check return value of cURL query
    if (!(res == CURLE_OK)) {
       throw "Curl request failed";
    }
 
    // Deallocate resources
    curl_slist_free_all(request_headers);
    curl_easy_cleanup(curl);

    return response_buffer;
}

/*
 *
 */
std::vector<Trade> CoinbaseDriver::get_trades() {
    std::vector<Trade> processed_txs;
    std::string string_txs = this->query_for_trades();
    std::cout << "[COINBASE QUERIED TRADES] " << string_txs << std::endl;

    //////////////////////////////
    // Process the transactions //
    // libjson                  //
    //////////////////////////////

    return processed_txs;
}

///////////////////////////////////////////////////////////////////////////////
// Kraken API                                                                //
///////////////////////////////////////////////////////////////////////////////
/*
 * Generate a nonce
 */
std::string KrakenDriver::generate_nonce() {
    using namespace std::chrono;
    milliseconds time_in_ms = duration_cast <milliseconds>(system_clock::now().time_since_epoch());
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
std::string KrakenDriver::generate_signature(std::string uri_path, std::string request_data, std::string request_nonce, std::vector<unsigned char> api_key_secret) {
    ///////////////////////////////////////////////////////////////////////////
    // Formula: HMAC using SHA-512 of (URI path + SHA-256 of (nonce +        // 
    //                      POST data)) and b64decoded secret API key        //
    ///////////////////////////////////////////////////////////////////////////
    // Hash the nonce and the request data, create data for HMAC
    std::vector<unsigned char> hashed_nonce_data = sha256_wrapper(request_nonce + request_data);
    // std::cout << "[NONCE + REQUEST_DATA] " << request_nonce + request_data << std::endl; 
    // std::cout << "[SHA256 (NONCE+DATA)] ";
    // print_vec_as_hex(hashed_nonce_data);

    // Concatenate the hash with URI Path
    hashed_nonce_data.insert(hashed_nonce_data.begin(), uri_path.begin(), uri_path.end());

    // HMAC the data and key, get the signature
    std::vector<unsigned char> signature = hmac_sha512_wrapper(hashed_nonce_data, api_key_secret);
    std::string str_signature = convert_vec_to_str(signature);
    std::string encoded_signature = crow::utility::base64encode(str_signature, str_signature.size());

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
std::string KrakenDriver::query_for_trades() {
    ///////////////////////////////////////////////////////////////////////////
    // Part 1: Generate the required request headers                         //
    ///////////////////////////////////////////////////////////////////////////
    if (this->_ukey == "" || this->_uprivatekey == "") {
        throw "API Creds Missing";
    }

    // Decode secret api key
    std::string decoded_api_secret = crow::utility::base64decode(this->_uprivatekey, this->_uprivatekey.size());
    std::vector<unsigned char> vec_decoded_api_secret (decoded_api_secret.begin(), decoded_api_secret.end()); 
   
    // Generate nonce
    std::string request_nonce = this->generate_nonce();

    // Generate payload
    std::string request_data  = this->generate_payload(request_nonce);

    // Generate the URI path
    std::string krn_uri_path = this->generate_path();

    // Generate signature
    std::string request_sig = this->generate_signature(krn_uri_path, request_data, request_nonce, vec_decoded_api_secret);

    // Generate the url to query
    std::string krn_url = this->generate_url();

    ///////////////////////////////////////////////////////////////////////////
    // Part 2: Send the formatted request to api.kraken.com                  //
    ///////////////////////////////////////////////////////////////////////////
    // Using curllib, wrapper for cURL
    CURL *curl;
    CURLcode res;
    std::string response_buffer;
    curl_slist* request_headers = NULL;

    // Configure the cURL agent
    curl = curl_easy_init();
    if (curl) {

        curl_easy_setopt(curl, CURLOPT_URL, krn_url.c_str());                              // Set path to query
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);                                // Verify certificate name against host
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "CodeBuddies Crypto Portfolio Tracker"); // Specify user agent
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data.c_str());                  // Specify what we want from the service
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, kraken_write_callback);              // Specify the write callback function

        // Prepare the headers
        std::string api_key_header = "API-Key: " + this->_ukey;
        std::string api_secret_header = "API-Sign: " + request_sig;

        // Create the request headers and add them to the cURL agent
        request_headers = curl_slist_append(request_headers, api_key_header.c_str());      // Add API key header
        request_headers = curl_slist_append(request_headers, api_secret_header.c_str());   // Add signature header
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request_headers);                       // Add the headers to curl object

        // Set a place for cURL to write a response to, once one is received
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&response_buffer));  // Buffer to which the write callback will write

        // Execute the request
        res = curl_easy_perform(curl);
    } else {
        throw "Curl setup failed";
    }

    // Check return value of cURL query
    if (!(res == CURLE_OK)) {
       throw "Curl request failed";
    }
 
    // Deallocate resources
    curl_slist_free_all(request_headers);
    curl_easy_cleanup(curl);

    return response_buffer;
}

std::vector<Trade> KrakenDriver::get_trades() {
    std::vector<Trade> processed_txs;
    std::string string_txs = this->query_for_trades();
    std::cout << "[KRAKEN QUERIED TRADES] " << string_txs << std::endl;

    //////////////////////////////
    // Process the transactions //
    crow::json::wvalue wval (string_txs);
    // cout

    return processed_txs;
}

int main () {

    // Kraken
    API_key krn_api_token = "J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU";
    API_key krn_secret_api= "VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg==";
    
    KrakenDriver kraken_client("client", krn_api_token, krn_secret_api);
    for (auto x : kraken_client.get_trades()) {
        std::cout << "Trade record"      << "\n";
        std::cout << "Currency sold: "   << x.sold_currency   << "\n";
        std::cout << "Currency bought: " << x.bought_currency << "\n";
        std::cout << "Sold amount: "     << x.sold_amount     << "\n";
        std::cout << "Bought amountL "   << x.bought_amount   << "\n";
    }

    // Coinbase
    // API_key cnb_api_token = "VViR0nG9v7KoX6gL";
    // API_key cnb_secret_token = "Xy9h8UrlqtDQyYL9El7LrC4xnAxTQzCy";
    // std::string cnb_account_id = "atk315@nyu.edu";
    
    API_key cnb_api_token = "c0a4e40189be2a9439ced5306ca4f64b";
    API_key cnb_secret_token = "oMYpyEEEKRF7A24/ev4tciy3TIybludyEbQIj6ls3naEi79aphzvOqvRtDqf+i/TeLzLLjWlkATnjInbPwQU3g==";
    std::string password = "pass";
/*
    CoinbaseDriver coinbase_client("client", cnb_api_token, cnb_secret_token, password, "temp");
    for (auto x : coinbase_client.get_trades()) {
        std::cout << "Trade record"      << "\n";
        std::cout << "Currency sold: "   << x.sold_currency   << "\n";
        std::cout << "Currency bought: " << x.bought_currency << "\n";
        std::cout << "Sold amount: "     << x.sold_amount     << "\n";
        std::cout << "Bought amountL "   << x.bought_amount   << "\n";
    }
*/
    return 0;
}
