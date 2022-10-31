// Copyright 2022 CodingBuddies
#include <curl/curl.h>
#include <crow.h>

#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>

#include "pricer/pricer.h"

/*
 *
 */
size_t pricer_write_callback(char* ptr, size_t size,
    size_t nmemb, void* userdata) {
    std::string* response = reinterpret_cast<std::string*> (userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

/*
 *
 */
std::string Pricer::perform_curl_request(std::string url) {
    std::string response_buffer;

    // Get the history of trades for that day for this asset
    CURL* curl = curl_easy_init();
    if (curl) {
        // Perform a GET request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT,
            "CodeBuddies Crypto Portfolio Tracker");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pricer_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA,
            static_cast<void*> (&response_buffer));

        curl_easy_perform(curl);
    } else {
        goto out;
    }

    // Deallocate resources
    curl_easy_cleanup(curl);

out:
    return response_buffer;
}

/*
 *
 */
std::string Pricer::get_asset_id(std::string currency) {
    size_t i;
    std::string url_list = "https://api.coingecko.com/api/v3/coins/list";

    std::string list_of_ids = this->perform_curl_request(url_list);
    auto jsonified_ids = crow::json::load(list_of_ids);
    
    for (i = 0; i < jsonified_ids.size(); i++) {
        if (std::string(jsonified_ids[i]["symbol"]) == currency) {
            return std::string(jsonified_ids[i]["id"]);
        }
    }

    return "";
}

/*
 *
 */
std::string Pricer::format_timestamp(Timestamp tstamp) {
    // Format : DD-MM-YEAR
    std::string day = std::to_string(get_day(tstamp));
    std::string month = std::to_string(get_month(tstamp));
    std::string year = std::to_string(get_year(tstamp));
    return (day + "-" + month + "-" + year);
}

/*
 *
 */
double Pricer::get_asset_price(std::string currency_id, Timestamp tstamp) {
    double ans = 0;

    std::string timestamp_str = format_timestamp(tstamp);

    std::string url_list = "https://api.coingecko.com/api/v3/coins/" +
        currency_id + "/history?date=" + timestamp_str;

    std::string price_records = this->perform_curl_request(url_list);
    //std::cout<<price_records<<std::endl;
    auto jsonified_ids = crow::json::load(price_records);

    if (jsonified_ids["market_data"] &&
        jsonified_ids["market_data"]["current_price"] &&
        jsonified_ids["market_data"]["current_price"]["usd"]) {
        ans = jsonified_ids["market_data"]["current_price"]["usd"].d();
    }

    return ans;
}

/*
 * CURRENCY HAS TO BE THE TICKER (e.g. 'BTC' for BITCOIN).
 * SEE: https://www.coingecko.com/en/api/documentation
 */
double Pricer::get_usd_price(std::string currency, Timestamp tstamp) {
    // Ensure that the currency is in lower case
    std::transform(currency.begin(),
        currency.end(), currency.begin(), [](unsigned char x) {
            return std::tolower(x);
        });

    //transform(currency.begin(),currency.end(),currency.begin(),::tolower);
    //std::cout<<currency<<std::endl;

    // Get the asset id for CoinGecko
    std::string currency_id = this->get_asset_id(currency);
    if (currency_id == "") {
        return 0;
    }
    //std::cout<<currency_id<<std::endl;
    return this->get_asset_price(currency_id, tstamp);
}
