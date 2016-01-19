#include "thread.h"
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

string output_str;
string desired_str = "2 got lock3 got lock";

int lock = 0;

void one (void * args) {
  thread_lock (lock);    // 1: R=[2,3]      
  thread_yield ();       // 2: R=[2,3]
  thread_unlock (lock);      // 5: R=[3], L=[2] 
}

void two (void * args) {
  thread_lock (lock);    // 3: R=[3,1]
  output_str.append("2 got lock");  // 7: R=[], L=[3]
  thread_unlock (lock);  // 8: R=[], L=[3] 
}

void three (void * args) {
  thread_yield ();       // 4: R=[1], L=[2]
  thread_lock (lock);    // 6: R=[2]
  output_str.append("3 got lock");  // 9: R=[], L=[]
  thread_unlock (lock);  // 10: R=[], L=[]
  
  if (output_str.compare(desired_str) != 0) {
    exit(1);
  }
}

void start (void * a) {
  thread_create ((thread_startfunc_t) one, (void *) "a");
  thread_create ((thread_startfunc_t) two, (void *) "b");
  thread_create ((thread_startfunc_t) three, (void *) "c");
}

int main(int argc, char* argv[]) {
  thread_libinit((thread_startfunc_t) start, (void *) 100);
}
