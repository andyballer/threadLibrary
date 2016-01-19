#include "thread.h"
#include "interrupt.h"
#include <ucontext.h>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <deque>

using namespace std;

/*

To run this:
g++ -o deli thread.cc deli.cc libinterrupt.a -ldl

./deli 3 sw.in0 sw.in1 sw.in2 sw.in3 sw.in4

*/

ucontext_t *ucontext_ptr = NULL;

int id = 0;

bool lib_call = false;

ucontext_t * library_thread;

typedef struct Thread {
    int id;
    ucontext_t * ucontext_ptr;
    bool thread_done;
} Thread;

deque<Thread*> ready_queue;

Thread * current;

typedef struct wait_queue {
    unsigned int lock;
    unsigned int cond;
    deque<Thread*> * waiting_threads;
} wait_queue;

list<wait_queue*> wait_queue_list;

typedef struct Lock {
    unsigned int lock;
    Thread * owner;
    deque<Thread*> * blocked_queue;
} Lock;

list<Lock*> locked_list;

int start(thread_startfunc_t func, void * arg) {
    interrupt_enable();
    func(arg);
    interrupt_disable();
    current->thread_done = true;
    swapcontext(current->ucontext_ptr, library_thread);
    return 0;
}

int thread_create_nointerrupts(thread_startfunc_t func, void *arg){
    if(!lib_call) {
        return -1;
    }
    Thread * thread;
    char* stack;
    try {
        thread = new Thread;
        thread->ucontext_ptr = new ucontext_t;
        /*
        * Initialize a context structure by copying the current thread's context.
        */
        getcontext(thread->ucontext_ptr);

        stack = new char [STACK_SIZE];
        thread->ucontext_ptr->uc_stack.ss_sp = stack;
        thread->ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
        thread->ucontext_ptr->uc_stack.ss_flags = 0;
        thread->ucontext_ptr->uc_link = NULL;

        makecontext(thread->ucontext_ptr, (void (*)()) start, 2, func, arg);

        thread->id = id;
        id++;
        thread->thread_done = false;

        ready_queue.push_back(thread);
    } catch (std::bad_alloc b) {
        delete stack;
        delete thread->ucontext_ptr;
        delete thread;

        return -1;
    }
    return 0;
}

int thread_lock_nointerrupts(unsigned int lock){
    if(!lib_call) {
        return -1;
    }

    //locked_thread = new ucontext_t*;
    //getcontext(locked_thread); //now if anything is swapped, it swaps to this locked threads context, doesnt change

    list<Lock*>::iterator i;

    bool lock_found = false;

    Lock* current_lock;

    for (i = locked_list.begin(); i != locked_list.end(); i++) {
        current_lock = *i;

        if (current_lock->lock == lock) {

            lock_found = true;

            if (current_lock->owner == NULL) {
                current_lock->owner = current;
            } else {
                if (current_lock->owner->id == current->id) {
                    return -1;
                }
                else {
                    current_lock->blocked_queue->push_back(current);
                    swapcontext(current->ucontext_ptr, library_thread);
                }
            }
        }
    }

    if (!lock_found) {
        // Creating new lock.
        try {
            current_lock = new Lock;
            current_lock->lock = lock;
            current_lock->owner = current;
            current_lock->blocked_queue = new deque<Thread*>;
            locked_list.push_back(current_lock);
        } catch (std::bad_alloc b) {
            delete current_lock->blocked_queue;
            delete current_lock;
            return -1;
        }
    }

    return 0;
}

int thread_unlock_nointerrupts(unsigned int lock){
    if (!lib_call) {
        return -1;
    }

    list<Lock*>::iterator i;

    Lock* current_lock;

    bool lock_found = false;

    for (i = locked_list.begin(); i != locked_list.end(); i++) {

        current_lock = *i;

        if (current_lock->lock == lock) {

            lock_found = true;

            // The lock has no owner.
            if (current_lock->owner == NULL) {
                return -1;
            }

            // The lock has a different owner.
            if (current_lock->owner->id != current->id) {
                return -1;

                // You're the owner of the lock!!
            }else {

                // There are threads blocked on the lock.
                if (current_lock->blocked_queue->size() > 0) {

                    current_lock->owner = current_lock->blocked_queue->front();
                    current_lock->blocked_queue->pop_front();

                    ready_queue.push_back(current_lock->owner);
                }
                    // No threads are blocked on the lock.
                else {
                    current_lock->owner = NULL;
                }
            }

        }
    }

    if (!lock_found) {
        return -1;
    }

    return 0;
}

