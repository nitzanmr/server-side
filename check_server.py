import requests
import json
import subprocess
import os
import urllib3
import telnetlib
import re
import threading
import sys
import multiprocessing
import time
EXECUTABLE = "ex2"
C_FILES = "client.c "

def check_main_folder():
    # status = subprocess.run(f"telnet 127.0.0.1 8070","GET /folder/ HTTP/1.1",shell=True)
    # subprocess.run(f"GET /folder/ HTTP/1.1",shell=True)
    # HOST = "127.0.0.1"
    # PORT = "8070"
    # tn = telnetlib.Telnet(HOST,PORT)
    # # tn.mt_interact()
    # tn.write(b'GET /folder/ HTTP/1.1')
    # tn.write(b"GET /folder/ HTTP/1.1")
    # tn.write(b"GET /check_file.txt HTTP/1.1")
    time.sleep(5)
    URL = 'http://127.0.0.1:80/'
    r = requests.get(URL,timeout=1)
    print(r.text)
    
   
def check_named_folder():
    time.sleep(5)
    URL = 'http://127.0.0.1:80/folder/'
    r = requests.get(URL,timeout=1)
    print(r.text)
def check_file():
    time.sleep(5)
    URL = 'http://127.0.0.1:80/check_file'
    r = requests.get(URL, timeout=1)
    print(r.text)


if __name__ == "__main__":
    # setup()
   
    child = subprocess.Popen([sys.executable, './tester.py', '--username', 'root'])
    process1 = multiprocessing.Process(target=check_main_folder())
    process2 = multiprocessing.Process(target=check_named_folder())
    process3 = multiprocessing.Process(target=check_file())
    process1.start()
    process2.start()
    process3.start()
# class myClass( wx.Frame ):
#     def __init__(self):
#         self.child = subprocess.Popen([sys.executable, './child.py', '--username', 'root'])
#     def onClose( self , evt ):
#         self.child.wait()
#         self.exit();