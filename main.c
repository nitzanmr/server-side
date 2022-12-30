#include "threadpool.h"
#include <stdio.h>
#include "malloc.h"
#include "stdlib.h"
int job1(void*);
int job2(void*);
int main(){
    int size_pool = 20;
    threadpool* new_threadpool = create_threadpool(size_pool);
    if(new_threadpool==NULL){
        perror("create error");
        exit(1);
    }
    for (int i = 0; i < size_pool; i++)
    {
        if(i%2){
            dispatch(new_threadpool,(dispatch_fn)job2,(void*)i);
        }
        else{
            dispatch(new_threadpool,(dispatch_fn)job1,(void*)i);
        }
    }
    destroy_threadpool(new_threadpool);
}
int job1(void* number_of_thread){
    printf("\npthread number: %d\n",number_of_thread);
    return 0;
}
int job2(void* number_of_thread){
    printf("\npthread number: %d\n ",number_of_thread);
    return 0;
}