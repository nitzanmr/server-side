import requests
import re
import subprocess
import os
import socket
import time

from prettytable import PrettyTable
from http.client import HTTPConnection

HTTPConnection._http_vsn_str = 'HTTP/1.0'

EXECUTABLE = "ex3"
C_FILES = "server.c threadpool.c"
H_FILES = "threadpool.h"
PORT = 8010


def recvall(s: socket.socket):
    msg = b''
    while True:
        b = s.recv(1024)
        if not b:
            break
        msg += b.lower()

    return msg


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


def valgrind():
    req1 = b"GET /randpath/abc/ghi/mno/pq/uv/z/ HTTP/1.0\r\n\r\n"
    req2 = b"GET /randpath/abc/ HTTP/1.0\r\n\r\n"
    try:
        proc = subprocess.Popen(["valgrind", "--leak-check=full", "--tool=memcheck", "--show-leak-kinds=all",
                                 "--track-origins=yes", "--verbose", "--error-exitcode=1", "-v",
                                 "--log-file=valgrind-out.txt", f"./{EXECUTABLE}", str(PORT), "3", "2"], stderr=subprocess.PIPE)
        time.sleep(3)

        try:
            sock1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock1.settimeout(5)
            sock1.connect(('localhost', PORT))
            sock1.sendall(req1)

        except ConnectionResetError as e:
            err = proc.communicate(timeout=5)
            if 'in use' in str(err):
                raise ConnectionRefusedError(e)
            return False

        sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock2.settimeout(5)
        sock2.connect(('localhost', PORT))
        sock2.sendall(req2)

        resp1 = recvall(sock1)
        resp2 = recvall(sock2)

        proc.wait(timeout=5)

        sock1.close()
        sock2.close()

    except (UnicodeDecodeError, subprocess.TimeoutExpired,
            socket.timeout, ConnectionError) as e:
        proc.terminate()
        return False

    # check that the server exits successfully
    if proc.returncode != 0:
        return False

    if not b'200 OK'.lower() in resp1:
        return False

    if not b'200 OK'.lower() in resp2:
        return False

    if check_leaks():
        return False

    return True


def race_condition():
    valid_req = b'GET /randpath/abc/ HTTP/1.0\r\n'
    found_req = b'GET /randpath/abc/def/mno/found HTTP/1.0\r\n'
    content_req = b'GET /randpath/abc/def/jkl/pq/uv/z/ HTTP/1.0\r\n'
    bad_req = b'GET HTTP/1.0\r\n'
    not_found_req = b'GET /randpath/not/found HTTP/1.0\r\n'

    with open("std_race_condition.txt", "wb") as output:
        try:
            proc = subprocess.Popen(['./ex3', str(PORT), "3", "5"], stderr=subprocess.PIPE)

            time.sleep(3)

            try:
                valid_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                valid_sock.settimeout(5)
                valid_sock.connect(('localhost', PORT))
                valid_sock.sendall(valid_req)

            except ConnectionRefusedError:
                err = proc.communicate(timeout=5)
                valid_sock.close()
                if 'in use' in str(err):
                    raise ConnectionRefusedError
                print("connect\n")
                return False

            found_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            found_sock.settimeout(5)
            found_sock.connect(('localhost', PORT))
            found_sock.sendall(found_req)

            content_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            content_sock.settimeout(5)
            content_sock.connect(('localhost', PORT))
            content_sock.sendall(content_req)

            bad_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            bad_sock.settimeout(5)
            bad_sock.connect(('localhost', PORT))
            bad_sock.sendall(bad_req)

            not_found_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            not_found_sock.settimeout(20)
            not_found_sock.connect(('localhost', PORT))
            not_found_sock.sendall(not_found_req)

            valid_resp = recvall(valid_sock)
            found_resp = recvall(found_sock)
            content_resp = recvall(content_sock)
            bad_resp = recvall(bad_sock)
            not_found_resp = recvall(not_found_sock)

            proc.wait(timeout=5)

            output.write(
                valid_resp + b'\n' 
                + found_resp + b'\n'
                + content_resp + b'\n' 
                + bad_resp + b'\n' + 
                not_found_resp)

        except (UnicodeDecodeError, subprocess.TimeoutExpired,
                socket.timeout, ConnectionError) as e:
            valid_sock.close()
            found_sock.close()
            content_sock.close()
            bad_sock.close()
            not_found_sock.close()
            proc.terminate()
            print("terminated")
            return False

    test_has_succeeded = True

    # check that the server exits successfully
    if proc.returncode != 0:
        print("false\n")
        test_has_succeeded = False

    valid_sock.close()
    found_sock.close()
    content_sock.close()
    bad_sock.close()
    not_found_sock.close()

    if b'HTTP/1.0 200 OK\r\n'.lower() not in valid_resp and b'HTTP/1.1 200 OK\r\n'.lower() not in valid_resp:
        print("<0>\n")
        test_has_succeeded = False
    if b'HTTP/1.1 302 Found\r\n'.lower() not in found_resp and b'HTTP/1.0 302 Found\r\n'.lower() not in found_resp:
        print("<1>\n")
        test_has_succeeded = False
    if b'HTTP/1.0 200 OK\r\n'.lower() not in content_resp and b'HTTP/1.1 200 OK\r\n'.lower() not in content_resp:
        print("<2>\n")
        test_has_succeeded = False
    if b'HTTP/1.1 400 Bad Request\r\n'.lower() not in bad_resp and b'HTTP/1.0 400 Bad Request\r\n'.lower() not in bad_resp:
        print("<3>\n")
        test_has_succeeded = False
    if b'HTTP/1.1 404 Not Found\r\n'.lower() not in not_found_resp and b'HTTP/1.0 404 Not Found\r\n'.lower() not in not_found_resp:
        print("<4>\n")
        test_has_succeeded = False

    return test_has_succeeded


