#include "thread.h"
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

string output_str;
string desired_str = "yaydone";


void one (void * args) {

  thread_lock (0);
  output_str.append("yay");
  thread_yield();
  
}

void two (void * args) {
  thread_lock (0);
  output_str.append("success");
  thread_unlock(0);
  thread_yield();
  output_str.append("we made it");


}

void three (void * args) {
  thread_lock (0);         // R[2,1],L[ , ],CV[ , ] 9
  output_str.append("lock for three"); // R[ , ],L[1, ],CV[ , ] 13
  thread_unlock (0);       // R[ , ],L[1, ],CV[ , ] 14
  output_str.append("three done");      // R[1, ],L[ , ],CV[ , ] 15
}


void start (void * a) {
  thread_create ((thread_startfunc_t) one, (void *) 100);
  thread_create ((thread_startfunc_t) two, (void *) 100);
  thread_create ((thread_startfunc_t) three, (void *) 100);
  thread_yield();
  output_str.append("done");
    if (output_str.compare(desired_str) != 0) {
    exit(1);
  }
	
}

int main(int argc, char* argv[]) {
    thread_libinit((thread_startfunc_t) start, (void *) 100);
}