int thread_libinit(thread_startfunc_t func, void *arg) {
    interrupt_disable();

    if(lib_call == true) {
        interrupt_enable();
        return -1;
    }
    lib_call = true;

    // Putting the first thread on the queue.
    if (thread_create_nointerrupts(func, arg) == -1) {
        interrupt_enable();
        return -1;
    }

    current = ready_queue.front();
    ready_queue.pop_front();
    try {
        library_thread = new ucontext_t;
    } catch(std::bad_alloc b) {
        delete library_thread;
        interrupt_enable();
        return -1;
    }
    getcontext(library_thread);

    swapcontext(library_thread, current->ucontext_ptr);

    while (ready_queue.size() > 0) {

        if (current->thread_done == true) {
            delete (char *) current->ucontext_ptr->uc_stack.ss_sp;
	    current->ucontext_ptr->uc_stack.ss_sp = NULL;
            current->ucontext_ptr->uc_stack.ss_size = 0;
            delete current->ucontext_ptr;
            delete current;
            current = NULL;
        }

        current = ready_queue.front();
        ready_queue.pop_front();
        if (current != NULL) {
            swapcontext(library_thread, current->ucontext_ptr);
        } else {
            break;
        }
        //cout << "sweet" << endl;
    }

    cout << "Thread library exiting.\n";
    exit(0);
}

int thread_create(thread_startfunc_t func, void *arg) {
    interrupt_disable();
    int i = thread_create_nointerrupts(func, arg);
    interrupt_enable();
    return i;
}

int thread_yield(void) {
    interrupt_disable();F
    if (!lib_call) {
        interrupt_enable();
	return -1;
    }
    if(ready_queue.size() < 1) {
        interrupt_enable();
        return 0;
    } else {
        //Thread * next = ready_queue.front();
        //ready_queue.pop_front();
        ready_queue.push_back(current);
        swapcontext(current->ucontext_ptr, library_thread);
    }

    interrupt_enable();

    return 0;
}

int thread_lock(unsigned int lock){
    interrupt_disable();
    int i = thread_lock_nointerrupts(lock);
    interrupt_enable();
    return i;
}

int thread_unlock(unsigned int lock){
    interrupt_disable();
    int i = thread_unlock_nointerrupts(lock);
    interrupt_enable();
    return i;
}

int thread_wait(unsigned int lock, unsigned int cond){
    interrupt_disable();
    if (!lib_call) {
        interrupt_enable();
        return -1;
    }

    if (thread_unlock_nointerrupts(lock) == -1) {
        interrupt_enable();
        return -1;
    }

    wait_queue* cur_wait_queue;

    bool found_cv = false;

    list<wait_queue*>::iterator j;

    for(j = wait_queue_list.begin(); j != wait_queue_list.end(); j++) {

        cur_wait_queue = *j;

        if(cur_wait_queue->lock == lock && cur_wait_queue->cond == cond) {
            found_cv = true;

            cur_wait_queue->waiting_threads->push_back(current);
        }
    }

    if (!found_cv) {
        // Make a new CV.
        wait_queue* new_wait;
        try {
            new_wait = new wait_queue;
            new_wait->lock = lock;
            new_wait->cond = cond;

            new_wait->waiting_threads = new deque<Thread*>;
            new_wait->waiting_threads->push_back(current);

            wait_queue_list.push_back(new_wait);
        } catch (std::bad_alloc b) {
            delete new_wait->waiting_threads;
            delete new_wait;
            interrupt_enable();
            return -1;
        }

    }

    swapcontext(current->ucontext_ptr, library_thread);

    int i = thread_lock_nointerrupts(lock);
    interrupt_enable();
    return i;
}

int thread_signal(unsigned int lock, unsigned int cond){
    interrupt_disable();
    if (!lib_call) {
        interrupt_enable();
        return -1;
    }

    list<wait_queue*>::iterator i;

    for(i = wait_queue_list.begin(); i != wait_queue_list.end(); i++) {
        wait_queue* queue_signaled = *i;

        if(queue_signaled->lock == lock && queue_signaled->cond == cond) {
            deque<Thread*>* waiting = queue_signaled->waiting_threads;
            if(waiting->empty()){
                interrupt_enable();
                return 0;
            }
            Thread* thread = waiting->front();
            waiting->pop_front();

            ready_queue.push_back(thread);
            //delete waiting;
            //wait_queue_list.remove(queue_signaled);
            //break;
        }
    }

    interrupt_enable();

    return 0;
}

int thread_broadcast(unsigned int lock, unsigned int cond){
    interrupt_disable();
    if (!lib_call) {
        interrupt_enable();
        return -1;
    }

    list<wait_queue*>::iterator i;

    for(i = wait_queue_list.begin(); i != wait_queue_list.end(); i++) {

        wait_queue* queue_signaled = *i;

        if(queue_signaled->lock == lock && queue_signaled->cond == cond) {
            deque<Thread*> * waiting = queue_signaled->waiting_threads;

            while (waiting->size() > 0){
                Thread* thread = waiting->front();
                waiting->pop_front();

                ready_queue.push_back(thread);
            }
            //delete waiting;
            //wait_queue_list.remove(queue_signaled);
            //break;
        }
    }

    interrupt_enable();

    return 0;
}

