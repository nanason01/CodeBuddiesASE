// Copyright 2022 CodeBuddies ASE Group
//
// Base driver for exchanges
//

#pragma once

#include <exception>
#include <vector>

#include "common/types.h"

struct APICredsMissing : std::exception {
    const char* what() const noexcept override {
        return "Credentials for API Calls are Missing";
    }
};

class ExchangeDriver {
public:
    virtual ~ExchangeDriver() = default;
    virtual std::vector<Trade> get_trades(API_key public_key,
        API_key private_key) = 0;
};
