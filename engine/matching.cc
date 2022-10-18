#include "matching.h"

using std::vector;


// get all matched trades for user
std::vector<MatchedTrade> get_matched_trades(AuthenticUser user);

// get pnl of trade
// internally, this assumes the trade was net-0 value the first day
// then calculated the present value of both legs to get the result
MatchedTrade get_pnl_from(AuthenticUser user, Trade trade) {
    return MatchedTrade();
}

// get year end stats for a year
YearEndPNL get_year_end_pnl(AuthenticUser user, std::chrono::time_point<std::chrono::system_clock> year) {
    return YearEndPNL();
}

// get pnl over various points in time
vector<SnapshotPNL> get_pnl_snapshots(AuthenticUser user, vector<TimeDelta> timestamps = DEFAULT_SAMPLES) {
    return vector<SnapshotPNL>();
}

// returns the earliest dates user could sell each of his cryptos for
// all of them to be considered long term cap gains
vector<Trade> get_earliest_long_term_sells(AuthenticUser user) {
    return vector<Trade>();
}