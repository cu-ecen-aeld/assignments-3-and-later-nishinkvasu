#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
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
        ERROR_LOG("pthread_create failed with rc = %d", &rc);
        return false;
    }


    return true;
}


int main(int argc, char **argv)
{
    // pthread_t thread;
    // pthread_mutex_t mutex;
    // bool thread_started = false;
    // TEST_ASSERT_EQUAL_INT_MESSAGE(0,pthread_mutex_init(&mutex, NULL),
    //                 "pthread_mutex_init should succeed");
    // TEST_ASSERT_EQUAL_INT_MESSAGE(0,pthread_mutex_lock(&mutex),
    //                 "pthread_mutex_lock should succeed");
    // printf("Start a thread obtaining a locked mutex, sleeping 1 millisecond before locking and waiting to return\n");
    // printf("until 1 millisecond after locking.\n");
    // thread_started = start_thread_obtaining_mutex(&thread, &mutex, 1, 1);
    // TEST_ASSERT_TRUE_MESSAGE(thread_started,
    //             "start_thread_obtaining_mutex should start a new thread with locked mutex");
    // if (thread_started) {
    //     validate_thread_waits_for_mutex(&thread,&mutex,1,1);
    // }
        
    // TEST_ASSERT_EQUAL_INT_MESSAGE(0, pthread_mutex_destroy(&mutex),
    //         "The mutex should be able to be destroyed succesfully at the conclusion of the test");

// void UnityAssertEqualNumber(const UNITY_INT expected,
//                             const UNITY_INT actual,
//                             const char* msg,
//                             const UNITY_LINE_TYPE lineNumber,
//                             const UNITY_DISPLAY_STYLE_T style)
// {
//     RETURN_IF_FAIL_OR_IGNORE;

//     if (expected != actual)
//     {
//         UnityTestResultsFailBegin(lineNumber);
//         UnityPrint(UnityStrExpected);
//         UnityPrintNumberByStyle(expected, style);
//         UnityPrint(UnityStrWas);
//         UnityPrintNumberByStyle(actual, style);
//         UnityAddMsgIfSpecified(msg);
//         UNITY_FAIL_AND_BAIL;
//     }
// }    
}

