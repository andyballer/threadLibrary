#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void test(void * args) {
	cout << "test called with argument " << endl;
}

void parent(void * a) {
	if (thread_libinit((thread_startfunc_t) test, (void *) "1") == 0) {
		cout << "Second call of lib_init succeeded.\n";
		exit(1);
	}
	if (thread_create((thread_startfunc_t) test, (void *) "1") != 0) {
		cout << "Creation of first thread (thread_create) failed.\n";
		exit(1);
	}
}

int main(int argc, char* argv[]) {
	thread_libinit((thread_startfunc_t) parent, (void *) 100);
	exit(0);
}
