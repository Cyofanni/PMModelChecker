#ifndef INPUT_CHECKER_H
#define INPUT_CHECKER_H

#include "expr_move.h"
#include "move_composer.h"
#include <set>

using namespace std;

//find inconsistencies in the input equational system (and moves)
class InputChecker
{
private:
	vector<Equation*> *eq_mv;
	int basis_size;

	//retrieve all the operators used in an equation's rhs
	void
	find_used_operators_equation(ExpNode *rhs, set<string> &operators) const;

	//retrieve all the 'phi' equations (thus, moves)
	vector<string>
	get_phi_identifiers() const;

	//retrieve all the operators used in the system
	set<string>
	find_used_operators_system() const;

	int
	find_max_left_index() const;

public:
	InputChecker(vector<Equation*> *eq_mv, int basis_size);

	/*check if the number of moves is consistent with basis_size and used operators:
	 * eg) basis_size = 2, operators: "and", "or" then there should be:
	 * 		  phi_0_and = ...; phi_1_and = ...;
	 * 		  phi_0_or = ...; phi_1_or = ...;
	 */
	 /*returns an empty vector for enough moves, otherwise returns
	   the missing 'phi' identifier*/
	vector<string>
	get_missing_moves();

	int
	find_out_of_bound_index() const; //return index of wrong equation
};

#endif
