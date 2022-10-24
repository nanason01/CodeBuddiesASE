#include "../common/helpers.h"
#include "coinbase.h"
#include "kraken.h"

#include <curl/curl.h>
#include <iostream>
#include <chrono>
#include <string>
#include <crow.h>

API_key krn_api_token = "J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU";
API_key krn_secret_api= "VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg==";
std::string krn_url  = "https://api.kraken.com/0/private/TradesHistory";
std::string krn_uri_path = "/0/private/TradesHistory";

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
    std::cout << "[NONCE + REQUEST_DATA] " << request_nonce + request_data << std::endl; 
    std::cout << "[SHA256 (NONCE+DATA)] ";
    print_vec_as_hex(hashed_nonce_data);

    // Concatenate the hash with URI Path
    hashed_nonce_data.insert(hashed_nonce_data.begin(), uri_path.begin(), uri_path.end());
    std::cout << "[URIPATH + SHA256 (NONCE+DATA)] ";
    print_vec_as_hex(hashed_nonce_data);

    // HMAC the data and key, get the signature
    std::vector<unsigned char> signature = hmac_sha512_wrapper(hashed_nonce_data, api_key_secret);
    std::string str_signature = convert_vec_to_str(signature);
    std::string encoded_signature = crow::utility::base64encode(str_signature, str_signature.size());

    // Test
    std::vector<unsigned char> vect_encoded_signature (encoded_signature.begin(), encoded_signature.end());
    std::cout << "[B64_ENCODE (SIGNATURE)] ";
    print_vec_as_hex(vect_encoded_signature);

    return encoded_signature;
}

/*
 *  Send request for info to Kraken
 */
std::string KrakenDriver::query_for_trades() {
    ///////////////////////////////////////////////////////////////////////////
    // Part 1: Generate the required request headers                         //
    ///////////////////////////////////////////////////////////////////////////

    std::cout << "[API KEY] " << this->_ukey << "\n";
    std::cout << "[SECRET API KEY] " << this->_uprivatekey << "\n";

    if (this->_ukey == "" || this->_uprivatekey == "") {
        throw "API Creds Missing";
    }

    // Decode secret api key
    std::string decoded_api_secret = crow::utility::base64decode(this->_uprivatekey, this->_uprivatekey.size());
   
    // Test
    std::vector<unsigned char> vec_decoded_api_secret (decoded_api_secret.begin(), decoded_api_secret.end()); 
    std::cout << "[B64 DECODE (SECRET API KEY)] ";
    print_vec_as_hex(vec_decoded_api_secret);

    std::cout << "[B64 DECODE (SECRET API KEY) IN PLAINTEXT] " << decoded_api_secret << std::endl;
    
    // Generate nonce
    std::string request_nonce = this->generate_nonce();
    std::cout << "[NONCE] " << request_nonce << std::endl;

    // Generate payload
    std::string request_data  = this->generate_payload(request_nonce);
    std::cout << "[REQUEST DATA] " << request_data << std::endl;

    // Generate signature
    std::string request_sig = this->generate_signature(krn_uri_path, request_data, request_nonce, vec_decoded_api_secret);
    std::cout << "[REQUEST SIGNATURE] " << request_sig << std::endl; 

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

        curl_easy_setopt(curl, CURLOPT_URL, krn_url.c_str());                         // Set path to query
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);                                    // Set verbosity level
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
        std::cout << "[cURL RESPONSE] " << res << std::endl;
    } else {
        throw "Curl setup failed";
    }

    // Check return value of cURL query
    if (!(res == CURLE_OK)) {
       throw "Curl request failed";
    }
 
    // Deallocate resources
    // curl_easy_cleanup(curl);
    curl_slist_free_all(request_headers);

    return response_buffer;
}

std::vector<Trade> KrakenDriver::get_trades() {
    std::vector<Trade> processed_txs;
    std::string string_txs = this->query_for_trades();
    std::cout << "[QUERIED TRADES] " << string_txs << std::endl;

    //////////////////////////////
    // Process the transactions //
    //////////////////////////////

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
