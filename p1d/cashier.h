#ifndef CASHIER_H_
#define CASHIER_H_

#include <string>

class Cashier {

public:
    Cashier(std::string filename);
    void initialize(int id);
    void post_note(void);

private:
    int cashier_id;
    bool active_note;
    std::string filename;

};

#endif