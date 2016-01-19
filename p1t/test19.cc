#include "thread.h"
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

string output_str;
string desired_str = "one waittwo yieldthree signalstwo returnstwo donethree gets lockthree doneone awakeone done";


void one (void * args) {

  thread_lock (0);      // R[2,3],L[ , ],CV[ , ] 1
  output_str.append("one wait");     // R[2,3],L[ , ],CV[ , ] 2
  thread_wait (0,0);    // R[2,3],L[ , ],CV[ , ] 3
  output_str.append("one awake");    // R[ , ],L[ , ],CV[ , ] 16
  thread_unlock (0);    // R[ , ],L[ , ],CV[ , ] 17
  output_str.append("one done");     // R[ , ],L[ , ],CV[ , ] 18

  if (output_str.compare(desired_str) != 0) {
    exit(1);
  }
}

void two (void * args) {
  thread_lock (0);      // R[3, ],L[ , ],CV[1, ] 4
  output_str.append("two yield");    // R[3, ],L[ , ],CV[1, ] 5
  thread_yield ();      // R[3, ],L[ , ],CV[1, ] 6
  output_str.append("two returns");  // R[1, ],L[3, ],CV[ , ] 10
  thread_unlock (0);    // R[1, ],L[3, ],CV[ , ] 11
  output_str.append("two done");     // R[1,3],L[ , ],CV[ , ] 12
}

void three (void * args) {
  thread_signal(0,0);      // R[2, ],L[ , ],CV[1, ] 7
  output_str.append("three signals");   // R[2,1],L[ , ],CV[ , ] 8
  thread_lock (0);         // R[2,1],L[ , ],CV[ , ] 9
  output_str.append("three gets lock"); // R[ , ],L[1, ],CV[ , ] 13
  thread_unlock (0);       // R[ , ],L[1, ],CV[ , ] 14
  output_str.append("three done");      // R[1, ],L[ , ],CV[ , ] 15
}


void start (void * a) {
  thread_create ((thread_startfunc_t) one, (void *) "b");
  thread_create ((thread_startfunc_t) two, (void *) "c");
  thread_create ((thread_startfunc_t) three, (void *) "d");
}

int main(int argc, char* argv[]) {
    thread_libinit((thread_startfunc_t) start, (void *) 100);
}