def send_image():
    my_request = b'GET /randpath/abc/ghi/mno/pq/uv/z/meow.jpg HTTP/1.0\r\n'

    with open("stdout_image.txt", "wb") as output:
        try:
            proc = subprocess.Popen(['./ex3', str(PORT), "1", "1"], stderr=subprocess.PIPE)

            time.sleep(3)

            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect(('localhost', PORT))
            sock.sendall(my_request)

            msg = recvall(sock)

            proc.wait(timeout=5)
            output.write(msg)

        except (UnicodeDecodeError, subprocess.TimeoutExpired,
                socket.timeout, ConnectionResetError, ConnectionRefusedError) as e:
            if isinstance(e, ConnectionRefusedError):
                err = proc.communicate(timeout=5)
                if 'in use' in str(err):
                    raise ConnectionRefusedError(e)
            sock.close()
            proc.terminate()
            return False

    test_has_succeeded = True

    # check that the server exits successfully
    if proc.returncode != 0:
        test_has_succeeded = False

    sock.close()

    # check headers
    if b'HTTP/1.0 200 OK\r\n'.lower() not in msg and b'HTTP/1.1 200 OK\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Server: webserver/1.0\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Date:'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Type: image/jpeg\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Length: 254259\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Connection: close\r\n\r\n'.lower() not in msg:
        test_has_succeeded = False

    # check that all bytes has been received
    img_file = open("randpath/abc/ghi/mno/pq/uv/z/meow.jpg", "rb")
    img = img_file.read().lower()

    if img not in msg:
        test_has_succeeded = False

    img_file.close()

    return test_has_succeeded


def forbidden():
    my_request = b'GET /randpath/abc/def/jkl/pq/rst/y/no_permissions.html HTTP/1.0\r\n'

    with open("stdout_forbidden.txt", "wb") as output:
        try:
            proc = subprocess.Popen(['./ex3', str(PORT), "1", "1"], stderr=subprocess.PIPE)

            time.sleep(3)

            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect(('localhost', PORT))
            sock.sendall(my_request)

            msg = recvall(sock)

            proc.wait(timeout=5)
            output.write(msg)

        except (UnicodeDecodeError, subprocess.TimeoutExpired,
                socket.timeout, ConnectionResetError, ConnectionRefusedError) as e:
            if isinstance(e, ConnectionRefusedError):
                err = proc.communicate(timeout=5)
                if 'in use' in str(err):
                    raise ConnectionRefusedError(e)
            sock.close()
            proc.terminate()
            return False

    test_has_succeeded = True

    # check that the server exits successfully
    if proc.returncode != 0:
        test_has_succeeded = False

    sock.close()

    c_len = msg.find(b'\r\n\r\n')
    if c_len == -1:
        return False

    c_len = c_len + 4
    c_len_header = f'Content-Length: {len(msg[c_len:])}\r\n'.lower()

    # check headers
    if b'HTTP/1.1 403 Forbidden\r\n'.lower() not in msg and b'HTTP/1.0 403 Forbidden\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Server: webserver/1.0\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Date:'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Type: text/html\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Length: 111\r\n'.lower() not in msg and c_len_header.encode() not in msg:
        test_has_succeeded = False
    if b'Connection: close\r\n\r\n'.lower() not in msg:
        test_has_succeeded = False

    # check body
    body1 = "<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\n" \
            "<BODY><H4>403 Forbidden</H4>\n" \
            "Access denied.\n" \
            "</BODY></HTML>"

    body2 = b"<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\r\n" \
            b"<BODY><H4>403 Forbidden</H4>\r\n" \
            b"Access denied.\r\n" \
            b"</BODY></HTML>"

    if body1.lower() not in str(msg) and body2.lower() not in msg:
        test_has_succeeded = False

    return test_has_succeeded


