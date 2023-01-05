import requests
import json
def send_client_http():
    x = requests.get("http://localhost:8080")
    print(x.json)
    return
if __name__ == "__main__":
    send_client_http()
    