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
#define PASSED_TEST 0
#define FAILED_TEST 1
#define EQUAL 0
#define ZEROTH 1
#define THREAD 2
#define TASKS 3
#define ZEROTA 4
#define SPLIT 5
int main(int argc,char* argv[]){
    /*argv = [number_threads,number_tasks]*/
    int test = FAILED_TEST;
    if(atoi(argv[1])==EQUAL)equal();
    if(atoi(argv[1])==ZEROTH)zero_threads();
    if(atoi(argv[1])==THREAD)threads_larger();
    if(atoi(argv[1])==TASKS)tasks_larger();
    if(atoi(argv[1])==ZEROTA)zero_tasks();
    if(atoi(argv[1])==SPLIT)split();
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
