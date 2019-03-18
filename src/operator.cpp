#include "../include/operator.h"

Operator::Operator(){}
Operator::Operator(string n, unsigned int be, /*int a,*/ ExpNode* f): name(n), basis_element(be),/*arity(a),*/ formula(f){}

string
Operator::get_name() const
{
	return name;
}

unsigned int
Operator::get_basis_element() const
{
	return basis_element;
}

/*unsigned int Operator::get_arity() const{
	return arity;
}*/

ExpNode*
Operator::get_formula() const
{
	return ExpNode::deep_clone(formula);
}
