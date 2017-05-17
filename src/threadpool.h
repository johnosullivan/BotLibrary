
#ifndef _THREADPOOL_

#define _THREADPOOL_


#include <pthread.h>

typedef enum {
    threadpool_invalid= -1,
    threadpool_memory_failure= -2,
    threadpool_lock_failure= -3,
    threadpool_queue_full= -4,
    threadpool_is_shutdown= -5,
    threadpool_thread_failure= -6,
    threadpool_pop_failure= -7,
    threadpool_signal_failure= -8
} threadpool_error_t;


typedef enum {
    threadpool_shutdown_type=1
} threadpool_shutdown;




// basic structure for job, job_queue, thread_pool

typedef struct threadpool_job_t{
    void* (*function)(void *arg);
    void *arg;
    struct threadpool_job_t *prev;
    struct threadpool_job_t *next;
} threadpool_job_t;


typedef struct threadpool_job_queue {
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_cond_t notify_full;
    threadpool_job_t *head;
    threadpool_job_t *tail;
    int job_done;
    int job_count;
} threadpool_job_queue;





typedef struct threadpool_t{
    pthread_t *threads;
    int thread_num;
    threadpool_job_queue * job_queue;
    int shutdown;
} threadpool_t;



/******************
 function for thread pool
 ******************/



threadpool_t * threadpool_init(int thread_num);



int threadpool_add_job(threadpool_t *tp, void *(*function)(void *), void *arg);



int threadpool_destroy(threadpool_t *tp);




/******************
 function for job queue
 ******************/



int threadpool_job_queue_init(threadpool_t *tp);


void threadpool_job_queue_add(threadpool_t* tp, threadpool_job_t* newjob);


int threadpool_job_queue_pop_back(threadpool_t* tp);


threadpool_job_t* threadpool_job_queue_back(threadpool_t* tp);


void threadpool_job_queue_destroy(threadpool_t* tp);

#endif 
















