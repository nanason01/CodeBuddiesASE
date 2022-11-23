Dependencies:

```
apt-get install libsqlite3-dev # sqlite3 dev
apt-get install libcurl4-gnutls-dev # libcurl
https://github.com/CrowCpp/Crow/releases/tag/v1.0+5 # crow
apt install crow-v1.0+5.deb # to install crow deb
apt-get install libboost-all-dev # boost (needed by crow)
apt install lcov # for processing coverage test results
gcc --version >= 9.0.0
```

To run service:

```
bazel run //entry:service
bazel build //entry:service # to just build the binary, then the binary will exist in bazel-bin/
```

To modify database ddl:\
(eventually this will be done via an admin-only api endpoint)

```
./data/db_admin.sh {create, clear, dump} # any other input will print a help message
```

To run style checker:

```
pip install cpplint # only need to run install once
cpplint */* # to run for entire repo
cpplint <path_to_file> # to run for single file
```
To run static analysis:
```
apt-get install cppcheck # install once
cppcheck ./ --file-filter=*.cc # run for entire repo
cppcheck <file> # to run for single file
```

To run tests:

```
bazel test --test_output=all //... # run every test
bazel test --test_output=all //<target> # run single test suite
```

To run branch coverage checker:
```
./coverage-testing/test_coverage.sh
```
and launch the index.html generated in the coverage/ directory of this project in your browser for an interactive view of the results.

# Endpoint Specification :

Except for /getcredentials, calls to all other endpoints must be authenticated with the API key (or the refresh token when calling /refreshcredentials) by sending it in the **Authorization** header, where the API key (or the refresh token) is concatenated to the string _"Bearer "_ . (Note that the space after the letter r is intentional.)

| Endpoint url          |      | Description                                                                                                                                   | Input                                                                                                  | Output                                                                         |
| --------------------- | ---- | --------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------ |
| /getcredentials       | GET  | Generates credentials. Called the first time client uses the api                                                                              | None                                                                                                   | Json data with keys _client_id_, _api_key_ and _refresh_token_                 |
| /refreshcredentials   | GET  | Generates new API key and refresh_token for existing client. Client needs to authenticate with refresh_token instead of API key for this call | None                                                                                                   | Json data with keys _client_id_, _api_key_ and _refresh_token_                 |
| /trade                | POST | Takes information about a trade and saves it in the database                                                                                  | Json data with keys _timestamp_, _sold_currency_, _bought_currency_, _sold_amount_ and _bought_amount_ | HTTP Status Code                                                               |
| /exchangekey          | POST | Takes read key for a supported exchange and stores in database                                                                                | Json data with keys _exchange_, _secretkey_ and _readkey_                                              | HTTP Status Code                                                               |
| /removekey            | POST | Takes read key for a supported exchange and deletes that key from database                                                                    | Json data with key _exchange_                                             | HTTP Status Code                                                               |
| /get_annotated_trades | GET  | Gets matched trades                                                                                                                           | None                                                                                                   | Matched trades                                                                 |
| /year_end_stats       | GET  | Gets year end statistics for a year                                                                                                           | None                                                                                                   | Json data with keys _lt_realized_pnl_, _st_realized_pnl_ and _actual_pnl_      |
| /trade_pnl            | POST | Gets the profit and loss for the input trade and returns it                                                                                   | Json data with keys _timestamp_, _sold_currency_, _bought_currency_, _sold_amount_ and _bought_amount_ | The profit or loss : Json data with key _pnl_ whose value is the profit or loss |
| /portfolio_pnl        | GET  | Get profit and loss of portfolio at various points in time                                                                                    | None                                                                                                   | Profit and loss over time                                                      |


Example curl commands to send requests to the endpoints are in entry/README.md
