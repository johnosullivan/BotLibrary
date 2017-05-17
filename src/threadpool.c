#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "threadpool.h"



/**************************************************************************
 function for thread pool
 **************************************************************************/

int debug=0;



static void * threadpool_thread(void * tp);


// initialize the thread pool

threadpool_t * threadpool_init(int thread_num ) {
    
    threadpool_t * tp=(threadpool_t *)malloc(sizeof(threadpool_t));
    
    if (tp==NULL) {
        fprintf(stderr,"Cannot allocate memory for the new thread pool");
        return NULL;
    }
    
    tp->threads=(pthread_t *)malloc(sizeof(pthread_t)*thread_num);
    
    if (tp->threads==NULL) {
        fprintf(stderr,"Cannot allocate memory for the worker threads");
        return NULL;
    }
    
    tp->thread_num=thread_num;
    
    if (threadpool_job_queue_init(tp)==-1) {
        fprintf(stderr,"Cannot allocate memory for the job queue");
        return NULL;
    }
    
    if((pthread_mutex_init(&(tp->job_queue->lock), NULL) != 0) || (pthread_cond_init(&(tp->job_queue->notify), NULL) != 0) || (pthread_cond_init(&(tp->job_queue->notify_full), NULL)) ) {
        fprintf(stderr,"Cannot initialize lock and condition variable");
        return NULL;
    }
    
    tp->shutdown=0;
    
    for (int i=0; i<thread_num; i++){
        printf("Created thread %d in pool \n", i);
        pthread_create(&(tp->threads[i]), NULL , threadpool_thread , (void*)tp );
    }
    return tp;
}




// add job to the queue

int threadpool_add_job( threadpool_t *tp, void *(*function)(void*) , void* arg ) {
    threadpool_job_t * job=(threadpool_job_t *)malloc(sizeof(threadpool_job_t));
    
    if (job==NULL) {
        fprintf(stderr,"Thread pool cannot allocate memory for the new job");
        return threadpool_memory_failure;
    }
    job->function=function;
    job->arg=arg;
    
    pthread_mutex_lock(&tp->job_queue->lock);
    
//    if (tp->job_queue->job_count==tp->job_queue->queue_size) {
//        printf("there is no vacancies\n");
//        pthread_cond_wait(&(tp->job_queue->notify_full),&(tp->job_queue->lock));
//    }
    
    
    threadpool_job_queue_add(tp,job);
    
    if (1) {
        fprintf(stdout,"after threadpool_add_job, the job_count: %d\n",tp->job_queue->job_count);
    }
    
    pthread_mutex_unlock(&tp->job_queue->lock);
    
    return 0;
}




// destroy the thread pool

int threadpool_destroy( threadpool_t *tp ) {
    int flag=0;
    tp->shutdown=1;
    
    pthread_mutex_lock(&tp->job_queue->lock);
    
    if ( tp->job_queue->job_count!=0) {
        pthread_cond_wait(&tp->job_queue->notify,&tp->job_queue->lock);
    }
    
    do {
        
        // is shutting down
        if(tp->shutdown) {
            flag = threadpool_is_shutdown;
            break;
        }
        
        
        // Wake up all worker threads to break the infinit loop
        if((pthread_cond_broadcast(&(tp->job_queue->notify)) != 0) ||
           (pthread_mutex_unlock(&(tp->job_queue->lock)) != 0)) {
            flag = threadpool_lock_failure;
            if (debug) {
                printf("destroy failure when wake up all worker threads\n");
            }
            break;
        }
        
        // Join all worker thread
        for(int i = 0; i < tp->thread_num; i++) {
            if(pthread_join(tp->threads[i], NULL) != 0) {
                flag = threadpool_thread_failure;
            }
        }
    } while(0);
    
    printf(" %d jobs have been done\n",tp->job_queue->job_done);
    
    if (!flag) {
        threadpool_job_queue_destroy(tp);
        
        free(tp->threads);
        
        free(tp->job_queue);
        
        
    }
    
    free(tp);
    
    return 0;
}