def dir_content():
    my_request = b'GET /randpath/abc/def/jkl/pq/uv/z/ HTTP/1.0\r\n'

    with open("stdout_dir_content.txt", "wb") as output:
        try:
            proc = subprocess.Popen(['./ex3', str(PORT), "1", "1"], stderr=subprocess.PIPE)

            time.sleep(3)

            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect(('localhost', PORT))
            sock.sendall(my_request)

            msg = recvall(sock)

            proc.wait(timeout=5)
            output.write(msg)

        except (UnicodeDecodeError, subprocess.TimeoutExpired,
                socket.timeout, ConnectionResetError, ConnectionRefusedError) as e:
            if isinstance(e, ConnectionRefusedError):
                err = proc.communicate(timeout=5)
                if 'in use' in str(err):
                    raise ConnectionRefusedError(e)
            sock.close()
            proc.terminate()
            return False

    test_has_succeeded = True

    # check that the server exits successfully
    if proc.returncode != 0:
        test_has_succeeded = False

    sock.close()

    # check content length
    match = re.search(b"content-length: ([0-9]*)", msg)
    if match is None:
        print("<5>\n")
        return False

    # check the correct size
    c_len = match.groups()[0]
    end_of_headers = msg.find(b'\r\n\r\n') + 4
    if c_len != b'' and int(c_len) != len(msg[end_of_headers:]):
        print("<4> \n")
        print(len(msg[end_of_headers:]))
        test_has_succeeded = False

    # check body and head
    if msg.find(b'<HEAD><TITLE>Index of /randpath/abc/def/jkl/pq/uv/z/</TITLE></HEAD>'.lower()) == -1 and \
            msg.find(b'<HEAD><TITLE>Index of ./randpath/abc/def/jkl/pq/uv/z/</TITLE></HEAD>'.lower()) == -1 and \
            msg.find(b'<HEAD><TITLE>Index of randpath/abc/def/jkl/pq/uv/z/</TITLE></HEAD>'.lower()) == -1:
        print("<6>\n")
        test_has_succeeded = False

    # check table rows
    start_trs = msg.count(b'<tr>')
    end_trs = msg.count(b'</tr>')

    # check for the correct number of rows
    if start_trs != end_trs:
        print("<0>\n")       
        test_has_succeeded = False

    if (start_trs != 11) and (start_trs != 13):
        print("<1>\n")
        test_has_succeeded = False

    # check names
    for index in range(1, 9):
        
        if str(msg).find(f'<td><A HREF="{index}">{index}</a></td>'.lower()) == -1:
            test_has_succeeded = False
            print("%d\n",index)
            break

    # check one file with size
    if msg.find(b'<td><A HREF="not_empty.txt">not_empty.txt</a></td>'.lower()) == -1 or \
            msg.find(b'<td>10</td>') == -1:
        print("<3>\n")
        test_has_succeeded = False

    return test_has_succeeded


