/*represent basic (user-defined) operators with their moves*/

#ifndef OPERATOR_H
#define OPERATOR_H

#include "expr_move.h"

class Operator{
private:
	string name;
	//unsigned int arity;

	unsigned int basis_element;
	ExpNode *formula; //the operator's basic move
public:
	Operator();
	Operator(string n, unsigned int be, /*int a,*/ ExpNode* f);
	string get_name() const;
	unsigned int get_basis_element() const;

	//unsigned int get_arity() const;
	ExpNode* get_formula() const;
};

#endif
