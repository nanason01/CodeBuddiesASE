import requests
import pytest
import sys

SERVER = "http://0.0.0.0:18420/"

# Tests for endpoint /getcredentials
@pytest.fixture(scope="class")
def endpoint_getcredentials(request):
    response = requests.get(SERVER + "getcredentials")
    request.cls.response = response

@pytest.mark.usefixtures("endpoint_getcredentials")
class TestGetcredentialsClass:
    def test_response_getcredentials(self):
        #response = endpoint_getcredentials
        assert self.response.status_code == 200

        # save credentials in global variables
        authenticatedUser = self.response.json()
        global client_id
        global api_key
        global refresh_token
        client_id = authenticatedUser["client_id"]
        api_key = authenticatedUser["api_key"]
        refresh_token = authenticatedUser["refresh_token"]

        print(api_key + " " + refresh_token + " " + client_id)

    # test if api key and refresh token have correct composition
    def test_credential_properties(self):
        global client_id
        global api_key
        global refresh_token

        assert client_id == api_key[0 : 4] == refresh_token[0 : 4]

        print(api_key + " " + refresh_token + " " + client_id)

    # test if returned credentials authenticate user as expected
    def test_authentication(self):
        
        print(api_key + " " + refresh_token + " " + client_id)
        assert requests.get(SERVER, headers = {"Authorization" : "Bearer " + api_key}).status_code == 200

    # test if wrong api key returns correct error code
    def test_invalid_apikey(self):
        global client_id
        global api_key
        global refresh_token

        # Check Response for wrong api key
        if api_key == client_id + "aaaa":
            fake_api_key = "aaab"
        else:
            fake_api_key = "aaaa"
        response = requests.get(SERVER, headers = {"Authorization" : "Bearer " + client_id + fake_api_key})
        assert response.status_code == 401


# Tests for endpoint /refreshcredentials        
@pytest.fixture(scope="class")
def endpoint_refreshcredentials(request):
    response = requests.get(SERVER + "refreshcredentials", headers = {"Authorization" : "Bearer " + refresh_token})
    request.cls.response = response

@pytest.mark.usefixtures("endpoint_refreshcredentials")
class TestrefreshcredentialsClass:
    def test_response_refreshcredentials(self):
        assert self.response.status_code == 200

        # Update credentials
        global client_id
        global api_key
        global refresh_token

        authenticatedUser = self.response.json()
        api_key = authenticatedUser["api_key"]
        refresh_token = authenticatedUser["refresh_token"]

    def test_credential_properties(self):
        global client_id
        global api_key
        global refresh_token

        assert client_id == api_key[0 : 4] == refresh_token[0 : 4]

        print(api_key + " " + refresh_token + " " + client_id)

    def test_authentication_with_new_creds(self):
        
        print(api_key + " " + refresh_token + " " + client_id)
        assert requests.get(SERVER, headers = {"Authorization" : "Bearer " + api_key}).status_code == 200
    
    def test_invalid_apikey(self):
        global client_id
        global api_key
        global refresh_token

        # Check Response for wrong Refresh token
        if refresh_token == client_id + "aaaa":
            fake_refresh_key = "aaab"
        else:
            fake_refresh_key = "aaaa"
        response = requests.get(SERVER + "refreshcredentials", headers = {"Authorization" : "Bearer " + client_id + fake_refresh_key})
        assert response.status_code == 401

    def test_invalid_clientid(self):
        global client_id
        global api_key
        global refresh_token

        # Check Response for client Id that doesn't exist
        if client_id == "aaaa":
            fake_client_id = "aaab"
        else:
            fake_client_id = "aaaa"
        response = requests.get(SERVER + "refreshcredentials", headers = {"Authorization" : "Bearer " + fake_client_id + refresh_token})
        assert response.status_code == 401


# Tests for endpoint /trade
@pytest.fixture(scope="class")
def endpoint_trade(request):
    trade = {
        "timestamp":"2/02/2020",
        "sold_currency":"BTC",
        "bought_currency":"USD",
        "sold_amount":"1",
        "bought_amount":"1"
    }
    response = requests.post(SERVER + "trade", headers = {"Authorization" : "Bearer " + api_key}, json = trade)
    request.cls.response = response

