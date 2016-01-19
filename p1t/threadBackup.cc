#include "thread.h"
#include "interrupt.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <queue>

using namespace std;

ucontext_t *ucontext_ptr = NULL;
ucontext_t *prev_ptr = NULL;

int id = 0;

bool lib_call false;

ucontext_t * library_thread;

typedef struct Thread {
    int id;
    ucontext_t * ucontext_ptr;
    bool thread_done;
} Thread;

//each thread has state ready, running, or blocked
// Queue containing all ready threads.
queue<Thread*> ready_threads; //queue for all ready threads

typedef struct wait_queue {
    int lock;
    int cond;
    queue<Thread*> waiting_threads;
} wait_queue;


typedef struct blocked_queue{
int lock;
int cond;
queue<Thread*> blocked_threads;
} blocked_queue;

//THREAD CONTROL BLOCK?
list<wait_queue> queue_list; //list of structs that keeps track of all threads waiting with different lock combos


//Linux provides some library calls (getcontext, makecontext, swapcontext) to help implement user-level thread libraries.
//You will need to read the manual pages for these calls
=======
// Contains all wait_queues on locks and cvs.
list<wait_queue*> wait_queue_list;

int thread_libinit(thread_startfunc_t func, void *arg) {
    if(lib_call == true) {
        return -1;
    }

    lib_call = true;

    thread_create(func, arg);

    library_thread = new ucontext_t;
    getcontext(library_thread);


    while (ready_queue.size() > 0) {
        if (current->finished == true) {
            // Delete current thread.
        }
        current = ready.pop_front();
        // switches between library_thread and the first thread in queue.

        swapcontext(library_thread, current->ucontext_ptr);
    }

    cout << "Thread library exiting.\n";
    exit(0);
}

int thread_create(thread_startfunc_t func, void *arg){

    if(!lib_call){
        return -1;
    }

    Thread * current = new Thread;
    current->ucontext_ptr = new ucontext_t;
    /*
    * Initialize a context structure by copying the current thread's context.
    */
    getcontext(current->ucontext_ptr); // ucontext_ptr has type (ucontext_t *)

    /*
    * Direct the new thread to use a different stack. Your thread library
    * should allocate STACK_SIZE bytes for each thread's stack.
    */
    char * stack = new char [STACK_SIZE];
    current->ucontext_ptr->uc_stack.ss_sp = stack;
    current->ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
    current->ucontext_ptr->uc_stack.ss_flags = 0;
    current->ucontext_ptr->uc_link = NULL;

    if (child.uc_stack.ss_sp == 0) {
        perror("Failed to allocate the stack.");
        delete current->ucontext_ptr;
        delete current;
        return -1;
    }

    /*
    * Direct the new thread to start by calling start(arg1, arg2).
    */
    makecontext(current->ucontext_ptr, (void (*)()) func, 2, arg1, arg2);

/*
    // swapcontext(parent, child);
    // Execute the child's context.
    // Afterward, we need to free the stack
    // free(child.uc_stack.ss_sp);

    // What is this doing...?
    getcontext(prev_ptr);
    makecontext(prev_ptr, (void (*)()) func, 2, arg1, arg2);

    // make new thread to add to queue instead of the global one
    ucontext_t *new_thread = ucontext_ptr;

    ready_threads.push(new_thread);
    return 0;

    */

    current->id = id;
    id++;
    current->thread_done = false;

    ready_queue.push(current);

    return 0;
}

int thread_yield(void) {

    if (!lib_call) {
        return -1;
    }

    if(ready_queue.size() <= 1) {
        return -1;
    } else {
        //swap from current thread to next runnable
        swapcontext(ucontext_ptr, ready_queue.pop());
    }

    return 0;

}

int thread_lock(unsigned int lock){
    //how do we implement a mesa monitor

    if(has_lock(lock)){

    if(!lib_call) {
        return -1;
    }

    return 0;

}


int thread_unlock(unsigned int lock){
    if (!lib_call) {
        return -1;
    }

    return 0;

}

//links thread to condition variable
int thread_wait(unsigned int lock, unsigned int cond){
    if (!lib_call) {
        return -1;
    }

    list<wait_queue>::iterator j;

    for(i = wait_queue_list.begin(); i != wait_queue_list.end();) {
        if(i->lock == lock && i->cond == cond) {
          //  i->wait_queue.push_back()
          //add to queue
        }
    }


    else{
        wait_queue new_queue;
        new_queue.lock = lock;
        new_queue.cond = cond;
        wait_queue_list.push_back(new_queue);

    }

    return 0;

}

int thread_signal(unsigned int lock, unsigned int cond){
    if (!lib_call) {
        return -1;
    }

    list<wait_queue>::iterator j;

    for(i=wait_queue_list.begin(); i != wait_queue_list.end();) {

        if(i->lock == lock && i->cond == cond) {

            wait_queue queue_signaled = *i;
            queue_signaled.waiting_threads.front(); //we don't know what we're doing with queue functions. double checkkkkk
            wait_queue_list.push_back(queue_signaled);

        }
    }

    return 0;

}

int thread_broadcast(unsigned int lock, unsigned int cond){

    if (!lib_call) {
        return -1;
    }

    while(wait_queue_list.size() > 0){
        thread_signal(lock, cond);
    }

    return 0;

}
