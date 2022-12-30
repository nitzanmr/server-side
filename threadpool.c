#include <stdio.h>
#include "malloc.h"
#include "stdlib.h"
#include "threadpool.h"
int number_of_threads_asked = 0;
void init_threadpool(int max_number_of_threads,threadpool* new_threadpool){
    /*this function initialize the threadpool to it defult values*/
   
    number_of_threads_asked = max_number_of_threads;
    new_threadpool->num_threads = 0;
    new_threadpool->qsize = 0;
    new_threadpool->threads = (pthread_t*)malloc(max_number_of_threads*sizeof(pthread_t));
    new_threadpool->dont_accept=0;
    new_threadpool->shutdown = 0;
    pthread_cond_init(&new_threadpool->q_not_empty,NULL);
    pthread_cond_init(&new_threadpool->q_empty,NULL);
    pthread_mutex_init(&new_threadpool->qlock,NULL);
    new_threadpool->qhead = NULL;
    new_threadpool->qtail = NULL;
    for(int i =0;i<max_number_of_threads;i++){
        if(pthread_create(&(new_threadpool->threads[i]),NULL,do_work,new_threadpool)){
            perror("create failed");
            exit(1);
        }
        printf("pthread number:%d created\n",i);
    }
}
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
     if(num_threads_in_pool <= 0 || num_threads_in_pool > MAXT_IN_POOL){
        perror("too many or too few threads wanted");
        return NULL;
    }
    threadpool* new_threadpool = (threadpool*)malloc(sizeof(threadpool));
    init_threadpool(num_threads_in_pool,new_threadpool);
    /*need to address what happens if the return is null from init*/
    return new_threadpool;
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
 */
void dispatch(threadpool* from_me, dispatch_fn dispatch_to_here, void *arg){
    work_t* new_work_t = (work_t*)malloc(sizeof(work_t));
    new_work_t->arg =arg;
    new_work_t->routine = dispatch_to_here;
    new_work_t->next = NULL;
    // dispatch_to_here(arg);
    // new_work_t.routine(new_work_t.arg);
    // (&new_work_t)->routine((&new_work_t)->arg);
    if(from_me->dont_accept==0){
        pthread_mutex_lock(&from_me->qlock);
        if(from_me->qhead==NULL){
            from_me->qhead = new_work_t;
            from_me->qtail = new_work_t;
        }
        else{
            if(from_me->qhead==from_me->qtail){
                from_me->qhead->next = new_work_t;
                from_me->qtail = new_work_t;
            }
            else{
                from_me->qtail->next = new_work_t;
                from_me->qtail = new_work_t;
        
            }
        }
        from_me->qsize++;
        pthread_mutex_unlock(&from_me->qlock);
        pthread_cond_signal(&from_me->q_not_empty);
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
    
    work_t* temp_work;
    while (((threadpool*)p)->shutdown !=1)
    {
        pthread_mutex_lock(&((threadpool*)p)->qlock);
        if(((threadpool*)p)->qsize==0){
            if(((threadpool*)p)->qsize==0&& ((threadpool*)p)->dont_accept == 1&&((threadpool*)p)->num_threads==0){
                pthread_cond_signal(&((threadpool*)p)->q_empty);
                printf("%d",((threadpool*)p)->shutdown);
                printf("\npthread exits\n");
                pthread_mutex_unlock(&((threadpool*)p)->qlock);
                pthread_exit(NULL);
            }
            // pthread_cond_signal(&((threadpool*)p)->q_empty);
            pthread_cond_wait(&((threadpool*)p)->q_not_empty,&((threadpool*)p)->qlock);

        }
        if(((threadpool*)p)->shutdown==1){
            pthread_mutex_unlock(&((threadpool*)p)->qlock);
            // pthread_cond_signal(&)
            printf("\npthread exits\n");
            pthread_exit(NULL);
        }
        ((threadpool*)p)->num_threads++;
        temp_work = ((threadpool*)p)->qhead;
        ((threadpool*)p)->qhead = ((threadpool*)p)->qhead->next;
        ((threadpool*)p)->qsize--;
        printf("the queue size is: %d\n",((threadpool*)p)->qsize);
       
        
        pthread_mutex_unlock(&((threadpool*)p)->qlock);
        temp_work->routine(temp_work->arg);
        free(temp_work);
        ((threadpool*)p)->num_threads--;
        
    }
    // pthread_mutex_unlock(&((threadpool*)p)->qlock);
    // pthread_cond_signal(&)
    printf("\npthread exits\n");
    pthread_exit(NULL);
};


/**
 * destroy_threadpool kills the threadpool, causing
 * all threads in it to commit suicide, and then
 * frees all the memory associated with the threadpool.
 */
void destroy_threadpool(threadpool* destroyme){
    destroyme->dont_accept = 1;
    pthread_mutex_lock(&destroyme->qlock);
    pthread_cond_wait(&destroyme->q_empty,&destroyme->qlock);
    destroyme->shutdown = 1;
    pthread_mutex_unlock(&destroyme->qlock);

    // pthread_cond_broadcast(&destroyme->q_not_empty);
    for (int i = 0; i < number_of_threads_asked; i++)
    {
        pthread_cond_signal(&destroyme->q_not_empty);
        printf("pthread joined\n");
        pthread_join(destroyme->threads[i],NULL);
    }
    return;
};


