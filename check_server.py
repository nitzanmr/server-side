import requests
import json
import subprocess
import os

EXECUTABLE = "ex2"
C_FILES = "client.c "

def send_client_http():
    status = subprocess.run(f"./{EXECUTABLE} http://localhost:8080",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! split test")
        return False

    print("[+] Passed split test")
    return True
    # from http.client import HTTPConnection
    # HTTPConnection._http_vsn_str = 'HTTP/1.0'
    # x = requests.get("http://localhost:8080/check_file")
    # print(x.json)
    
    # status = subprocess.run(f"./{EXECUTABLE} 9",shell=True)
    # if status.returncode  != 0:
    #     print("[-]FAILED! test tasks bigger then number of threads")
    #     return False

    # print("[+] Passed test tasks bigger then number of threads")
    # return True

def setup():
    if os.path.isfile(EXECUTABLE):
        os.remove(EXECUTABLE)

    with open("stdout_compilation.txt", 'w') as out_file:
        c = subprocess.run(
            f'gcc -Wall {C_FILES} -o {EXECUTABLE} ',
            stderr=out_file,
            stdout=out_file,
            shell=True,
        )
    with open("stdout_compilation.txt") as out_file:
        res = out_file.read()
        return_val = None
        if bytes(res, 'utf-8') == b'':
            print("Ex. compiled successfully.")
            return_val = "Compiled"

        if "warning: " in res:
            print("Warnings during compilation")
            return_val = "Warnings"

        if "error: " in res:
            print("\nSomething didn't go right when compiling your C source "
                "please check stdout_compilation.txt\n")
            return_val = "Error"

        return return_val

if __name__ == "__main__":
    setup()
    send_client_http()
    