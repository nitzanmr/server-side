#include "threadpool.c"
int job1(int);
int main(){
    int size_pool = 5;
    threadpool* new_threadpool = create_threadpool(size_pool);
    if(new_threadpool==NULL){
        perror("create error");
        exit(1);
    }
    for (size_t i = 0; i < size_pool; i++)
    {
        dispatch(new_threadpool,(dispatch_fn)job1,(void*)i);
    }
}
int job1(int number_of_thread){
    printf("pthread number: %d",number_of_thread);
    return 0;
}