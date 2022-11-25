import requests
import pytest

SERVER = "http://0.0.0.0:18420/"


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

    def test_credential_properties(self):
        global client_id
        global api_key
        global refresh_token

        assert client_id == api_key[0 : 4] == refresh_token[0 : 4]

        print(api_key + " " + refresh_token + " " + client_id)

    def test_authentication(self):
        
        print(api_key + " " + refresh_token + " " + client_id)
        assert requests.get(SERVER, headers = {"Authorization" : "Bearer " + api_key}).status_code == 200

    def test_invalid_apikey(self):
        global client_id
        global api_key
        global refresh_token

        # Check Response for wrong Refresh token
        if api_key == client_id + "aaaa":
            fake_api_key = "aaab"
        else:
            fake_api_key = "aaaa"
        response = requests.get(SERVER, headers = {"Authorization" : "Bearer " + client_id + fake_api_key})
        assert response.status_code == 401

        
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

# def test_trade():
#     global api_key
#     trade = {
#         "timestamp":"2/02/2020",
#         "sold_currency":"BTC",
#         "bought_currency":"USD",
#         "sold_amount":"1",
#         "bought_amount":"1"
#     }
#     response = requests.post(SERVER + "trade", headers = {"Authorization" : "Bearer " + api_key}, json = trade)
#     assert response.status_code == 200

        
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

# def test_exchangekey():
#     global api_key
#     keydata = {
#         "exchange":"Kraken",
#         "readkey":"J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU",
#         "secretkey":"VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg=="
#     }
#     response = requests.post(SERVER + "exchangekey", headers = {"Authorization" : "Bearer " + api_key}, json = keydata)
#     assert response.status_code == 200
        
@pytest.fixture(scope="class")
def endpoint_removekey(request):
    keydata = { "exchange":"Kraken" }
    response = requests.post(SERVER + "removekey", headers = {"Authorization" : "Bearer " + api_key}, json = keydata)
    request.cls.response = response

@pytest.mark.usefixtures("endpoint_removekey")
class TestRemovekeyClass:
    def test_response_removekey(self):
        assert self.response.status_code == 200

# def test_removekey():
#     global api_key
#     keydata = { "exchange":"Kraken" }
#     response = requests.post(SERVER + "removekey", headers = {"Authorization" : "Bearer " + api_key}, json = keydata)
#     assert response.status_code == 200


def test_tradepnl():
    global api_key
    trade = {
        "timestamp":"2/02/2020",
        "sold_currency":"BTC",
        "bought_currency":"USD",
        "sold_amount":"1",
        "bought_amount":"1"
    }
    response = requests.post(SERVER + "trade_pnl", headers = {"Authorization" : "Bearer " + api_key}, json = trade)
    assert response.status_code == 200
    pnl = response.json()
    assert float(pnl["pnl"])

def test_portfolio_pnl():
    global api_key
    response = requests.get(SERVER + "portfolio_pnl", headers = {"Authorization" : "Bearer " + api_key})
    assert response.status_code == 200
    pnl = response.json()
    assert float(pnl["pnl"])

# def test_get_annotated_trades():
#     global api_key
#     response = requests.get(SERVER + "get_annotated_trades", headers = {"Authorization" : "Bearer " + api_key})
#     assert response.status_code == 200
#     print(response.json())

# def test_year_end_pnl():
#     global api_key
#     response = requests.get(SERVER + "year_end_stats", headers = {"Authorization" : "Bearer " + api_key})
#     assert response.status_code == 200
#     print(response.json())