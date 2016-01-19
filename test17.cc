#include "thread.h"
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

string output_str;

void one (void * args) {
  thread_yield();
  output_str.append("a");
  thread_lock (0);      // R[2,3],L[ , ],CV[ , ] 1
  output_str.append("b");     // R[2,3],L[ , ],CV[ , ] 2
  thread_lock(0);
  output_str.append("z"); 
  thread_yield();
  output_str.append("t"); 
  thread_wait (0,0);    // R[2,3],L[ , ],CV[ , ] 3
  output_str.append("c");    // R[ , ],L[ , ],CV[ , ] 16
  thread_yield();
  output_str.append("y"); 
  thread_signal(0,2);
  output_str.append("x"); 
  thread_unlock (0);    // R[ , ],L[ , ],CV[ , ] 17
  output_str.append("?");     // R[ , ],L[ , ],CV[ , ] 18
  string desired_str = "odjklatmnbztpq1efsghi2cyx?";

    if (output_str.compare(desired_str) != 0) {
      cout << "boo\n";
	exit(1);
    }
}

void two (void * args) {
  thread_lock (0);      // R[3, ],L[ , ],CV[1, ] 4
  output_str.append("d");    // R[3, ],L[ , ],CV[1, ] 5
  thread_wait(0,1);
  output_str.append("e");
  thread_yield ();      // R[3, ],L[ , ],CV[1, ] 6
  output_str.append("f");  // R[1, ],L[3, ],CV[ , ] 10
  thread_yield();
  output_str.append("s"); 
  thread_lock(1);
  output_str.append("g");
  thread_unlock(0);
  output_str.append("h");	
  thread_unlock (1);    // R[1, ],L[3, ],CV[ , ] 11
  output_str.append("i");     // R[1,3],L[ , ],CV[ , ] 12
  thread_broadcast(0,0);
  output_str.append("2"); 
}

void three (void * args) {
  thread_signal(0,0);      // R[2, ],L[ , ],CV[1, ] 7
  output_str.append("j");   // R[2,1],L[ , ],CV[ , ] 8
  thread_lock (0);         // R[2,1],L[ , ],CV[ , ] 9
  output_str.append("k"); // R[ , ],L[1, ],CV[ , ] 13
  thread_broadcast(0,1);
  output_str.append("l");
  thread_yield();
  output_str.append("t"); 
  thread_unlock (0);       // R[ , ],L[1, ],CV[ , ] 14
  output_str.append("m");
  thread_broadcast(0,0);
  output_str.append("n");   // R[1, ],L[ , ],CV[ , ] 15
}


void start (void * a) {
  thread_create ((thread_startfunc_t) one, (void *) 100);
  thread_create ((thread_startfunc_t) two, (void *) 100);
  thread_create ((thread_startfunc_t) three, (void *) 100);
  thread_lock(0);
  output_str.append("o");
  thread_wait(0,0);
  output_str.append("p");
  thread_unlock(0);
  output_str.append("q");
  thread_wait(0,0);
  output_str.append("1");
}

int main(int argc, char* argv[]) {
    thread_libinit((thread_startfunc_t) start, (void *) 100);
	

}
