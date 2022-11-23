// Copyright 2022 CodeBuddies ASE Group

#include <iostream>

#include "engine/matcher.h"
#include "common/types.h"

int main() {
    Matcher m;

    const Trade trade_in = {
        from_usa_date(5, 14, 2021),
        "ETH",
        "BTC",
        12.821,
        1.0,
    };
    const Timestamp sample_end = from_usa_date(11, 18, 2022);

    const auto res1 = m.get_pnl_from(trade_in, sample_end);

    std::cout << "res1: " << res1 << std::endl;
}
