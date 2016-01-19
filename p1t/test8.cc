#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void test(void * args) {

	thread_lock(0);
	thread_wait(0,4);

	if (thread_lock(0) == 0) {
		exit(1);
	}
	if (thread_unlock(0) != 0) {
		exit(1);
	}
}

void sig(void * args) {
	thread_signal(0,4);
}

void parent(void * a) {
	thread_create((thread_startfunc_t) test, (void *) "1");
	thread_create((thread_startfunc_t) sig, (void *) "2");
}

int main(int argc, char* argv[]) {
	thread_libinit((thread_startfunc_t) parent, (void *) 100);
	exit(0);
}

