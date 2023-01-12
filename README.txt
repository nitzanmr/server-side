server-side 
Authored by Nitzan Migdal
206763351

==Description==
This program makes a server with the number of threads asked of it arraied in a threadpool.
and listens to get request and returns the values from withen the files or folders inside the server.
Program DATABASE:
threadpool :
1. int (routine*)(void*) - safe keeping for the function we want the thread to make.
2. arg - the arguments we want to pass the function.
3. next - the next thread available inside the threadpool.

Functions:
server.c:
    error_message- order the error_message and returns the currect error inside of buf.
    
    get_mime_type - gets the path and returns the file type.
    
    split_str - get a char* and splits it by the split_by char given returns array with the amount of elements inside.
    
    read_and_write_file - reads the file asked of it and prints it to the client.
    
    return_wrote_size - returns the length of the data needed to be writen from the file given to the buf.
    
    make_folder_file - makes the data of the file inside a folder given and returns it inside of the buf given.
    
    create_ok - creates the 200 ok messege of the file given. 
    
    accept_client - check for the errors needed inside of the request and then prints the file or the content of the folder given inside of the path.
    
    client_read - reads the content of the request given by the client and sends it to be hendeled by accept_client.
    
    create_server - creates the server and make it listen to the port given inside of the server request. 
    
    server - given 3 args - (port,threadpool_size,max_number_requests).
    
    build_header_m - builds the error messege itself and returns it to error_message inside of the buf given.

threadpool.c:
    init_threadpool- initialize the threadpool in size given and waits for the tasks to begin or to delete the threads.
    
    create_threadpool - creates the threadpool and call the initialize threadpool function.
    
    dispatch - adds the function we want the threads inside the threadpool to do calls one of the threads to handle it at the end.
    
    do_work - the function all the threads make. it is set to take one work from the linklist of work_t and make it and then go back to the end of the line to wait to be called again.
    
    destroy_threadpool - destroies the threadpool by blocking the option to get more assigments insdie the work_t and then when it empties to delete the threadpool.

==Program Files==
    threadpool.c
    threadpool.h
    server.c
    README.txt
==How to compile?==
compile: gcc -o tester main.c threadpool.c server.c threadpool.h -lpthread
run: ./tester (with the <port> <pool-size> <number_of_request_allowed>)

==Input:==
after the ./tester <port> <pool-size> <number_of_request_allowed>
and then need to ask request throgh the client in the website.
==Output:==
will output the request to the client not on the server itself.



