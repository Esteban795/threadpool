#include "../headers/threadpool.h"

bool wait_on_sem(sem_t* sem){
    int ret;
    do {
        ret = sem_wait(sem);
    } while (ret == -1 && errno == EINTR); //wait until there is an error or semaphore says we can go
    return ret ? false : true;
}

bool post_sem(sem_t* sem){
    return sem_post(sem) ? false : true;
}


void* worker(void* arg){
    thread_pool* pool = (thread_pool*) arg;

    while (true){
        wait_on_sem(&pool->sem_full);
        pthread_mutex_lock(&pool->mutex);

        if (pool->shutdown){
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(NULL);
        }

        int index = pool->head;
        thread_task task = pool->tasks[index];
        pool->head = (pool->head + 1) % pool->size;
        pool->count--;

        pthread_mutex_unlock(&pool->mutex);

        task.function(task.arg);

        sync_obj* sync_object = &pool->sync_objects[index];
        post_sem(&sync_object->mutex);
    }

    return NULL;
}


ThreadPool create_thread_pool(int num_threads,int max_queue_size){
    thread_pool* pool = malloc(sizeof(thread_pool));
    pool->tasks = malloc(sizeof(thread_task*) * max_queue_size);
    pool->head = 0;
    pool->tail = 0;
    pool->count = 0;
    pool->size = max_queue_size;
    pthread_mutex_init(&pool->mutex,NULL);
    sem_init(&pool->sem_empty,0,max_queue_size);
    sem_init(&pool->sem_full,0,0);
    pool->shutdown = false;
    pool->threads = malloc(sizeof(pthread_t) * num_threads);
    pool->num_threads = num_threads;
    pool->sync_objects = malloc(sizeof(sync_obj) * max_queue_size);

    for (int i = 0; i < max_queue_size;i++){
        sem_init(&pool->sync_objects[i].mutex,0,0);
        pool->sync_objects[i].empty = &pool->sem_empty;
    }

    for (int i = 0; i < num_threads;i++){
        pthread_create(&pool->threads[i],NULL,worker,pool);
    }
    return (ThreadPool)pool;
}

void destroy_thread_pool(ThreadPool _pool){
    thread_pool *pool = (thread_pool*)_pool;
    pool->shutdown = true;

    for(int i = 0; i < pool->num_threads; i++) {
        sem_post(&pool->sem_full);
    }

    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    free(pool->threads);
    free(pool->tasks);

    for (int i = 0; i < pool->size; i++) {
        sem_destroy(&pool->sync_objects[i].mutex);
    }

    free(pool->sync_objects);
    pthread_mutex_destroy(&pool->mutex);
    sem_destroy(&pool->sem_full);
    sem_destroy(&pool->sem_empty);
    free(pool);
}

SyncObject queue_task(ThreadPool _pool,void (*function)(void*),void* arg){
    thread_pool* pool = (thread_pool*)_pool;

    wait_on_sem(&pool->sem_empty);
    pthread_mutex_lock(&pool->mutex);

    if (pool->shutdown){
        pthread_mutex_unlock(&pool->mutex);
        return (SyncObject)NULL;
    }

    int index = pool->tail;
    if (index == pool->size) return (SyncObject)NULL;
    pool->tasks[index].function = function;
    pool->tasks[index].arg = arg;
    pool->tail = (pool->tail + 1) % pool->size;
    pool->count ++;
    pthread_mutex_unlock(&pool->mutex);
    sem_post(&pool->sem_full);
    return (SyncObject)(&pool->sync_objects[index]);
}

void wait_for_sync_object(SyncObject obj){
    sync_obj* sync_object = (sync_obj*)obj;
    if (obj != NULL){
        wait_on_sem(&sync_object->mutex);
        post_sem(sync_object->empty);
    }
}