#include "matcher.h"

#include <unordered_map>
#include <map>
#include <queue>
#include <algorithm>
#include <limits>

using std::vector;
using std::unordered_map;
using std::map;
using std::priority_queue;

constexpr Term Matcher::get_term(Timestamp buy, Timestamp sell) {
    using namespace std::chrono;

    buy = normalize(buy);
    sell = normalize(sell);

    if (buy.year() - sell.year() != years{ 1 })
        return buy.year() - sell.year() > years{ 1 } ? Term::Long : Term::Short;
    else
        return buy.month() - sell.month() >= months{ 1 } || buy.day() - sell.day() >= days{ 1 } ?
        Term::Long : Term::Short;
}

// get all matched trades for user
// see README for details on how this is done
vector<MatchedTrade> Matcher::get_matched_trades(const vector<Trade>& trades_in) {
    vector<MatchedTrade> ret;

    if (trades_in.empty())
        return ret;

    struct TradePayload {
        Timestamp ts;
        double basis;
        double rem_sz;
    };

    map<std::chrono::year, vector<Trade>> year_to_trades;

    for (const Trade& trade : trades_in) {
        year_to_trades[trade.timestamp.year()].push_back(trade);
    }

    // curr -> (list of trade sz and date info)
    unordered_map<std::string, vector<TradePayload>> unmatched_buys;

    // work through each year, backwards
    for (const auto& [year, trades] : year_to_trades) {
        for (const auto& trade : trades) {
            if (trade.bought_currency == trade.sold_currency)
                continue; // should this throw ?? is this even worth checking ?

            const double basis_bought = trade.bought_currency == "USD" ? 1 :
                trade.sold_currency == "USD" ? trade.bought_amount / trade.sold_amount :
                pricer->get_usd_price(trade.bought_currency, trade.timestamp);

            unmatched_buys[trade.bought_currency].push_back({
                trade.timestamp,
                basis_bought,
                trade.bought_amount
                });

            const double basis_sold = trade.sold_currency == "USD" ? 1 :
                trade.bought_currency == "USD" ? trade.sold_amount / trade.bought_amount :
                pricer->get_usd_price(trade.sold_currency, trade.timestamp);
            // all of the sold_amount must be matched with a corresponding buy
            double sz = trade.sold_amount;

            // this is pretty much the driver
            // STCL > LTCL > LTCG ~> fixing STCG
            // so losses over gains
            // STCL over LTCL
            // LTCG over STCG if over some fixing factor
            const auto comp = [sell_ts = trade.timestamp, new_basis = basis_sold](const TradePayload& a, const TradePayload& b) {
                const double a_gain = new_basis - a.basis;
                const double b_gain = new_basis - b.basis;

                if ((a_gain > 0) != (b_gain > 0))
                    // if different directions, losses preferred to gains
                    return a_gain > 0;
                else if (get_term(a.ts, sell_ts) == get_term(b.ts, sell_ts))
                    // if same term, same direction, higher basis preferred
                    return a.basis < b.basis;
                else if (a_gain <= 0)
                    // if both loss, diff term, STCL preferred over LTCL
                    return get_term(a.ts, sell_ts) == Term::Long;
                else if (get_term(a.ts, sell_ts) == Term::Long)
                    // if both gain, a is LTCG, prefer minimal normalized gain
                    return a_gain > b_gain * ST_FIXING;
                else
                    // same as prev, but b is LTCG
                    return a_gain * ST_FIXING > b_gain;
            };
            priority_queue<TradePayload, vector<TradePayload>, decltype(comp)>
                matching_orders(
                    unmatched_buys[trade.sold_currency].begin(),
                    unmatched_buys[trade.sold_currency].end(),
                    comp
                );

            unmatched_buys[trade.sold_currency].clear();

            while (sz && !matching_orders.empty()) {
                TradePayload matched = matching_orders.top();
                matching_orders.pop();

                const double sz_filled = std::min(sz, matched.rem_sz);

                matched.rem_sz -= sz_filled;
                sz -= sz_filled;

                ret.push_back({
                    matched.ts,
                    trade.timestamp,
                    get_term(matched.ts, trade.timestamp),
                    trade.sold_currency,
                    sz_filled,
                    sz_filled * (basis_sold - matched.basis)
                    });

                // if there is still volume left, push this back
                if (matched.rem_sz > std::numeric_limits<double>::epsilon())
                    matching_orders.push(matched);
            }

            // case: put unused buys back into unmatched buys
            while (!matching_orders.empty()) {
                unmatched_buys[trade.sold_currency].push_back(matching_orders.top());
                matching_orders.pop();
            }

            // case: remaining sold volume unaccounted for
            if (sz) {
                ret.push_back({
                    beginning_of_time(),
                    trade.timestamp,
                    Term::UnmatchedSell,
                    trade.sold_currency,
                    sz,
                    sz * basis_sold
                    });
            }
        }
    }

    // register any held positions
    for (const auto& [curr, buys] : unmatched_buys) {
        for (const auto& buy : buys) {
            ret.push_back({
                buy.ts,
                now(),
                Term::Held,
                curr,
                buy.rem_sz,
                buy.rem_sz * (pricer->get_usd_price(curr) - buy.basis)
                });
        }
    }

    return ret;
}

