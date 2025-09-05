#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// #define DEBUG_ENABLE
// Optional: use these functions to add debug or error prints to your application
#ifndef DEBUG_ENABLE
#define DEBUG_LOG(msg,...)
#else
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#endif
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    DEBUG_LOG("in the created thread");

    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    // wait before obtain
    usleep(thread_func_args->wait_to_obtain_ms * 1000);
    DEBUG_LOG("About to lock");

    // obtain mutex
    pthread_mutex_lock(thread_func_args->mutex);
    // wait before release
    DEBUG_LOG("After lock");

    usleep(thread_func_args->wait_to_release_ms * 1000);
    // release mutex
    pthread_mutex_unlock(thread_func_args->mutex);
    DEBUG_LOG("After unlock");


    thread_func_args->thread_complete_success = true;
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    //allocate memory for thread data
    struct thread_data* pstthreaddata = (struct thread_data*)malloc(sizeof(struct thread_data));
    if(pstthreaddata == NULL)
    {
        ERROR_LOG("Allocate memory failed for thread data");
        return false;
    }

    //setup mutex and wait arguments
    pstthreaddata->mutex = mutex;
    pstthreaddata->wait_to_obtain_ms = wait_to_obtain_ms;
    pstthreaddata->wait_to_release_ms = wait_to_release_ms;
    pstthreaddata->thread_complete_success = false;

    //create thread with above thread_data
    int rc = pthread_create(thread, NULL, threadfunc, pstthreaddata);
    if(rc != 0){
        ERROR_LOG("pthread_create failed with rc = %d", rc);
        return false;
    }

    return true;
}


