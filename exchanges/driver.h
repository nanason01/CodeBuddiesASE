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

#include "../common/types.h"
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
    API_key privatekey;

public:
    ExchangeDriver(User _user, API_key _key, API_key _privatekey) : Driver(_user), key(_key), privatekey(_privatekey) {}
    virtual ~ExchangeDriver() = default;
};
