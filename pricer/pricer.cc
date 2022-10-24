#include <curl/curl.h>
#include <iostream>
#include <crow.h>

#include "base_pricer.h"
#include "pricer.h"

/*
 * 
 */
std::string convert_to_string(const crow::json::rvalue jrvalue){
	std::ostringstream os;
        os << jrvalue;
        std::string s = os.str();
        s = s.substr(1, s.length() - 2);
        return s;
}

/*
 *
 */
size_t pricer_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::string *response = reinterpret_cast<std::string *> (userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

/*
 * 
 */
double Pricer::process_response(std::string response_buffer, std::string currency_pair) {
    double ans = 0;
    auto x = crow::json::load(response_buffer);
    if (!x["result"] || !x["result"][currency_pair] || !x["result"][currency_pair]["a"]) {
        goto out;
    }

    ans = std::__cxx11::stod(convert_to_string(x["result"][currency_pair]["a"][0]));

out:
    return ans;
}

/*
 * PAIR HAS TO BE FORMATTED ACCORDING TO KRAKEN API DOC.
 * SEE: https://docs.kraken.com/rest/#tag/Market-Data/operation/getTradableAssetPairs
 */
double Pricer::get_usd_price(std::string currency_pair) {
    std::string response_buffer;
    CURLcode res;
    double ans;

    std::string url = "https://api.kraken.com/0/public/Ticker?pair=" + currency_pair;

    CURL *curl = curl_easy_init();
    if(curl) {
        // Perform a GET request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "CodeBuddies Crypto Portfolio Tracker");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pricer_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void*> (&response_buffer));
        
        res = curl_easy_perform(curl);
    } else {
        goto err_out;
    }

    if (!(res == CURLE_OK)) {
        goto err_out;
    }


    ans = process_response(response_buffer, currency_pair);

    // Deallocate resources
    curl_easy_cleanup(curl);
    return ans;

err_out:
    return 0;
}