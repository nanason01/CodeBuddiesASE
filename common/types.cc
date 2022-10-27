#include "types.h"

std::ostream& operator<<(std::ostream& os, const Trade& tr) {
    os << "trade at ";
    std::PrintTo(tr.timestamp, &os);
    os << " exchanged " << tr.sold_amount << " " <<
        tr.sold_currency << " for " << tr.bought_amount <<
        tr.bought_currency;

    return os;
}

std::ostream& operator<<(std::ostream& os, const MatchedTrade& mt) {
    switch (mt.term) {
    case(Term::Short):
        os << "Short term " << mt.sz << ":" << mt.currency <<
            " bought at ";
        std::PrintTo(mt.bought_timestamp, &os);
        os << " sold at ";
        std::PrintTo(mt.sold_timestamp, &os);
        os << " pnl: " << mt.pnl;
        break;
    case(Term::Long):
        os << "Long term " << mt.sz << ":" << mt.currency <<
            " bought at ";
        std::PrintTo(mt.bought_timestamp, &os);
        os << " sold at ";
        std::PrintTo(mt.sold_timestamp, &os);
        os << " pnl: " << mt.pnl;
        break;
    case(Term::UnmatchedSell):
        os << "Unmatched Sell " << mt.sz << ":" << mt.currency <<
            " sold at ";
        std::PrintTo(mt.sold_timestamp, &os);
        os << " pnl: " << mt.pnl;
        break;
    case(Term::Held):
        os << "Held " << mt.sz << ":" << mt.currency <<
            " bought at ";
        std::PrintTo(mt.bought_timestamp, &os);
        os << " pnl: " << mt.pnl;
        break;
    }

    return os;
}

void std::PrintTo(const Timestamp& ts, std::ostream* os) {
    std::tm* cal_ts = localtime(&ts);

    *os << std::put_time(cal_ts, "%c %Z");
}
