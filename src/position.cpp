#include "../include/position.h"

Position::Position(){}

Position::Position(int b_el, int eq_in, int i, int l, bool o)
{ 
	b_element = b_el;
	eq_index = eq_in;
	index = -1;
	lowlink = 0;
	on_stack = false;
}

Position::Position(const Position& p)
{
	this->b_element = p.b_element;
	this->eq_index = p.eq_index;
	this->index = p.index;
	this->lowlink = p.lowlink;
	this->on_stack = p.on_stack;
}

Position&
Position::operator=(const Position &p)
{
	this->b_element = p.b_element;
	this->eq_index = p.eq_index;
	this->index = p.index;
	this->lowlink = p.lowlink;
	this->on_stack = p.on_stack;
}

bool
Position::operator==(const Position &p)
{
	return (this->b_element == p.b_element) and (this->eq_index == p.eq_index);
}

bool
Position::operator!=(const Position &p)
{
	return (this->b_element != p.b_element) and (this->eq_index != p.eq_index);
}

int
Position::get_b_element() const
{
	return b_element;
}

int
Position::get_eq_index() const
{
	return eq_index;
}

int
Position::get_index() const{
	return index;
}

void
Position::set_index(int i) const
{
	index = i;
}

int
Position::get_lowlink() const
{
	return lowlink;
}

void
Position::set_lowlink(int l) const
{
	lowlink = l;
}

bool
Position::is_on_stack() const
{
	return on_stack;
}

void
Position::set_on_stack(bool os) const
{
	on_stack = os;
}
	

bool
operator<(const Position &p1, const Position &p2)
{
	if (p1.get_b_element() < p2.get_b_element())
	{
		return true;
   	}
	else if (p1.get_b_element() == p2.get_b_element())
	{
		return p1.get_eq_index() < p2.get_eq_index();
	}
	else
	{
		return false;
   	}
}
