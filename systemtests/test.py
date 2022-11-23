import requests

service = "http://0.0.0.0:18420/"

response = requests.get(service + "getcredentials")
print(response)
print(response.json())

authenticatedUser = response.json()

client_id = authenticatedUser["client_id"]
api_key = authenticatedUser["api_key"]
refresh_token = authenticatedUser["refresh_token"]

response = requests.get(service, headers = {"Authorization" : "Bearer " + api_key})
print(response)