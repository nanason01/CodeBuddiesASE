//
// Base driver for exchanges
//
// TODO: if registering an API key requires 2 stages,
// we'll have to keep some state for in-progress registries
// like a hashmap of user -> in progress Driver
//
// After that, hopefully we can just use their API_key,
// which should be maintained by data/
//

#pragma once

#include "common/types.h"

#include <exception>
#include <vector>

struct InvalidAPIKey : std::exception {
    const char* what() const noexcept override {
        return "Invalid API Key";
    }
};
struct WriteableAPIKey : std::exception {
    const char* what() const noexcept override {
        return "API Key writeable";
    }
};

class ExchangeDriver {
protected:
    API_key key;
public:
    virtual ~ExchangeDriver() = default;

    virtual std::vector<Trade> get_trades(User user, API_key key) = 0;
    virtual void check_api_key(User user, API_key key) = 0;
};