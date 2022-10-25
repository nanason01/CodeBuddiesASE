//
// Base driver for exchanges
// Copyright 2022 CodingBuddies

#pragma once

#include <exception>
#include <vector>

#include "common/types.h"

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
 public:
    ExchangeDriver() : Driver() {}
    virtual ~ExchangeDriver() = default;
};
