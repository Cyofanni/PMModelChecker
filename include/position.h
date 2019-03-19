#ifndef POSITION_H
#define POSITION_H

class Position
{
private:
	int b_element;
	int eq_index;
	//Position could be a graph node
	mutable int index; 		//needed by Tarjan's algorithm
	mutable int lowlink;	//needed by Tarjan's algorithm
	mutable bool defined;	//needed by Tarjan's algorithm
	mutable bool on_stack;	//needed by Tarjan's algorithm

public:
	Position();
	Position(int b_el, int eq_in, int i=0, int l=0, bool d=0, bool o=0);
	Position(const Position& p);

	Position&
	operator=(const Position &p);

	int
	get_b_element() const;

	int
	get_eq_index() const;
	
	int
	get_index() const;
	
	void
	set_index(int i) const;
	
	void
	set_lowlink(int l) const;
	
	bool
	is_defined() const;
	
	void
	set_defined(bool def) const;
	
	bool
	is_on_stack() const;
	
	void
	set_on_stack(bool os) const;
};


bool
operator<(const Position &p1, const Position &p2);

#endif
