import os
import subprocess
from subprocess import call
from prettytable import PrettyTable

EXECUTABLE = "ex3"
C_FILES = "main.c threadpool.c "
H_FILES = "threadpool.h"   

def numthreads_bigger_than_tasks():
    status = subprocess.run(f"./{EXECUTABLE} 3 6",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! test tasks bigger then number of threads")
        return False

    print("[+] Passed test tasks bigger then number of threads")
    return True

def equal():
    status = subprocess.run(f"./{EXECUTABLE} 5 5",shell=True)
    if status.returncode  != 0:
        print("[-]FAILED! test tasks equal/smaller then number of threads")
        return False

    print("[+] Passed test tasks equal/smaller then number of threads")
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
    t_numthreads_bigger_than_tasks = numthreads_bigger_than_tasks()
    t_equal = equal()
    t = PrettyTable(['Test', 'Result'])
    t.align['Test'] = 'l'
    t.add_row(['Tasks > Threads', t_numthreads_bigger_than_tasks])
    t.add_row(['Tasks <= Threads', t_equal])
    print(t)



