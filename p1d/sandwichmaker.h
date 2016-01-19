#ifndef SANDWICHMAKER_H_
#define SANDWICHMAKER_H_

class Sandwich_maker {
public:
	void operate(void);

private:
	int last_sandwich = 0;
	void remove_note(void);
};

#endif