#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int wrong_lock = 10;
int right_lock = 5;
int which_sig;

void test(void * args) {

	thread_lock(0);
	thread_wait(right_lock, 0);

	if (which_sig == 1) {
		exit(1);
	}
}

void sig(void * args) {
	thread_signal(wrong_lock, 0);
	which_sig = 1;
}

void sig2(void * args) {
	thread_signal(right_lock, 0);
	which_sig = 2;
}

void parent(void * a) {
	thread_create((thread_startfunc_t) test, (void *) "1");
	thread_create((thread_startfunc_t) sig, (void *) "2");
	thread_create((thread_startfunc_t) sig2, (void *) "3");
}

int main(int argc, char* argv[]) {
	thread_libinit((thread_startfunc_t) parent, (void *) 100);
	exit(0);
}

