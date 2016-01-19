#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void test(void * args) {

	if (thread_lock(0) != 0) {
		cout << "Locking with two threads (thread_lock) failed.\n";
		exit(1);
	}
}

void parent(void * a) {
	thread_create((thread_startfunc_t) test, (void *) "1");
	thread_create((thread_startfunc_t) test, (void *) "2");
}

int main(int argc, char* argv[]) {
	thread_libinit((thread_startfunc_t) parent, (void *) 100);
	exit(0);
}
