#include <pthread.h>
#include <stdio.h>

struct {
    int number_of_threads;/*number of active threads <= max size of threads allowed*/
    int qsize;/*the current size of the queue*/
    pthread_t* threads;
    work_t qhead;
    work_t qtail;
    pthread_mutex_t qlock;/*mutex lock for critical portion*/
    int q_not_empty;/*flags*/
    int shutdown;
    int dont_accept;
    }typedef thread_pool;

    struct work_t{
        int (*routine)(void*);
        void* args;
        struct work_t* next;
    }typedef work_t;

void init_threadpool(int max_number_of_threads,thread_pool new_threadpool){
    /*this function initialize the threadpool to it defult values*/
    new_threadpool.number_of_threads = 0;
    new_threadpool.qsize = 0;
    new_threadpool.threads = NULL;
    new_threadpool.dont_accept=0;
    new_threadpool.shutdown = 0;
    new_threadpool.q_not_empty = 0;
    new_threadpool.qhead = NULL;
    new_threadpool.qtail = NULL;
}   
void create_threadpool(int max_number_of_threads,thread_pool new_threadpool){
    /*creates the threadpool and asign its purpuse to it.*/
    init_threadpool(max_number_of_threads,new_threadpool);
    for (int i = 0; i <max_number_of_threads ;i++){
        pthread_create(new_threadpool.threads[i],NULL,new_threadpool.qhead->routine,new_threadpool.qhead->args);
        
    }
    
}
