### Pricer Module

# Problem
*ENGINE/DATA* modules have to rely on the clients submitting past trades in order to be able to perform analysis. That analysis is limited in
scope, as these modules are unable to find prices of these assets at the time of when they were traded/bought.

# Overview
This module fills in the gap to provide the *ENGINE/DATA* modules to obtain information on the price of assets (in USD). It relies on
accessesing CoinGecko's service through 2 of its API endpoints. Same as the *EXCHANGES* module, here we rely on functionality from ```libcurl```
to perform formatted ```GET``` requests to CoinGecko.

The 2 API endpoints used are:
1) https://api.coingecko.com/api/v3/coins/list
2) https://api.coingecko.com/api/v3/coins/:currency-id/history?date=:timestamp

Querying (1) allows us to obtain a list of all cryptocurrencies CoinGecko has on record, and match the ticker of a currency provided to
the *PRICER* by *ENGINE/DATA*, to the ```:currency-id```. 

After successfully querying (1), we can then query (2). We replace ```:currency-id``` with the actual identifier of the currency for which
a ticker was provided earlier on, and teplace ```:timestamp``` with a properly formatted timestamp as ```DD:MM:YYY```. On success, the response
from CoinGecko will contain the information on the market price of the cryptocurrency, of which the crypt-USD pair is the most relevant to us.

We extract the price of a currency on a given date and the module returns it to the calling method.