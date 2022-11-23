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




