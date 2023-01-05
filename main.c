#include "server.c"
#include <stdio.h>
#include "malloc.h"
#include "stdlib.h"
// #include "server.c"
int job1(void*);
int job2(void*);
int check_threadpool(int,int);
int equal();
int zero_threads();
int zero_tasks();
int threads_larger();
int tasks_larger();
int split();
int bad_request();
int create_s();
#define PASSED_TEST 0
#define FAILED_TEST 1
#define EQUAL 0
#define ZEROTH 1
#define THREAD 2
#define TASKS 3
#define ZEROTA 4
#define SPLIT 5
#define BADR 6 //bad request
#define CREATES 7 // create server
#define ACCEPT 8 
#define CLIENTR 9
int main(int argc,char* argv[]){
    /*argv = [number_threads,number_tasks]*/
    int test = FAILED_TEST;
    if(atoi(argv[1])==EQUAL)equal();
    if(atoi(argv[1])==ZEROTH)zero_threads();
    if(atoi(argv[1])==THREAD)threads_larger();
    if(atoi(argv[1])==TASKS)tasks_larger();
    if(atoi(argv[1])==ZEROTA)zero_tasks();
    if(atoi(argv[1])==SPLIT)split();
    if(atoi(argv[1])==BADR)bad_request();
    if(atoi(argv[1]) == CREATES)create_s();
    // if(atoi(argv[1])==ACCEPT)accept();
    if(atoi(argv[1])==CLIENTR)client_read2();
}
int equal(){
    if(check_threadpool(5,5)){
            return FAILED_TEST;
        };
    return PASSED_TEST;
}
int zero_threads(){
     if(check_threadpool(0,5)){
            return PASSED_TEST;
        };
    return FAILED_TEST;
}
int zero_tasks(){
    if(check_threadpool(5,0)){
            return PASSED_TEST;
        };
    return FAILED_TEST;
}
int threads_larger(){
    if(check_threadpool(6,5)){
            return FAILED_TEST;
        };
    return PASSED_TEST;
}
int tasks_larger(){
    if(check_threadpool(3,6)){
        return FAILED_TEST;
        };
    return PASSED_TEST;
}
int split(){
    char check[] = "GET /google.com HTTP/1.1";
    char* correct_split[] = {"GET","/google.com","HTTP/1.1"};
    char* split_by = " ";
    char* split_res[3];
    split_str(check,split_by,split_res);
    printf("\n after asgining\n");
    for (int i = 0; i < 3; i++)
    {
        if(!strcmp(correct_split[i],split_res[i])){
            return FAILED_TEST;
        }
    }
    // free(check);
    // free(split_res);
    return PASSED_TEST;
}
int bad_request(){
    char* args[] ={"GET","/path"} ;
    if(server(2,args)>0){
        return PASSED_TEST;
    }
    return FAILED_TEST;

}
int create_s(){
    char* args[] = {"8070","3","3"};
    if(server(3,args)==0)return 0;
    else return 1;
}
// int accept(){
//     char request[] = "8080 /check_file HTTP/1.0";
//     char returned_value[512];
//     accept_client(request,returned_value,)
// }
int client_read2(){
    int fd;
    char buf[] = "GET / HTTP/1.0";
    struct sockaddr_in new_socket;
    fd = socket(AF_INET,SOCK_STREAM,0);
    new_socket.sin_addr.s_addr = INADDR_ANY;
    new_socket.sin_port = htonl(8080);
    new_socket.sin_family = AF_INET;
    connect(fd,(SA*)&new_socket,sizeof(new_socket));
    write(fd,buf,strlen(buf));

    return 0;
}
int job1(void* number_of_thread){
    printf("\ntask number: %d\n",number_of_thread);
    return 0;
}
int job2(void* number_of_thread){
    printf("\ntask number: %d\n ",number_of_thread);
    return 0;
}

int check_threadpool(int size_pool,int number_of_tasks){
    threadpool* new_threadpool = create_threadpool(size_pool);
    if(new_threadpool==NULL){
        perror("create error");
        return 1;
    }
    for (int i = 0; i < number_of_tasks; i++)
    {
        if(i%2){
            dispatch(new_threadpool,(dispatch_fn)job2,(void*)i);
        }
        else{
            dispatch(new_threadpool,(dispatch_fn)job1,(void*)i);
        }
    }
    destroy_threadpool(new_threadpool);
    return 0;
}
