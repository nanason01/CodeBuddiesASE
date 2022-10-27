//
// Base driver for exchanges
// Copyright 2022 CodingBuddies

#pragma once

#include <exception>
#include <vector>

#include "common/types.h"

struct InvalidAPIKey: std::exception {
    const char* what() const noexcept override {
        return "Invalid API Key";
    }
};

struct APICredsMissing: std::exception {
    const char* what() const noexcept override {
        return "Credentials for API Calls are Missing";
    }
};

struct CurlSetupFailed: std::exception {
    const char* what() const noexcept override {
        return "Setting up cURL failed";
    }
};

struct CurlRequestFailed: std::exception {
    const char* what() const noexcept override {
        return "Requesting resource with cURL failed";
    }
};

class ExchangeDriver {
public:
    virtual ~ExchangeDriver() = default;
    virtual std::vector<Trade> get_trades(API_key public_key,
        API_key private_key) = 0;
};
