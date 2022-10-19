#include "matching.h"

using std::vector;

// get all matched trades for user
// TODO: test this works with intraday trades
std::vector<MatchedTrade> get_matched_trades(AuthenticUser user);

PNL __get_pnl_from(Trade trade, Timestamp end_time) {
    return PNL((trade.sold_amount * get_usd_price(trade.sold_currency, trade.timestamp)) +
        (trade.bought_amount * get_usd_price(trade.bought_currency, end_time)) -
        (trade.sold_amount * get_usd_price(trade.sold_currency, end_time)) -
        (trade.bought_amount * get_usd_price(trade.bought_currency, trade.timestamp)));
}

// get pnl of trade
// internally, this assumes the trade was net-0 value the first day
// then calculated the present value of both legs to get the result
PNL get_pnl_from(AuthenticUser user, Trade trade, Timestamp end_time = std::chrono::system_clock::now()) {
    check_creds(user);

    return __get_pnl_from(trade, end_time);
}

// get net pnl of a user up to end_date
PNL get_net_pnl(AuthenticUser user, Timestamp end_time = std::chrono::system_clock::now()) {
    PNL pnl = 0.0;

    for (const auto trade : get_trades(user)) {
        pnl += __get_pnl_from(trade, end_time);
    }

    return pnl;
}

// get year end stats for a year
YearEndPNL get_year_end_pnl(AuthenticUser user, Timestamp year) {
    PNL net = 0.0, lt = 0.0, st = 0.0;

    for (const auto matched_trade : get_matched_trades(user)) {
        if (matched_trade.sold_timestamp.year() != year.year())
            continue;

        net += matched_trade.pnl;

        switch (matched_trade.term) {
        case(Term::Long):
            lt += matched_trade.pnl;
        case(Term::Short):
            st += matched_trade.pnl;
        }
    }

    return YearEndPNL{
        .actual = net,
        .lt_realized = lt,
        .st_realized = st,
    };
}

// get pnl over various points in time
vector<SnapshotPNL> get_pnl_snapshots(AuthenticUser user, vector<TimeDelta> timestamps = DEFAULT_SAMPLES) {
    PNL running_pnl = 0.0;

    for (const auto offset : get_trades | ranges::rev)
        ;

    this_should_not_compile
}

// returns the earliest dates user could sell each of his cryptos for
// all of them to be considered long term cap gains
vector<Trade> get_earliest_long_term_sells(AuthenticUser user) {
    return vector<Trade>();
}