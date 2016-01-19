#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void test(void * args) {
	cout << "test called" << endl;
	thread_lock(3);
	thread_unlock(3);
	if (thread_unlock(3) == 0) {
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
