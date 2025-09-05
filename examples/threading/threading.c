#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG_ENABLE
// Optional: use these functions to add debug or error prints to your application
#ifndef DEBUG_ENABLE
#define DEBUG_LOG(msg,...)
#else
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#endif
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

#define UNITY_TEST_ASSERT_EQUAL_INT(expected, actual, line, message) TestAssertEqualNumber((int)(expected), (int)(actual), (message), (unsigned int)(line))
#define TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message)    UNITY_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, (message))

#define UNITY_TEST_ASSERT(condition, line, message)     do {if (condition) {} else {printf("%s %d",(message),(line));}} while(0)
#define TEST_ASSERT_TRUE_MESSAGE(condition, message)    UNITY_TEST_ASSERT((condition), __LINE__, (message))



void TestAssertEqualNumber(const int expected,
                            const int actual,
                            const char* msg,
                            const unsigned int lineNumber)
{
    if (expected != actual)
    {
        printf("Expected and actual do not match");
        printf("%s /n",msg);
    }
}

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


// int main(int argc, char **argv)
// {
//     pthread_t thread;
//     pthread_mutex_t mutex;
//     bool thread_started = false;

//     void * thread_rtn = NULL;
//     TEST_ASSERT_EQUAL_INT_MESSAGE(0,pthread_mutex_init(&mutex, NULL),
//                     "pthread_mutex_init should succeed");
//     // TEST_ASSERT_EQUAL_INT_MESSAGE(0,pthread_mutex_lock(&mutex),
//     //                 "pthread_mutex_lock should succeed");
//     printf("Start a thread obtaining a locked mutex, sleeping 1 millisecond before locking and waiting to return\n");
//     printf("until 1 millisecond after locking.\n");
//     thread_started = start_thread_obtaining_mutex(&thread, &mutex, 1, 1);
//     TEST_ASSERT_TRUE_MESSAGE(thread_started,
//                 "start_thread_obtaining_mutex should start a new thread with locked mutex");
//     if (thread_started) {
//         // validate_thread_waits_for_mutex(&thread,&mutex,1,1);
//         DEBUG_LOG("Thread started successfully");
//     }
//     usleep(1000000);
//     int rc = pthread_join(thread, &thread_rtn);
//     if( rc != 0)
//         DEBUG_LOG("join failed");

//     if(thread_rtn != NULL)
//     {
//         struct thread_data* thread_rtndata = (struct thread_data*)thread_rtn;
//         if(thread_rtndata->thread_complete_success == true)
//             DEBUG_LOG("Thread complete status Success!");
//         else
//             DEBUG_LOG("Thread complete status failed");
//         free(thread_rtn);

//     }
//     else
//         DEBUG_LOG("No thread return data");

//     rc = 0;
//     // pthread_mutex_unlock(&mutex);
//     rc = pthread_mutex_destroy(&mutex);    
//     if( rc != 0)
//         DEBUG_LOG("destroy mutex failed");   
//     // TEST_ASSERT_EQUAL_INT_MESSAGE(0, pthread_mutex_destroy(&mutex),
//     //         "The mutex should be able to be destroyed succesfully at the conclusion of the test");

// }

