### The entrypoints to the service

This folder contains the entrypoints for the service.

# main.cc
main.cc - Contains the endpoints of the service and the handler functions for each of them.

# endpoints.cc
endpoints.cc - Contains all of the authentication functionality, and the handlers that run when each endpoint is called. endpoints.cc is basically the interface between the clients and the rest of the functionality of the project. The detailed inputs and outputs for the endpoints is in the main README.md

# Authentication

We use tokens for authenticating the client. The client is identified by a client ID, and is issued an API key that needs to be sent with every request to the endpoint. The client is also issued a Refresh token that is used to change the API key in case the old API key is lost or compromised. The refresh token is only used for one purpose - it is sent with a request to /refreshcredentials when a client needs to change the credentials. Except for /getcredentials, calls to all other endpoints must be authenticated with the API key (or the refresh token when calling /refreshcredentials) by sending it in the **Authorization** header, where the API key (or the refresh token) is concatenated to the string _"Bearer "_ . (Note that the space after the letter r is intentional.)

# Example calls to endpoints

Below commands are tested on Ubuntu 20.04.

| Endpoint         | Curl Command |
| ---------------- | ------------ |
| /getcredentials  | curl http://0.0.0.0:18420/getcredentials  |
| /refreshcredentials | curl http://0.0.0.0:18420/refreshcredentials -H 'Authorization: Bearer rmwbRBI4' |
| /exchangekey | curl http://0.0.0.0:18420/exchangekey -H 'Authorization: Bearer Fw7JrYcV' -d '{"exchange":"Kraken", "readkey":"J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU", "secretkey":"VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg=="}' |
| /removekey | curl http://0.0.0.0:18420/removekey -H 'Authorization: Bearer Fw7JrYcV' -d '{"exchange":"Kraken"}' |
| /trade | curl -X POST http://0.0.0.0:18420/trade -H 'Authorization: Bearer Fw7JrYcV' -d '{"timestamp":"2/02/2020", "sold_currency":"BTC", "bought_currency":"USD", "sold_amount":"-1", "bought_amount":"-1"}' |
| /portfolio_pnl | curl http://0.0.0.0:18420/portfolio_pnl -H 'Authorization: Bearer XkKfVmF7' |
| /trade_pnl | curl -X POST http://0.0.0.0:18420/trade_pnl -H 'Authorization: Bearer XkKfVmF7' -d '{"timestamp":"2/02/2020", "sold_currency":"BTC", "bought_currency":"USD", "sold_amount":"-1", "bought_amount":"-1"}' |

Please note that the port used by crow might be different. Please generate your own API key and use that in the above commands, as the database is local right now.