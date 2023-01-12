import os
import subprocess
from subprocess import call
from prettytable import PrettyTable
import requests
import json
import sys
EXECUTABLE = "ex3"
C_FILES = "server.c threadpool.c "
H_FILES = "threadpool.h"   


def check_leaks():
    leaks = True
    with open("valgrind-out.txt", 'r') as valgrind_log:
        try:

            val_lines = valgrind_log.readlines()

        except UnicodeDecodeError:
            return False

        for line in val_lines:
            if "no leaks are possible" in line:
                leaks = False
        if leaks:
            print("valgrind found memory leaks, check your code for allocation/freeing errors and run the tests again")
            return True
    return False


def valgrind_test():
    print("running valgrind with full check and debug mode")

    with open("valgrind-out.txt", 'w') as out_file:
        try:
            valgrind = subprocess.run(
                "valgrind --leak-check=full --tool=memcheck --show-leak-kinds=all --track-origins=yes --verbose "
                f"--error-exitcode=1 -v --log-file=valgrind-out.txt ./{EXECUTABLE} 8090 5 1",
                stdout=out_file, text=True, shell=True, timeout=60)

        except subprocess.TimeoutExpired:
            print("Timeout occurred.")
            return False

    if check_leaks():
        return False

    return valgrind.returncode == 0


def numthreads_bigger_than_tasks():
    status = subprocess.run(f"./{EXECUTABLE} 2",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! test tasks bigger then number of threads")
        return False

    print("[+] Passed test tasks bigger then number of threads")
    return True

def equal():
    status = subprocess.run(f"./{EXECUTABLE} 0",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! Test tasks equal/smaller then number of threads")
        return False

    print("[+] Passed Test tasks equal/smaller then number of threads")
    return True

def zero_threads():
    status = subprocess.run(f"./{EXECUTABLE} 1",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! Zero Threads test")
        return False

    print("[+] Passed  Zero Threads test")
    return True
def zero_tasks():
    status = subprocess.run(f"./{EXECUTABLE} 4",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! zero Tasks test")
        return False

    print("[+] Passed Zero Tasks test")
    return True
def split():
    status = subprocess.run(f"./{EXECUTABLE} 5",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! split test")
        return False

    print("[+] Passed split test")
    return True
def bad_request():
    status = subprocess.run(f"./{EXECUTABLE} 6",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! bad_request test")
        return False

    print("[+] Passed bad_request test")
    return True
def test_read_client():
    status = subprocess.run(f"./{EXECUTABLE} 9",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! read_client test")
        return False

    print("[+] Passed read_client test")
    return True
def create_server():
    # URL = "127.0.0.1"
    # PORT = 8027
  
    
    status = subprocess.run(f"./{EXECUTABLE} 8090 5 1",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! create_server test")
        return False

    print("[+] Passed create_server test")
    return True
def print_content():
    status = subprocess.run(f"./{EXECUTABLE} 10",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! print content test")
        return False

    print("[+] Passed print content test")
    return True
def setup():
    if os.path.isfile(EXECUTABLE):
        os.remove(EXECUTABLE)

    with open("stdout_compilation.txt", 'w') as out_file:
        c = subprocess.run(
            f'gcc -Wall {C_FILES} {H_FILES} -o {EXECUTABLE} -lpthread',
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
    compilation_status = setup()

    if compilation_status == "Error":
        exit(0)
    # t_numthreads_bigger_than_tasks = numthreads_bigger_than_tasks()
    # t_valgrind = valgrind_test()
    # t_equal = equal()
    # t_zero_threads = zero_threads()
    # t_zero_tasks = zero_tasks()
    # t_split = split()
    # t_bad_request = bad_request()
    t_create_server = valgrind_test()
    # t_read_client = test_read_client()
    # t_print_content = print_content()
    t = PrettyTable(['Test', 'Result'])
    t.align['Test'] = 'l'
    # t.add_row(['Tasks > Threads', t_numthreads_bigger_than_tasks])
    # t.add_row(['Tasks <= Threads', t_equal])
    # t.add_row(['Threads = 0', t_zero_threads])
    # t.add_row(['Tasks = 0',t_zero_tasks])
    # t.add_row(['Split ',t_split])
    # t.add_row(['Bad Request',t_bad_request])
    t.add_row(['create_s',t_create_server])
    # t.add_row(['read_client',t_read_client])
    # t.add_row(['valgrind ',t_valgrind])
    # t.add_row(['content',t_print_content])
    print(t)
    



