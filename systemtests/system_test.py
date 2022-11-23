import requests
import pytest

SERVER = "http://0.0.0.0:18420/"

def test_getcredentials():
    response = requests.get(SERVER + "getcredentials")

    assert response.status_code == 200

    authenticatedUser = response.json()

    global client_id
    global api_key
    global refresh_token
    client_id = authenticatedUser["client_id"]
    api_key = authenticatedUser["api_key"]
    refresh_token = authenticatedUser["refresh_token"]

    assert client_id == api_key[0 : 4] == refresh_token[0 : 4]

    assert requests.get(SERVER, headers = {"Authorization" : "Bearer " + api_key}).status_code == 200

def test_refreshcredentials():
    global client_id
    global api_key
    global refresh_token
    
    response = requests.get(SERVER + "refreshcredentials", headers = {"Authorization" : "Bearer " + refresh_token})

    assert response.status_code == 200

    authenticatedUser = response.json()

    
    api_key = authenticatedUser["api_key"]
    refresh_token = authenticatedUser["refresh_token"]

    assert client_id == authenticatedUser["client_id"] == api_key[0 : 4] == refresh_token[0 : 4]

    assert requests.get(SERVER, headers = {"Authorization" : "Bearer " + api_key}).status_code == 200


def test_trade():
    global api_key
    trade = {
        "timestamp":"2/02/2020",
        "sold_currency":"BTC",
        "bought_currency":"USD",
        "sold_amount":"1",
        "bought_amount":"1"
    }
    response = requests.post(SERVER + "trade", headers = {"Authorization" : "Bearer " + api_key}, json = trade)
    assert response.status_code == 200


def test_exchangekey():
    global api_key
    keydata = {
        "exchange":"Kraken",
        "readkey":"J9LYvgnqF6wG4H0Y7/Yr1ysdXke/O2vPdu58nQGp9bmK+e7R4OSyWAsU",
        "secretkey":"VzP5UOZzOcsjJOw/9gU1D1QY78eBNO3LHnXIUngXxK7jbhy58EHpPOGI8b7CIg4D/304BOAwrxX5JwGVECJimg=="
    }
    response = requests.post(SERVER + "exchangekey", headers = {"Authorization" : "Bearer " + api_key}, json = keydata)
    assert response.status_code == 200

def test_removekey():
    global api_key
    keydata = { "exchange":"Kraken" }
    response = requests.post(SERVER + "removekey", headers = {"Authorization" : "Bearer " + api_key}, json = keydata)
    assert response.status_code == 200


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

