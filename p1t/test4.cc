#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void test(void * args) {
	thread_lock(7);
	if (thread_unlock(6) == 0) {
		exit(1);
	}
}

void parent(void * a) {
	thread_create((thread_startfunc_t) test, (void *) "1");
}

int main(int argc, char* argv[]) {
	thread_libinit((thread_startfunc_t) parent, (void *) 100);
	exit(0);
}

