#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void test(void * args) {

	thread_lock(0);

	if (thread_broadcast(0,0) != 0) {
		cout << "Broadcasting (thread_broadcast) fails for one thread.\n";
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

