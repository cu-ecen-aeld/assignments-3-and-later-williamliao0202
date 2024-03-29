#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    struct thread_data *thread_func_args = (struct thread_data *) thread_param;
    //wait
    int rc = 0;
    if ((rc = usleep(thread_func_args->wait_to_obtain_ms*1000)) != 0)
    {
        ERROR_LOG("Can not sleep for %ud ms, Error Code: %d, %s", thread_func_args->wait_to_obtain_ms, rc, strerror(rc));
        thread_func_args->thread_complete_success = false;
        return thread_param;
    }

    //obtain mutex
    if ((rc = pthread_mutex_lock(thread_func_args->mutex)) != 0)
    {
        ERROR_LOG("Can not lock. Error Code: %d, %s", rc, strerror(rc));
        thread_func_args->thread_complete_success = false;
    }
    else
    {
        //wait
        if ((rc = usleep(thread_func_args->wait_to_release_ms*1000)) != 0)
        {
            ERROR_LOG("Can not sleep for %ud ms, Error Code: %d, %s", thread_func_args->wait_to_release_ms, rc, strerror(rc));
            thread_func_args->thread_complete_success = false;
        }
        //release mute
        if ((rc = pthread_mutex_unlock(thread_func_args->mutex)) != 0)
        {
            ERROR_LOG("Can not release. Error Code: %d, %s", rc, strerror(rc));
            thread_func_args->thread_complete_success = false;
        }
        else
        {
            thread_func_args->thread_complete_success = true;
        }
    }
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
    //allocate memery for thead_data
    struct thread_data *data = (struct thread_data *) malloc(sizeof(struct thread_data));
    if (data == NULL)
    {
        ERROR_LOG("Allocate memory for thread_data failed.");
        return false;
    }
    memset(data, 0, sizeof(struct thread_data));
    data->wait_to_obtain_ms = wait_to_obtain_ms;
    data->wait_to_release_ms = wait_to_release_ms;
    data->mutex = mutex;
    data->thread_complete_success = false;

    int rc = pthread_create(thread, NULL, threadfunc, data);
    if (rc != 0)
    {
        ERROR_LOG("pthread_create failed, Error Code: %d", rc);
        return false;
    }
    return true;
}
