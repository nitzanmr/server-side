import requests
import json
import subprocess
import os
import urllib3
import telnetlib
import re
EXECUTABLE = "ex2"
C_FILES = "client.c "

def send_client_http():
    # status = subprocess.run(f"telnet 127.0.0.1 8070","GET /folder/ HTTP/1.1",shell=True)
    # subprocess.run(f"GET /folder/ HTTP/1.1",shell=True)
    HOST = "127.0.0.1"
    PORT = 8070
    tn = telnetlib.Telnet(HOST,PORT)
    tn.write(b'GET /folder/ HTTP/1.1')
    # tn.write("GET /folder/ HTTP/1.1")
    # tn.write("GET /check_file.txt HTTP/1.1")

if __name__ == "__main__":
    # setup()
    send_client_http()
    