static void * threadpool_thread( void * threadpool ) {
    threadpool_t *tp = (threadpool_t *)threadpool;
    threadpool_job_t *job;
    void* (*function)(void *);
    void * arg;
    
    for (;;) {
        
        
        
        
        
        
        if (pthread_mutex_lock(&(tp->job_queue->lock))!=0) {
            printf("cannot get lock\n");
        }
        
        
        
        if (1) {
            printf("threadpool_thread: %lu\n",(unsigned long int)pthread_self());
        }
        
        
        printf(" thread %lu check the job_count: %d\n",(unsigned long int)pthread_self(), tp->job_queue->job_count);
        
        while(tp->job_queue->job_count==0) {
            if (debug) {
                fprintf(stdout,"there is no pending job\n");
            }
            pthread_cond_wait(&(tp->job_queue->notify), &(tp->job_queue->lock));
            printf("thread %lu after waiting for the job, job_count: %d\n",(unsigned long int)pthread_self(),tp->job_queue->job_count);
        }
        

        
        
        
        if(tp->shutdown==1) {
            break;
        }
        
        if (debug) {
            fprintf(stdout,"ready to execute\n");
        }
    
        job=threadpool_job_queue_back(tp);
        
        if (job==NULL) {
            printf("no job\n");
        }
            
        function=job->function;
        arg=job->arg;
        threadpool_job_queue_pop_back(tp);
        pthread_mutex_unlock(&tp->job_queue->lock);
        function(arg);
        free(job);
    }
    return tp;
}



/************************************************************************************
 function for job queue
 ************************************************************************************/






// initialize the queue

int threadpool_job_queue_init(threadpool_t *tp) {
    threadpool_job_queue * tjq=(threadpool_job_queue *)malloc(sizeof(threadpool_job_queue));
    if (tjq==NULL) {
        return -1;
    }
    tjq->head=NULL;
    tjq->tail=NULL;
    tjq->job_count=0;
    tjq->job_done=0;
    tp->job_queue=tjq;
    
    return 0;
}





// add job to the queue

void threadpool_job_queue_add(threadpool_t* tp, threadpool_job_t* job) {
    
    threadpool_job_queue *job_queue=tp->job_queue;
    threadpool_job_t *temp_job=job_queue->head;
    
    job->prev=job->next=NULL;
    
    if (debug) {
        printf("job_queue add job: %d\n",job_queue->job_count);
    }
    switch (job_queue->job_count) {
        case 0:
            job_queue->head=job;
            job_queue->tail=job;
            break;
        default:
            temp_job->prev=job;
            job->next=temp_job;
            job_queue->head=job;
            break;
    }
    ++job_queue->job_count;
    ++job_queue->job_done;
    if (job_queue->job_count==1) {
        pthread_cond_signal(&(job_queue->notify));
    }
    
}






// pop the tail in the queue

int threadpool_job_queue_pop_back(threadpool_t* tp) {
    int flag=0;
    threadpool_job_queue *job_queue=tp->job_queue;
    threadpool_job_t *job=job_queue->tail;
    
    if (debug) {
        printf("job_queue pop back: %d\n",job_queue->job_count);
    }
    switch (job_queue->job_count) {
        case 0:
            if (debug) {
                printf("job queue pop failure");
            }
            return threadpool_pop_failure;
        case 1:
            job_queue->head=NULL;
            job_queue->tail=NULL;
            break;
        default:
            job->prev->next=NULL;
            job_queue->tail=job->prev;
            break;
    }
    
    --job_queue->job_count;
    
    if (1) {
        printf("After pop back job, the job_count: %d\n", job_queue->job_count);
    }
    
    
//    if (job_queue->job_count+1==job_queue->queue_size) {
//        if(pthread_cond_signal(&(job_queue->notify_full)) != 0) {
//            flag=threadpool_signal_failure;
//            printf("signal notify_full failure");
//        }
//        printf("new vancancy !!\n");
//    }
    return flag;
}







// access the tail in the queue

threadpool_job_t* threadpool_job_queue_back(threadpool_t* tp) {
    
    return tp->job_queue->tail;
}






// delete all the jobs in the queue
// destory the queue
// used for deleting the pool


void threadpool_job_queue_destroy(threadpool_t* tp) {
    threadpool_job_t * job=tp->job_queue->tail;
    
    
    
    while(tp->job_queue->job_count) {
        tp->job_queue->tail=job->prev;
        free(job);
        job=tp->job_queue->tail;
        --tp->job_queue->job_count;
    }
    tp->job_queue->head=tp->job_queue->tail=NULL;
    
    pthread_mutex_lock(&tp->job_queue->lock);
    pthread_mutex_destroy(&tp->job_queue->lock);
    pthread_cond_destroy(&tp->job_queue->notify);
    
}








