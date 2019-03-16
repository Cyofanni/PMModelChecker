#ifndef POSITION_H
#define POSITION_H

class Position{
private:
	int b_element;
	int eq_index;
public:
	Position();
	Position(int b_el, int eq_in);
	Position(const Position& p);

	Position& operator=(const Position &p);

	int get_b_element() const;
	int get_eq_index() const;
};

bool operator<(const Position &p1, const Position &p2);

#endif
