#include "threadpool.h"
#include <stdio.h>
#include "malloc.h"
#include "stdlib.h"
int job1(void*);
int job2(void*);
#define PASSED_TEST 0
#define FAILED_TEST 1
int main(int argc,char* argv[]){
    int test = FAILED_TEST;
    if(atoi(argv[1]) < atoi(argv[2])){
        if(check_threadpool(atoi(argv[1]),atoi(argv[2]))){
            return 1;
        };
        test = PASSED_TEST;
        return test;
    }
    if(atoi(argv[1]) >= atoi(argv[2])){
        if(check_threadpool(atoi(argv[1]),atoi(argv[2]))){
        };
        test = PASSED_TEST;
        return test;
    }


}
int job1(void* number_of_thread){
    printf("\npthread number: %d\n",number_of_thread);
    return 0;
}
int job2(void* number_of_thread){
    printf("\npthread number: %d\n ",number_of_thread);
    return 0;
}

int check_threadpool(int size_pool,int number_of_tasks){
    threadpool* new_threadpool = create_threadpool(size_pool);
    if(new_threadpool==NULL){
        perror("create error");
        return(1);
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