@pytest.mark.usefixtures("endpoint_trade")
class TestTradeClass:
    def test_response_trade(self):
        assert self.response.status_code == 200

    def test_wrong_date(self):
        trade = {
            "timestamp":"33/02/2020",
            "sold_currency":"BTC",
            "bought_currency":"USD",
            "sold_amount":"1",
            "bought_amount":"1"
        }
        response = requests.post(SERVER + "trade", headers = {"Authorization" : "Bearer " + api_key}, json = trade)
        assert response.status_code == 400


# Tests for endpoint /exchangekey        
@pytest.fixture(scope="class")
def endpoint_exchangekey(request):
    keydata = {
        "exchange":"Kraken",
        "readkey":"J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU",
        "secretkey":"VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg=="
    }
    response = requests.post(SERVER + "exchangekey", headers = {"Authorization" : "Bearer " + api_key}, json = keydata)
    request.cls.response = response

@pytest.mark.usefixtures("endpoint_exchangekey")
class TestExchangekeyClass:
    def test_response_exchangekey(self):
        assert self.response.status_code == 200


# Tests for endpoint /removekey        
@pytest.fixture(scope="class")
def endpoint_removekey(request):
    keydata = { "exchange":"Kraken" }
    response = requests.post(SERVER + "removekey", headers = {"Authorization" : "Bearer " + api_key}, json = keydata)
    request.cls.response = response

@pytest.mark.usefixtures("endpoint_removekey")
class TestRemovekeyClass:
    def test_response_removekey(self):
        assert self.response.status_code == 200


# Tests for endpoint /trade_pnl
@pytest.fixture(scope="class")
def endpoint_tradepnl(request):
    trade = {
        "timestamp":"2/02/2020",
        "sold_currency":"BTC",
        "bought_currency":"USD",
        "sold_amount":"1",
        "bought_amount":"1"
    }
    response = requests.post(SERVER + "trade_pnl", headers = {"Authorization" : "Bearer " + api_key}, json = trade)
    request.cls.response = response

@pytest.mark.usefixtures("endpoint_tradepnl")
class TestTradepnlClass:
    def test_response_trade_pnl(self):
        assert self.response.status_code == 200

    # test json data returned
    def test_pnl_trade_pnl(self):
        pnl = self.response.json()
        assert float(pnl["pnl"]) or (not float(pnl["pnl"]))

    def test_wrong_date_format(self):
        trade = {
            "timestamp":"22/02/2020",
            "sold_currency":"BTC",
            "bought_currency":"USD",
            "sold_amount":"1",
            "bought_amount":"1"
        }
        response = requests.post(SERVER + "trade_pnl", headers = {"Authorization" : "Bearer " + api_key}, json = trade)
        assert response.status_code == 400


# Tests for endpoint /portfolio_pnl
@pytest.fixture(scope="class")
def endpoint_portfoliopnl(request):
    response = requests.get(SERVER + "portfolio_pnl", headers = {"Authorization" : "Bearer " + api_key})
    request.cls.response = response

@pytest.mark.usefixtures("endpoint_portfoliopnl")
class TestPortfoliopnlClass:
    def test_response_portfolio_pnl(self):
        assert self.response.status_code == 200

    # test json data returned
    def test_pnl_portfolio_pnl(self):
        pnl = self.response.json()
        assert float(pnl["pnl"]) or (not float(pnl["pnl"]))


# Tests for endpoint /get_annotated_trades
def test_get_annotated_trades():
    global api_key
    keydata = {
        "exchange":"Kraken",
        "readkey":"J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU",
        "secretkey":"VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg=="
    }
    response = requests.post(SERVER + "exchangekey", headers = {"Authorization" : "Bearer " + api_key}, json = keydata)
    response.status_code == 200
    response = requests.get(SERVER + "get_annotated_trades", headers = {"Authorization" : "Bearer " + api_key})
    assert response.status_code == 200
    print(response.json())


