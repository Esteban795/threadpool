# Threadpool, made in C.

<br />

### __A lightweight, general purpose threadpool, in C.__

<br/>
- High performance threadpool. <br/>
- Tasks can be individually synchronized with synchronization objects (making it faster than using the standard lib 'future'.)


<br />
<br />

### __Requirements__

- No requirements : only a C compiler, and C version above C99.

<br/>

### __How to install it on your computer ?__

- Clone the project in the folder you'd like to use it, using :
```ps
git clone https://github.com/Esteban795/threadpool.git
``` 

- After that, all you need to do is including the header file (which is located in `headers/threadpool.h`), or simply copy and paste the file in your own folder. Pick one !

<br/>

### /!\ It should work just fine on Linux and Windows. I haven't tested it on MacOS, since I don't own a computer that has it.

<br/>
<br/>


### __Usage__ : 

- A quick example does wonders.

```c
#include "headers/threadpool.h"

void func(void* arg){
    int n = (int)arg;
    printf("The number is : %d",n);
}

int main(void){
    ThreadPool pool = create_thread_pool(16,128); // numbers of threads, and max queue size for tasks.
    SyncObject sync_obj[128];
    for (int i = 0; i < 128;i++){
        sync_obj[i] = queue_task(pool,func,i); // queue task with the given 'func', and 'i' as an argument.
    }
    for (int i = 0; i < 128; i++){
        wait_for_sync_object(sync_obj[i]); // mandatory to avoid deadlocks. Else, cannot guarantee it.
    }

    destroy_thread_pool(pool); //destroys the thread pool.
    return 0;
}
```

- Here, we create a thread pool with 16 threads (the first parameter passed to the `create_thread_pool` function.) and at most 128 tasks (second parameter). This is a current limitation of the implementation that I'm working on, using dynamic arrays to allow you to add as much tasks as you want !

- We then create tasks for our threads to work on : at most max queue size.
- We then wait for them to sync all.
- We end up destroying the thread pool, and freeing everything that was used by the thread pool. 

<br/>
<br/>

### __What's left to do ?__ 
- Really, the only thing that I need to work on is removing the limit of synchronized objects using a dynamic tab.

<br/>
<br/>
<br/>
<br/>

## Thanks!