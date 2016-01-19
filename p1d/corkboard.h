#ifndef CORKBOARD_H_
#define CORKBOARD_H_

class Corkboard {

	typedef struct corkboard_note{
       int sandwichNum;
       int cashierNum;
       struct corkboard_note* next;
       struct corkboard_note* prev;
    } corkboard_note;

    struct corkboard_note* head;

public:
    void add_sandwich(int sandwichNum, int cashier_id);
    void remove_sandwich(int last_sandwich);

private:
    corkboard_note* find_similar(int last_sandwich);

};

#endif