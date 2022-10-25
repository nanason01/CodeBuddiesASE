//
// Base driver for exchanges
//

#pragma once

#include "common/types.h"
#include <exception>
#include <vector>

struct InvalidAPIKey : std::exception {
    const char* what() const noexcept override {
        return "Invalid API Key";
    }
} inv_key;

struct APICredsMissing : std::exception {
    const char* what() const noexcept override {
        return "Credentials for API Calls are Missing";
    }
} creds_mia;

struct CurlSetupFailed : std::exception {
    const char* what() const noexcept override {
        return "Setting up cURL failed";
    }
} curl_setup_fail;

struct CurlRequestFailed : std::exception {
    const char* what() const noexcept override {
        return "Requesting resource with cURL failed";
    }
} curl_request_fail;

class ExchangeDriver : Driver {
protected:
    API_key key;
    API_key privatekey;

public:
    ExchangeDriver(User _user, API_key _key, API_key _privatekey) : Driver(_user), key(_key), privatekey(_privatekey) {}
    virtual ~ExchangeDriver() = default;
};