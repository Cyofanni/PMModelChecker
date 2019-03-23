/* Move Composer: accepts a tree representing an equation
 *                (with any number of operators),
 *                and generates its (composite) symbolic
 *                E-move as a tree of moves.
 */

#ifndef MOVE_COMPOSER_H
#define MOVE_COMPOSER_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <climits>
#include <algorithm>
#include "position.h"
#include "operator.h"
#include "expr_move.h"
#include "tokens.h"

using namespace std;


class MoveComposer
{
private:
	vector<Equation*> *equations_moves;
	int basis_size;
	vector<Operator> operators; //list of operators in the input

	Operator
	find_operator_by_name(string n) const;

	//compose the move for a single equation, for a given basis element
	ExpNode*
	single_composer(ExpNode *eq, int basis_element) const;

public:
	MoveComposer(vector<Equation*> *eq_mv, int basis_size);
	vector<Operator>
	get_operators() const;

	map<Position,ExpNode*>
	system_composer() const;  //compose each equation's move

	//given phi_0_name, extract '0' and 'name'
	static string
	extract_index_from_phi(string phi_id);

	static string
	extract_operator_from_phi(string phi_id);

	static string
	extract_index_from_variable(string x_id);

	static string
	extract_basis_from_atom(string atom);

	static string
	extract_index_from_atom(string atom);
};

#endif
