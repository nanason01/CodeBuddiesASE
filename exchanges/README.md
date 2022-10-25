### Exchanges Module

# Problem
*ENGINE/DATA* modules have to rely on client input for recording/processing transactions. We want to enrich our service with the
functionality to abstract much of that process away, if needed.

# Overview
We implemented the *EXCHANGES* module which provides support for automating transaction querying and abstracting it away from the client.
By allowing clients to send in their public and private api read-only keys for certain cryptocurrency exchanges (i.e. Coinbase and Kraken) we allow
for *DATA* to be able to directly obtain records of the most recent transactions that these clients have performed. 

Now, *DATA* is able to nstantiate a respective driver object for the exchange, and pass in the client's API keys. Through a chain of function
calls specific to each driver, they will obtain, process, and return to *DATA* a vector of most recently encountered trades for the specific
API key pair.

*EXCHANGES* relies heavily on 3 libraries: ```libcurl``` for the purpose of constructing HTTP(S) POST/GET requests, sending them off, and
retrieving the responses of our API endpoints. ```openssl``` for providing crypto-primitives (mainly for hashing and hmac) to sign the
requests sent off with use of ```libcurl```. ```crow``` for response JSON parsing, as well as base64 encoding and decoding (for private keys
and the final requests to other services).

API endpoints that we aim to reach:
- Kraken: 
    https://api.kraken.com/0/private/TradesHistory (allows us to query for individual client's recent transaction history).

- Coinbase:
    https://api.coinbase.com/v2/accounts/:account_id/transactions (same as above, where :account-id is replaced with the
        target client's actual account identification number).

Please note, in addition to the main functionality of this module contained in ```helpers.cc``` and ```exchanges.cc```, 
```exchanges.cc``` contains 3 definitions right at the top of the file taken from StackOverflow that we've used for the purpose
of debugging our use of certain crypto-primitives and our hashing function wrappers. The link to the actual post from under
which they were copied:

https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream


