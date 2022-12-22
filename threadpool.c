struct thread_pool{
    int number_of_threads;/*max size of threads allowed*/
    int qsize;/*the current size of the queue*/
    work_t qhead;
    work_t qtail;
    pthread_mutex qlock;/*mutex lock for critical portion*/
    int q_not_empty;/*flags*/
    int shutdown;
    int dont_accept;
    }typedef thread_pool;

    struct work_t{
        int (*routine)(void*);
        void* args;
        struct work_t* next;
    }typedef work_t;
    