def found():
    my_request = b'GET /randpath/abc/def/mno/found HTTP/1.0\r\n'

    with open("stdout_found.txt", "wb") as output:
        try:
            proc = subprocess.Popen(['./ex3', str(PORT), "1", "1"], stderr=subprocess.PIPE)

            time.sleep(3)

            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect(('localhost', PORT))
            sock.sendall(my_request)

            msg = recvall(sock)

            proc.wait(timeout=5)
            output.write(msg)

        except (UnicodeDecodeError, subprocess.TimeoutExpired,
                socket.timeout, ConnectionResetError, ConnectionRefusedError) as e:
            if isinstance(e, ConnectionRefusedError):
                err = proc.communicate(timeout=5)
                if 'in use' in str(err):
                    raise ConnectionRefusedError(e)
            sock.close()
            proc.terminate()
            return False

    test_has_succeeded = True

    # check that the server exits successfully
    if proc.returncode != 0:
        test_has_succeeded = False

    sock.close()

    c_len = msg.find(b'\r\n\r\n')
    if c_len == -1:
        return False

    c_len = c_len + 4
    c_len_header = f'Content-Length: {len(msg[c_len:])}\r\n'.lower()

    # check headers
    if b'HTTP/1.1 302 Found\r\n'.lower() not in msg and b'HTTP/1.0 302 Found\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Server: webserver/1.0\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Date:'.lower() not in msg:
        test_has_succeeded = False
    if b'Location: /randpath/abc/def/mno/found/'.lower() not in msg and \
            b'Location: ./randpath/abc/def/mno/found/'.lower() not in msg and \
            b'Location: randpath/abc/def/mno/found/'.lower() not in msg and \
            b'Location: /randpath/abc/def/mno/found\\'.lower() not in msg and \
            b'Location: ./randpath/abc/def/mno/found\\'.lower() not in msg and \
            b'Location: randpath/abc/def/mno/found\\'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Type: text/html\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Length: 123\r\n'.lower() not in msg and c_len_header.encode() not in msg:
        test_has_succeeded = False
    if b'Connection: close\r\n\r\n'.lower() not in msg:
        test_has_succeeded = False

    # check body
    body1 = "<HTML><HEAD><TITLE>302 Found</TITLE></HEAD>\n" \
            "<BODY><H4>302 Found</H4>\n" \
            "Directories must end with a slash.\n" \
            "</BODY></HTML>"

    body2 = b"<HTML><HEAD><TITLE>302 Found</TITLE></HEAD>\r\n" \
            b"<BODY><H4>302 Found</H4>\r\n" \
            b"Directories must end with a slash.\r\n" \
            b"</BODY></HTML>"

    if body1.lower() not in str(msg) and body2.lower() not in msg:
        test_has_succeeded = False

    return test_has_succeeded


def not_supported():
    my_request = b'POST /index.html HTTP/1.0\r\n'

    with open("stdout_not_supported.txt", "wb") as output:
        try:
            proc = subprocess.Popen(['./ex3', str(PORT), "1", "1"], stderr=subprocess.PIPE)

            time.sleep(3)

            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect(('localhost', PORT))
            sock.sendall(my_request)

            msg = recvall(sock)

            proc.wait(timeout=5)
            output.write(msg)

        except (UnicodeDecodeError, subprocess.TimeoutExpired,
                socket.timeout, ConnectionResetError, ConnectionRefusedError) as e:
            if isinstance(e, ConnectionRefusedError):
                err = proc.communicate(timeout=5)
                if 'in use' in str(err):
                    raise ConnectionRefusedError(e)
            sock.close()
            proc.terminate()
            return False

    test_has_succeeded = True

    # check that the server exits successfully
    if proc.returncode != 0:
        test_has_succeeded = False

    sock.close()

    c_len = msg.find(b'\r\n\r\n')
    if c_len == -1:
        return False

    c_len = msg.find(b'\r\n\r\n')
    if c_len == -1:
        return False

    c_len = c_len + 4
    c_len_header = f'Content-Length: {len(msg[c_len:])}\r\n'.lower()

    # check headers
    if b'HTTP/1.1 501 Not supported\r\n'.lower() not in msg and b'HTTP/1.0 501 Not supported\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Server: webserver/1.0\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Date:'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Type: text/html\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Length: 129\r\n'.lower() not in msg and c_len_header.encode() not in msg:
        test_has_succeeded = False
    if b'Connection: close\r\n\r\n'.lower() not in msg:
        test_has_succeeded = False

    # check body
    body1 = "<HTML><HEAD><TITLE>501 Not supported</TITLE></HEAD>\n" \
            "<BODY><H4>501 Not supported</H4>\n" \
            "Method is not supported.\n" \
            "</BODY></HTML>"

    body2 = b"<HTML><HEAD><TITLE>501 Not supported</TITLE></HEAD>\r\n" \
            b"<BODY><H4>501 Not supported</H4>\r\n" \
            b"Method is not supported.\r\n" \
            b"</BODY></HTML>"

    if body1.lower() not in str(msg) and body2.lower() not in msg:
        test_has_succeeded = False

    return test_has_succeeded


