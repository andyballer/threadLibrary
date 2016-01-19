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
  output_str.append("c"); 
  thread_lock(2);
  thread_yield();
  output_str.append("d"); 
  thread_wait (0,0);    // R[2,3],L[ , ],CV[ , ] 3
  output_str.append("e");    // R[ , ],L[ , ],CV[ , ] 16
  thread_yield();
  output_str.append("f"); 
  thread_signal(0,2);
  output_str.append("g"); 
  thread_broadcast(0,1);
  output_str.append("h");   
  thread_broadcast(0,0);
  output_str.append("i");   
  thread_unlock (0);    // R[ , ],L[ , ],CV[ , ] 17
  output_str.append("j");     // R[ , ],L[ , ],CV[ , ] 18
}


void two (void * args) {
  thread_lock (0);      // R[3, ],L[ , ],CV[1, ] 4
  output_str.append("k");    // R[3, ],L[ , ],CV[1, ] 5
  thread_wait(0,1);
  output_str.append("l");
  thread_yield ();      // R[3, ],L[ , ],CV[1, ] 6
  output_str.append("m");  // R[1, ],L[3, ],CV[ , ] 10
  thread_yield();
  output_str.append("n"); 
  thread_lock(1);
  output_str.append("o");
  thread_broadcast(0,0);
  thread_broadcast(0,1);
  thread_broadcast(1,0);
  thread_broadcast(1,1);
  output_str.append("p");
  thread_signal(1,1);
  output_str.append("q");
  thread_signal(1,1);
  output_str.append("s");
  thread_unlock(0);
  output_str.append("t");	
  thread_yield();
  output_str.append("u");
  thread_unlock (1);    // R[1, ],L[3, ],CV[ , ] 11
  output_str.append("v");     // R[1,3],L[ , ],CV[ , ] 12
  thread_broadcast(0,0);
  output_str.append("w"); 
  thread_yield();
  output_str.append("x");
}

void three (void * args) {
  thread_signal(0,0);      // R[2, ],L[ , ],CV[1, ] 7
  output_str.append("y");   // R[2,1],L[ , ],CV[ , ] 8
  thread_yield();
  output_str.append("z");
  thread_lock (0);         // R[2,1],L[ , ],CV[ , ] 9
  output_str.append("B"); 
  thread_broadcast(0,1);
  output_str.append("C");
  thread_broadcast(0,0);
  thread_broadcast(0,1);
  thread_broadcast(1,0);
  thread_broadcast(1,1);
  thread_yield();
  output_str.append("D");
  thread_wait(0,1);
  output_str.append("E"); 
  thread_unlock (0);       // R[ , ],L[1, ],CV[ , ] 14
  output_str.append("F");
  thread_yield();
  thread_signal(1,1);
  output_str.append("G"); 
  thread_broadcast(0,0);
  output_str.append("H");   // R[1, ],L[ , ],CV[ , ] 15
  thread_wait(0,1);
  output_str.append("I");
  thread_yield();
  output_str.append("J"); 
  string desired_str = "KLMNOPQkyabczdRSTBCDlmnopqstuvwexfghijEFGHIJ";

    if (output_str.compare(desired_str) != 0) {
      cout << "boo\n";
        exit(1);
    }
    else{
      cout << "wowowowowowoooooooo \n";
    }
}

void four (void * args) {
  thread_lock(0);
  thread_broadcast(0,0);
  thread_wait(0,1);
  thread_broadcast(0,1);
  thread_broadcast(1,0);
  thread_broadcast(1,1);
  thread_unlock(0);
    }

 void five (void * args) {
  thread_lock(1);
  thread_broadcast(0,0);
  thread_yield();
  thread_wait(1,1);
  thread_broadcast(0,1);
  thread_broadcast(1,0);
  thread_yield();
  thread_broadcast(1,1);
  thread_yield();
  thread_unlock(1);  

  }

void six (void * args) {
  thread_lock(2);
  thread_broadcast(0,0);
  thread_yield();
  thread_wait(1,0);
  thread_broadcast(0,1);
  thread_yield();
  thread_broadcast(1,0);
  thread_yield();
  thread_broadcast(1,1);
  thread_yield();
  thread_unlock(2);
}




void start (void * a) {
  thread_create ((thread_startfunc_t) one, (void *) 100);
  output_str.append("K");
  thread_create ((thread_startfunc_t) two, (void *) 100);
  output_str.append("L");
  thread_create ((thread_startfunc_t) three, (void *) 100);
  output_str.append("M");
  thread_create ((thread_startfunc_t) four, (void *) 100);
  output_str.append("N");
  thread_create ((thread_startfunc_t) five, (void *) 100);
  output_str.append("O");
  thread_create ((thread_startfunc_t) six, (void *) 100);
  output_str.append("P");
  thread_lock(0);
  output_str.append("Q");
  thread_wait(0,0);
  output_str.append("R");
  thread_unlock(0);
  output_str.append("S");
  thread_wait(0,1);
  output_str.append("T");
}

int main(int argc, char* argv[]) {
    thread_libinit((thread_startfunc_t) start, (void *) 100);
	

}
