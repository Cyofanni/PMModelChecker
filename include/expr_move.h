#ifndef EXPR_MOVE_H
#define EXPR_MOVE_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include "../include/tokens.h"

using namespace std;

//used for expressions and symbolic E-moves
class ExpNode
{
private:
	string node_name;

public:
	//should handle any arity (up to a certain number)

	ExpNode *left;
	ExpNode *right;
	ExpNode *right_1;
	ExpNode *right_2;
	ExpNode *right_3;
	ExpNode *right_4;

	ExpNode(string n_name, ExpNode* l_node, ExpNode *r_node = 0, ExpNode *r1_node = 0, ExpNode *r2_node = 0,
			ExpNode *r3_node = 0, ExpNode *r4_node = 0);

	string
	get_node_name() const;

	void
	set_node_name(string new_name);

	static void
	edit_index(ExpNode *e, string index);

	static ExpNode*
	edit_atoms(ExpNode *e, const vector<ExpNode*> &moves); //edit atoms in 'e' with moves in 'moves'

	static ExpNode*
	deep_clone(ExpNode *src);

	static void
	get_leaves(ExpNode *&e, vector<ExpNode*> &leaves_array);

	static void
	graft_subtree(ExpNode *&e, ExpNode *where, ExpNode *new_subtree);

	static void print_move(ExpNode *e);
	static void print_equation_infix(ExpNode *e, FILE *handle);

	static int
	get_max_index_in_equation(ExpNode *e);
};


//could also be a symbolic E-move (eg) phi_0_and = [b_1,1] etc...)
class Equation
{
private:
	string id;
	string eq_type;
	ExpNode *exp;

public:
	Equation(string name, string type, ExpNode *expression);

	string
	get_id() const;

	string
	get_eq_type() const;

	ExpNode*
	get_exp_node() const;

	static int
	mucalculus_normalizer_equation(ExpNode *eq_rhs, string left_id, string eq_type, int max_index, vector<Equation*> &equations);
};

class EqSystem
{
private:
	vector<Equation*> *equations;
public:
	EqSystem(vector<Equation*> *equationlist);

	vector<Equation*>*
	get_equations() const;

	void
	add_equation(Equation *eq);

	static vector<Equation*>
	mucalculus_normalizer_system(vector<Equation*> system);
};


extern int yyparse();
extern EqSystem *root;
extern FILE * yyin;

#endif
