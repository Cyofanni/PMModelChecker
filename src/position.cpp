#include "../include/position.h"

Position::Position(){}

Position::Position(int b_el, int eq_in): b_element(b_el), eq_index(eq_in){}

Position::Position(const Position& p){
	this->b_element = p.b_element;
	this->eq_index = p.eq_index;
}

Position& Position::operator=(const Position& p){
	this->b_element = p.b_element;
	this->eq_index = p.eq_index;
}

int Position::get_b_element() const{
	return b_element;
}

int Position::get_eq_index() const{
	return eq_index;
}

bool operator<(const Position &p1, const Position &p2){
	if (p1.get_b_element() < p2.get_b_element()){
		return true;
   	}
	else if (p1.get_b_element() == p2.get_b_element()){
		return p1.get_eq_index() < p2.get_eq_index();
	}
	else{
		return false;
   	}
}
