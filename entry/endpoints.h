//
// API endpoints
//

#pragma once

#include "common/types.h"
#include "data/data.h"
#include "engine/matcher.h"

class Endpoints {
    Data _data;
    Matcher _matcher;

    // use these->func to call functions
    BaseData* data;
    BaseMatcher* matcher;

public:
    Endpoints() : data(&_data), matcher(&_matcher) {}

    // only for testing
    Endpoints(BaseData* _data_ptr, BaseMatcher* _matcher_ptr)
        : data(_data_ptr), matcher(_matcher_ptr) {}

};