#include <pthread.h>
#include <stdio.h>
#include "malloc.h"

void init_threadpool(int max_number_of_threads,threadpool new_threadpool){
    /*this function initialize the threadpool to it defult values*/
    new_threadpool.num_threads = 0;
    new_threadpool.qsize = 0;
    new_threadpool.threads = NULL;
    new_threadpool.dont_accept=0;
    new_threadpool.shutdown = 0;
    new_threadpool.q_not_empty = 0;
    new_threadpool.qhead = NULL;
    new_threadpool.qtail = NULL;
}   
void do_work(threadpool new_threadpool){
    pthread_cond_wait(new_threadpool.qsize!=0,&new_threadpool.qlock);
    while (new_threadpool.shutdown !=1)
    {
        if(new_threadpool.dont_accept==0){
            pthread_create(new_threadpool.threads[new_threadpool.num_threads],NULL,new_threadpool.qhead->routine,new_threadpool.qhead->args);
            new_threadpool.num_threads++;
            new_threadpool.qhead = new_threadpool.qhead->next;
            new_threadpool.qsize--;
        }
    }
    
   
}
void dispatch(thread_pool new_threadpool,int (*routine)(void*),void* args){
    /*makes a new work_t and adds it to the work_t linklist in the threadpool given*/
    
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
    for (int i = 0; i < thread_pool_destroy.num_threads; i++)
    {
         pthread_join(thread_pool_destroy.threads[i],NULL);
    }
    
}
#include <pthread.h>

/**
 * threadpool.h
 *
 * This file declares the functionality associated with
 * your implementation of a threadpool.
 */

// maximum number of threads allowed in a pool
#define MAXT_IN_POOL 200


/**
 * the pool holds a queue of this structure
 */
typedef struct work_st{
      int (*routine) (void*);  //the threads process function
      void * arg;  //argument to the function
      struct work_st* next;  
} work_t;


/**
 * The actual pool
 */
typedef struct _threadpool_st {
 	int num_threads;	//number of active threads
	int qsize;	        //number in the queue
	pthread_t *threads;	//pointer to threads
	work_t* qhead;		//queue head pointer
	work_t* qtail;		//queue tail pointer
	pthread_mutex_t qlock;		//lock on the queue list
	pthread_cond_t q_not_empty;	//non empty and empty condidtion vairiables
	pthread_cond_t q_empty;
      int shutdown;            //1 if the pool is in distruction process     
      int dont_accept;       //1 if destroy function has begun
} threadpool;


// "dispatch_fn" declares a typed function pointer.  A
// variable of type "dispatch_fn" points to a function
// with the following signature:
// 
//     int dispatch_function(void *arg);

typedef int (*dispatch_fn)(void *);

/**
 * create_threadpool creates a fixed-sized thread
 * pool.  If the function succeeds, it returns a (non-NULL)
 * "threadpool", else it returns NULL.
 * this function should:
 * 1. input sanity check 
 * 2. initialize the threadpool structure
 * 3. initialized mutex and conditional variables
 * 4. create the threads, the thread init function is do_work and its argument is the initialized threadpool. 
 */
threadpool* create_threadpool(int num_threads_in_pool){
    threadpool* new_threadpool = (threadpool*)malloc(sizeof(threadpool));
    init_threadpool(num_threads_in_pool,new_threadpool);
    do_work(new_threadpool);

};


/**
 * dispatch enter a "job" of type work_t into the queue.
 * when an available thread takes a job from the queue, it will
 * call the function "dispatch_to_here" with argument "arg".
 * this function should:
 * 1. create and init work_t element
 * 2. lock the mutex
 * 3. add the work_t element to the queue
 * 4. unlock mutex
 *
 */
void dispatch(threadpool* from_me, dispatch_fn dispatch_to_here, void *arg){
    work_t new_work_t = {dispatch_to_here,arg,NULL};
    if(from_me->qhead==NULL){
        from_me->qhead = &new_work_t;
        from_me->qtail = &new_work_t;
    }
    else{
        from_me->qtail->next = &new_work_t;
        from_me->qtail = &new_work_t;
    }
};

/**
 * The work function of the thread
 * this function should:
 * 1. lock mutex
 * 2. if the queue is empty, wait
 * 3. take the first element from the queue (work_t)
 * 4. unlock mutex
 * 5. call the thread routine
 *
 */
void* do_work(void* p){
    pthread_cond_wait(((threadpool*)p)->qsize!=0,&((threadpool*)p)->qlock);
    while (((threadpool*)p)->shutdown !=1)
    {
        if(((threadpool*)p)->dont_accept==0){
            pthread_create(((threadpool*)p)->threads[((threadpool*)p)->num_threads],NULL,((threadpool*)p)->qhead->routine,((threadpool*)p)->qhead->args);
            ((threadpool*)p)->num_threads++;
            ((threadpool*)p)->qhead = ((threadpool*)p)->qhead->next;
            ((threadpool*)p)->qsize--;
        }
    }
};


/**
 * destroy_threadpool kills the threadpool, causing
 * all threads in it to commit suicide, and then
 * frees all the memory associated with the threadpool.
 */
void destroy_threadpool(threadpool* destroyme);