// get pnl of trade
// internally, this assumes the trade was net-0 value the first day
// then calculated the present value of both legs to get the result
PNL Matcher::get_pnl_from(Trade trade, Timestamp end_time) {
    const double cur_short_px = trade.sold_currency == "USD" ? 1.0 :
        pricer->get_usd_price(trade.sold_currency, end_time);
    const double cur_long_px = trade.bought_currency == "USD" ? 1.0 :
        pricer->get_usd_price(trade.bought_currency, end_time);

    return PNL((trade.bought_amount * cur_long_px) -
        (trade.sold_amount * cur_short_px));
}

// get net pnl of a user up to end_date
PNL Matcher::get_net_pnl(const vector<Trade>& trades, Timestamp end_time) {
    PNL pnl = 0.0;

    for (const auto& trade : trades) {
        if (trade.timestamp > end_time)
            continue;

        pnl += get_pnl_from(trade, end_time);
    }

    return pnl;
}

// get year end stats for a year
YearEndPNL Matcher::get_year_end_pnl(const vector<Trade>& trades, Timestamp year) {
    PNL net = 0.0, lt = 0.0, st = 0.0;

    for (const auto& matched_trade : get_matched_trades(trades)) {
        if (matched_trade.sold_timestamp.year() != year.year())
            continue;

        net += matched_trade.pnl;

        switch (matched_trade.term) {
        case(Term::Long):
            lt += matched_trade.pnl;
        case(Term::Short):
            st += matched_trade.pnl;
        case (Term::Held):
        case (Term::UnmatchedSell):
            throw;
        }
    }

    return YearEndPNL{
        .lt_realized = lt,
        .st_realized = st,
        .actual = net,
    };
}

// get pnl over various points in time
// currently assumes that get_trades returns trades in chronological asc order
vector<SnapshotPNL> Matcher::get_pnl_snapshots(const vector<Trade>& trades, vector<TimeDelta> timedeltas) {
    vector<SnapshotPNL> ret;

    for (const auto delta : timedeltas) {
        const auto next_timestamp = normalize(now() - delta);

        ret.push_back({
            .timestamp = next_timestamp,
            .pnl = get_net_pnl(trades, next_timestamp),
            });
    }

    return ret;
}

// returns the earliest dates user could sell each of his cryptos for
// all of them to be considered long term cap gains
// Trade::bought_amount is meaningless as the future price is unknown
vector<Trade> Matcher::get_earliest_long_term_sells(const vector<Trade>& trades) {
    using namespace std::chrono;

    vector<Trade> ret;

    for (const auto& mt : get_matched_trades(trades)) {
        if (mt.term != Term::Held)
            continue;

        ret.push_back({
            .timestamp = std::max(
                normalize(mt.bought_timestamp + years{1} + days{1}),
                now()
            ),
            .sold_currency = mt.currency,
            .bought_currency = "USD",
            .sold_amount = mt.sz,
            .bought_amount = 0.0,
            });
    }

    return ret;
}