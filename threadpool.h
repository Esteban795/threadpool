
/*

/!\ The 'max_queue_size' parameter is temporary until I add in dynamic tabs to allow you
to resize and have as many threads you want.

*/

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>


typedef struct {
    sem_t mutex;
    sem_t* empty;
}  sync_obj;

typedef struct {
    void (*function)(void*);
    void* arg;
} thread_task;

typedef struct {
    thread_task* tasks;
    int head;
    int tail;
    int count;
    int size;
    pthread_mutex_t mutex;
    sem_t sem_empty;
    sem_t sem_full;
    bool shutdown;
    pthread_t* threads;
    int num_threads;
    sync_obj* sync_objects;
    
} thread_pool;


typedef void* ThreadPool;
typedef void* SyncObject;

/*
Creates a ThreadPool instance.


*/
ThreadPool create_thread_pool(int num_threads,int max_queue_size);


void destroy_thread_pool(ThreadPool _pool);


/*
Pushes new task to the thread pool.

- pool : ThreadPool instance
- function : a function pointer to be called by the worker
- arg : paramaters to pass to the function

/!\ To avoid deadlock, make sure to call 'wait_for_sync_object' with its return value
*/
SyncObject queue_task(ThreadPool _pool,void (*function)(void*),void* arg);


/*
Block until the task is done.

- obj : SyncObject instance, that you've gotten from the 'queue_task' function.
*/
void wait_for_sync_object(SyncObject obj);


#endif