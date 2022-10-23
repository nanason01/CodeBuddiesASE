### The core of our project

# What counts as a trade?

Currently, any exchange of property to another property will trigger a realization of gain. For cryptos, there's no difference in exchanging ETH for BTC and selling ETH and buying BTC with the proceeds.

This is not the case in all asset classes.

# An overview of relavent tax law:

In order to calculate tax liability for a year, you must calculate the amount you made.
That is what is your overall trading revenue minus your overall cost?

How to calculate that is usually pretty straightforward, but there are some interesting cases.
E.g. what if you buy 1 BTC at 20k, then later buy 1 BTC at 40k, then even later sell 1 BTC at 30k. Did you make 10k or lose 10k?

The IRS gives you latitude to select yourself which trade you "match" to each sell, and there is a correct answer: in general we want to minimize our immediate tax burden and float gains for as long as possible. 

A very important consideration is long term vs short term capital gains. Short term capital gains (STCG) are taxed at ordinary income rates (OI) while long term capital gains are taxed at more favorable long term capital gains (LTCG) rates. So, another consideration in the above example is are either of the buys long enough ago that the sell will be taxed at LTCG rates?

At least so you'd think, the last consideration is how gains and losses offset each other. Specifically, STCL offset STCG and LTCL offset LTCG. Then, if there are STCL and LTCG (or vice versa), the remaining category offset each other.

An example to illustrate that:

(1/1/2020) buy 10 A @ $20/ea
(9/1/2020) buy 10 A @ $30/ea
(2/1/2021) sell 15 A @ $25/ea

If we say we sold 5 from the sept trade and all of the jan trade, then we'd have $50 in LTCG and $25 in STCL, so in total we'd have a $25 LTCG, and a floating $25 loss.

If we say we sold 5 from the jan trade and all of the sept trade, we'd have $25 in LTCG and $50 in STCL, so in total we'd have $25 STCL, and a floating $25 gain.

Which is better?

In general, cap losses aren't very useful on their own, you can probably only deduct 3k of them off of OI. However, the rest will carryforward and can be used in later years.

Paying taxes however is probably less ideal.

# Our algorithm prioritizes as follows

Realize STCL
Realize LTCL
Realize LTCG
Realize STCG

That is we will pair every buy and sell to maximize the realized STCL, then if trades are still unpaired maximize the LTCL, then pair trades to minimize LTCG (but account for every sell), then pair the remaining to minimize STCG.

Note this may not be ideal in every case: ie this relies on the assumption that you *eventually* will sell, but you may just die, then your basis is stepped up and whoever inherets it can sell it without a realized gain. In that case, aggressively minimizing overall tax liability ie probably more STCG realized but less gain overall makes more sense than minimizing taxes in inefficient buckets.

However, the cases where any of that's actually relavent are very niche. Most retail investors trades are probably to-close or to-open, meaning they are either selling their entire position or buying position in a new currency. No ambiguity here, sell-to-close must match the last buy-to-open.

Lastly, you must pay your taxes every year, and you can't go back and "change your answers" as to how you pair buys with sells. So, this assumes you used our algorithm for each year and the already paired buys are off limits for this years pairings. In the future, maybe we can allow you to request a hard coded FIFO, LIFO, or HIFO calculation for previous years, as that's what most brokers use and probably how you calculated it.

#Get PNL Snapshots

The actual PNL of your trades is much less ambiguous. In reality, you either made or lost money. The value of what you bought has shifted relative to what you sold, and if you reverse it right now you will have gain or lost value. Straightforward as that.

## **Disclaimer**
None of this is intended as tax advice nor trading advice and is only intended as a software engineering project for entertainment classes. We take no responsibility for our data gathering, our outputs, and ***especially your taxes***. You should consult a professional tax program for good advice.
