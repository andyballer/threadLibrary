#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "thread.h"
#include <list>

using namespace std;

unsigned int lock = 0;
unsigned int cv_enough_notes = 0;
unsigned int cv_more_notes = 1;

int num_notes;
int max_notes;
int num_files;

int last_sandwich = -1;

char ** filearray;

typedef struct corkboard_note{
    int sandwichNum;
    int cashierNum;
    bool lastFromCashier;
    //struct corkboard_note* next;
    //struct corkboard_note* prev;
} corkboard_note;

list<corkboard_note> noteList;

//struct corkboard_note* head;

bool board_has_note(int cashier_id) {
    //corkboard_note* current = head;
    if(noteList.empty()){
        return false;
    }
    list<corkboard_note>::iterator i;
    for(i=noteList.begin(); i != noteList.end(); ++i){
        if(i->cashierNum == cashier_id){
            return true;
        }
    }
    return false;
}

corkboard_note board_find_similar() {
    // Sort array and move everything down after removing.
    // Return the corkboard_note to remove.
    list<corkboard_note>::iterator k;
    corkboard_note best_sandwich;
    int distance = 1000;
    for(k=noteList.begin(); k != noteList.end(); ++k) {
        if (abs(last_sandwich - k->sandwichNum) < distance) {
            distance = abs(last_sandwich - k->sandwichNum);
            best_sandwich = *k;
        }
    }
    return best_sandwich;
}


// The parameters are transformed into a note.
void board_add_sandwich(int sandwichNum, int cashier_id, bool isLast) {
    corkboard_note note;
    note.sandwichNum = sandwichNum;
    note.cashierNum = cashier_id;
    note.lastFromCashier = isLast;
    noteList.push_back(note);
}

// last_sandwich is the sandwich that was last taken.
corkboard_note board_remove_sandwich(){

    corkboard_note order = board_find_similar();
    list<corkboard_note>::iterator j;
    for(j=noteList.begin(); j != noteList.end();){
        if(j->cashierNum == order.cashierNum && j->sandwichNum == order.sandwichNum){
            noteList.erase(j);
            break;
        }
        else{
            j++;
        }
    }
    // Formatting return.
    //note_elements note = {order.cashierNum, order.sandwichNum};
    return order;
}

void cashier_post_note(int cashier_id) {
    //extern unsigned int lock;
    //extern unsigned int cv_enough_notes;
    //extern unsigned int cv_more_notes;

    //thread_lock(lock);

    // Do I already have a note on the board?
    while (board_has_note(cashier_id)) {
        // If I have a note, wait for it to be taken off.
        thread_wait(lock, (unsigned int) cashier_id + 2);
    }

    // Are there too many total notes on the board?
    while (num_notes >= max_notes) {
        thread_wait(lock, cv_more_notes);
    }

    // I'm adding my note to the board!
    num_notes++;

    //thread_unlock(lock);
}

void cashier_initialize (void *id) {
    int cashier_id = (int)(size_t)id;
    //cout << "Cashier ID: " << cashier_id << endl;
    //What is the [cashier_id-1] for?
    //char* filename = filearray[cashier_id-1];
    //start cashier at +2 to offset argv[0] =??? and argv[1] = maxnotes
    char* filename = filearray[cashier_id+2];
    int sandwichNum;
    std::string current_sandwich;

    ifstream filestream(filename);

    if (filestream) { //double check this if we can't figure anything out
        while (std::getline(filestream, current_sandwich)) {
	    const char* c = current_sandwich.c_str();
            sandwichNum = atoi(c);
	    thread_lock(lock);
            cashier_post_note(cashier_id);
            if(filestream.peek() == EOF){
                board_add_sandwich(sandwichNum, cashier_id, true);
            }
            else{
                board_add_sandwich(sandwichNum, cashier_id, false);
            }
	    cout << "POSTED: cashier " << cashier_id << " sandwich " << sandwichNum << endl;
	    if (num_notes == max_notes) {
        	thread_broadcast(lock, cv_enough_notes);
    	    }
	    thread_unlock(lock);
        }
        filestream.close();
    }

}

corkboard_note maker_remove_note(void) {
    //extern unsigned int lock;
    //extern unsigned int cv_more_notes;
    //extern unsigned int cv_enough_notes;

    //thread_lock(lock);

    while (num_notes < max_notes || max_notes == 0) {
        thread_wait(lock, cv_enough_notes);
    }

    corkboard_note note = board_remove_sandwich();

    num_notes--;

    // The board is necessarily full and has lost one.
    thread_signal(lock, cv_more_notes);

    //thread_unlock(lock);
    return note;
}

// Thread should be initialized on this function.
void maker_operate(void) {
    //extern unsigned int lock;
    //this condition needs to be changed because it will not keep operating after the very first time you see it
    //while (num_notes > 0) {
    
    while(1){
        //printf("we made it\n");
        //maker_remove_note();
	thread_lock(lock);
        corkboard_note note = maker_remove_note();
        int cashier_id = note.cashierNum;
        int sandwich = note.sandwichNum;
        bool isLast = note.lastFromCashier;
	cout << "READY: cashier " << cashier_id << " sandwich " << sandwich << endl;
        last_sandwich = sandwich;
        if(isLast){
            num_files--;
        }
        if(num_files < max_notes){
            max_notes = num_files;
        }
        thread_broadcast(lock, (unsigned int) cashier_id+2);
	thread_unlock(lock);
    }
    

}

void start_threads(void * size) {
    // Id should start at two so as not to confound locks.
    int current_id = 0;
    num_files = (int)(size_t) size-2;
    if(max_notes > num_files){
	max_notes = num_files;
    }
    thread_create((thread_startfunc_t) maker_operate, (void *) 100);
    //cout << "Max Notes: " << max_notes << endl;
    for (int nArg = 1; nArg < num_files+1; nArg++) {

        // How do you put two arguments in?
        // Make the argv global and get the filename in the cashier!!!

        thread_create((thread_startfunc_t) cashier_initialize, (void *) current_id);

        current_id++;
    }
   //start_preemptions(false, true, 1);
   // cout << "Max Notes: " << max_notes << endl;
}

int main(int argc, char ** argv) {
    //max_notes = 3;
    //MUST fix how we obtain max notes: this code just sets to 0
    max_notes = atoi(argv[1]);
    //cout << "Max Notes: " << max_notes << endl;
    num_notes = 0;
    filearray = argv;
    thread_libinit((thread_startfunc_t) start_threads, (void *) argc);
    return 0;
}
