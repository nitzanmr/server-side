#include <pthread.h>
#include <stdio.h>

struct {
    int number_of_threads;/*number of active threads <= max size of threads allowed*/
    int qsize;/*the current size of the queue*/
    pthread_t* threads;
    work_t qhead;
    work_t qtail;
    pthread_mutex_t* qlock;/*mutex lock for critical portion*/
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
void do_work(thread_pool new_threadpool){
    pthread_cond_wait(new_threadpool.qsize!=0,new_threadpool.qlock);
    while (new_threadpool.shutdown !=1)
    {
        if(new_threadpool.dont_accept==0){
            pthread_create(new_threadpool.threads[new_threadpool.number_of_threads],NULL,new_threadpool.qhead->routine,new_threadpool.qhead->args);
            new_threadpool.number_of_threads++;
            new_threadpool.qhead = new_threadpool.qhead->next;
            new_threadpool.qsize--;
        }
    }
    
   
}
void dispatch(thread_pool new_threadpool,int (*routine)(void*),void* args){
    /*makes a new work_t and adds it to the work_t linklist in the threadpool given*/
    work_t new_work_t = {routine,args,NULL};
    if(new_threadpool.qhead==NULL){
        new_threadpool.qhead = new_work_t;
        new_threadpool.qtail = new_work_t;
    }
    else{
        new_threadpool.qtail->next = new_work_t;
        new_threadpool.qtail = new_work_t;
    }
}

void create_threadpool(int max_number_of_threads,thread_pool new_threadpool){
    /*creates the threadpool and asign its purpuse to it.*/
    init_threadpool(max_number_of_threads,new_threadpool);
    do_work(new_threadpool);
    
}
void destroy_threadpool(thread_pool thread_pool_destroy){
    thread_pool_destroy.dont_accept =1;
    pthread_cond_wait(thread_pool_destroy.q_not_empty = 1,thread_pool_destroy.qlock);
    thread_pool_destroy.shutdown = 1;
    pthread_cond_signal(thread_pool_destroy.qlock);
    for (int i = 0; i < thread_pool_destroy.number_of_threads; i++)
    {
         pthread_join(thread_pool_destroy.threads[i],NULL);
    }
    
}