def bad_request():
    my_request = b'GET HTTP/1.0\r\n'

    with open("stdout_bad_request.txt", "wb") as output:
        try:
            proc = subprocess.Popen(['./ex3', str(PORT), "1", "1"], stderr=subprocess.PIPE)

            time.sleep(3)

            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect(('localhost', PORT))
            sock.sendall(my_request)

            msg = recvall(sock)

            proc.communicate(timeout=5)
            output.write(msg)

        except (UnicodeDecodeError, subprocess.TimeoutExpired,
                socket.timeout, ConnectionResetError, ConnectionRefusedError) as e:
            if isinstance(e, ConnectionRefusedError):
                err = proc.communicate(timeout=5)
                if 'in use' in str(err):
                    raise ConnectionRefusedError(e)
            sock.close()
            proc.terminate()
            return False

    test_has_succeeded = True

    # check that the server exits successfully
    if proc.returncode != 0:
        test_has_succeeded = False

    sock.close()

    c_len = msg.find(b'\r\n\r\n')
    if c_len == -1:
        return False

    c_len = c_len + 4
    c_len_header = f'Content-Length: {len(msg[c_len:])}\r\n'.lower()

    # check headers
    if b'HTTP/1.1 400 Bad Request\r\n'.lower() not in msg and b'HTTP/1.0 400 Bad Request\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Server: webserver/1.0\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Date:'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Type: text/html\r\n'.lower() not in msg:
        test_has_succeeded = False
    if b'Content-Length: 113\r\n'.lower() not in msg and c_len_header.encode() not in msg:
        test_has_succeeded = False
    if b'Connection: close\r\n\r\n'.lower() not in msg:
        test_has_succeeded = False

    # check body
    body1 = "<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>\n" \
            "<BODY><H4>400 Bad request</H4>\n" \
            "Bad Request.\n" \
            "</BODY></HTML>"

    body2 = b"<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>\r\n" \
            b"<BODY><H4>400 Bad request</H4>\r\n" \
            b"Bad Request.\r\n" \
            b"</BODY></HTML>"

    if body1.lower() not in str(msg) and body2.lower() not in msg:
        test_has_succeeded = False

    return test_has_succeeded


def usage():
    with open("usage_stdout.txt", "w") as output:
        try:
            out = subprocess.run(["./ex3", "5000", "0"],
                                 stdout=output,
                                 timeout=5)
            if out.returncode != 0:
                return False

            out = subprocess.run(["./ex3"],
                                 stdout=output,
                                 timeout=5)

            if out.returncode != 0:
                return False

        except (subprocess.TimeoutExpired, UnicodeDecodeError) as e:
            return False

    with open("usage_stdout.txt", "r") as output:
        out = output.read().lower()
        matches = re.findall(r"usage:", out)
        if len(matches) != 2:
            return False

    return True


def setup():
    if os.path.isfile(EXECUTABLE):
        os.remove(EXECUTABLE)

    with open("stdout_compilation.txt", 'w') as out_file:
        c = subprocess.run(
            f'gcc -g -Wall threadpool.h {C_FILES} -o {EXECUTABLE} -lpthread',
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


if __name__ == '__main__':
    compilation_status = setup()

    if compilation_status == "Error":
        exit(0)

    subprocess.run(['chmod', '-r', 'randpath/abc/def/jkl/pq/rst/y/no_permissions.html'])
    # t_usage = usage()
    # PORT = PORT + 1
    # t_bad_request = bad_request()
    # PORT = PORT + 1
    # t_not_supported = not_supported()
    # PORT = PORT + 1
    # t_found = found()
    # PORT = PORT + 1
    # t_dir_content = dir_content()
    # PORT = PORT + 1
    # t_forbidden = forbidden()
    # PORT = PORT + 1
    # t_send_img = send_image()
    # PORT = PORT + 1
    # t_race_condition = race_condition()
    # PORT = PORT + 1
    t_valgrind = valgrind()

    t = PrettyTable(['Test', 'Result'])
    t.align['Test'] = 'l'
    # t.add_row(['Usage', t_usage])
    # t.add_row(['Bad Request', t_bad_request])
    # t.add_row(['Not Supported', t_not_supported])
    # t.add_row(['Found', t_found])
    # t.add_row(['Dir Content', t_dir_content])
    # t.add_row(['Forbidden', t_forbidden])
    # t.add_row(['Send Image', t_send_img])
    # t.add_row(['Race Condition', t_race_condition])
    t.add_row(['Valgrind', t_valgrind])
    print(t)