# Tests for endpoint /year_end_pnl
def test_year_end_pnl():
    global api_key
    response = requests.get(SERVER + "year_end_stats", headers = {"Authorization" : "Bearer " + api_key})
    assert response.status_code == 200
    print(response.json())


# Tests to check response if Authorization header not sent : get endpoints
@pytest.mark.parametrize("endpoint", [("refreshcredentials"), ("get_annotated_trades"), ("year_end_stats"), ("portfolio_pnl")])
def test_no_auth_headers_get_endpoints(endpoint):
    assert requests.get(SERVER + endpoint).status_code == 400


# Tests to check response if Authorization header not sent : post endpoints
@pytest.mark.parametrize("endpoint", [("trade"), ("exchangekey"), ("removekey"), ("trade_pnl")])
def test_no_auth_headers_post_endpoints(endpoint):
    assert requests.post(SERVER + endpoint).status_code == 400


# Tests to check response if body not sent : post endpoints
@pytest.mark.parametrize("endpoint", [("trade"), ("exchangekey"), ("removekey"), ("trade_pnl")])
def test_no_body_post_endpoints(endpoint):
    assert requests.post(SERVER + endpoint, headers = {"Authorization" : "Bearer " + api_key}).status_code == 400


# Tests to check response if wrong api_key sent : get endpoints
@pytest.mark.parametrize("endpoint", [("get_annotated_trades"), ("year_end_stats"), ("portfolio_pnl")])
def test_wrong_apikey_get_endpoints(endpoint):
    # Check Response for wrong api key
    if api_key == client_id + "aaaa":
        fake_api_key = "aaab"
    else:
        fake_api_key = "aaaa"
    response = requests.get(SERVER + endpoint, headers = {"Authorization" : "Bearer " + client_id + fake_api_key})
    assert response.status_code == 401


# data to send to post requests
trade = {
    "timestamp":"2/02/2020",
    "sold_currency":"BTC",
    "bought_currency":"USD",
    "sold_amount":"1",
    "bought_amount":"1"
}

keydata = {
    "exchange":"Kraken",
    "readkey":"J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU",
    "secretkey":"VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg=="
}

removekeydata = { "exchange":"Kraken" }

# Tests to check response if wrong api_key sent : post endpoints
@pytest.mark.parametrize("endpoint, data", [("trade", trade), ("exchangekey", keydata), ("removekey", removekeydata), ("trade_pnl", trade)])
def test_wrong_apikey_post_endpoints(endpoint, data):
    # Check Response for wrong api key
    if api_key == client_id + "aaaa":
        fake_api_key = "aaab"
    else:
        fake_api_key = "aaaa"
    response = requests.post(SERVER + endpoint, headers = {"Authorization" : "Bearer " + client_id + fake_api_key}, json = data)
    assert response.status_code == 401


# Tests to check response if wrong user sent : get endpoints
@pytest.mark.parametrize("endpoint", [("get_annotated_trades"), ("year_end_stats"), ("portfolio_pnl")])
def test_wrong_clientid_get_endpoints(endpoint):
    # Check Response for client Id that doesn't exist
    if client_id == "aaaa":
        fake_client_id = "aaab"
    else:
        fake_client_id = "aaaa"
    response = requests.get(SERVER + endpoint, headers = {"Authorization" : "Bearer " + fake_client_id + api_key})
    assert response.status_code == 401


# Tests to check response if wrong user sent : post endpoints
@pytest.mark.parametrize("endpoint, data", [("trade", trade), ("exchangekey", keydata), ("removekey", removekeydata), ("trade_pnl", trade)])
def test_wrong_clientid_post_endpoints(endpoint, data):
    # Check Response for client Id that doesn't exist
    if client_id == "aaaa":
        fake_client_id = "aaab"
    else:
        fake_client_id = "aaaa"
    response = requests.post(SERVER + endpoint, headers = {"Authorization" : "Bearer " + fake_client_id + api_key}, json = data)
    assert response.status_code == 401


# if __name__ == "__main__":
#     import pytest
#     import requests
#     raise SystemExit(pytest.main([__file__]))
if __name__ == "__main__":
    sys.exit(pytest.main([__file__]))
