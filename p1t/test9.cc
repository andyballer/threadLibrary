#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int wrong_cv = 10;
int right_cv = 5;
int which_sig;

void test(void * args) {
	thread_lock(14);
	thread_wait(14, right_cv);

	if (which_sig == 1) {
		exit(1);
	}
}


void sig(void * args) {
	thread_signal(14, wrong_cv);
	which_sig = 1;
}

void sig2(void * args) {
	thread_signal(14, right_cv);
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

