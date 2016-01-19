#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void test(void * args) {
    cout << "Test called." << endl;
}

int main(int argc, char* argv[]) {

    if(thread_create((thread_startfunc_t) test, (void *) "1") == 0) {
        cout << "creating thread before libinit called .\n";
        exit(1);
    }

    if(thread_yield() == 0) {
        cout << "Yielding before libinit.\n";
        exit(1);
    }

    if(thread_signal(1,2) == 0) {
        cout << "signal before libinit.\n";
        exit(1);
    }

    if(thread_broadcast(1,2) == 0) {
        cout <<"broadcast before libinit.\n";
        exit(1);
    }
    if(thread_lock(1) == 0) {
        cout <<"locking before libinit .\n";
        exit(1);
    }
    if(thread_unlock(1) == 0) {
        cout <<"yeilding before libinit .\n";
        exit(1);
    }
    if(thread_wait(1,2) == 0) {
        cout <<"yeilding before libinit .\n";
        exit(1);
    }
    
	exit(0);
}
