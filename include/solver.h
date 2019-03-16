#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <climits>
#include "position.h"
#include "operator.h"
#include "move_composer.h"
#include "dependency_graph.h"

using namespace std;

/*Solver doesn't need the original system, just the type of each equations*/
class Solver{
private:
	map<int,string> min_max_eq; //type (min/max) of each equation
	int basis_size;
	int system_size;
	int lattice_height;
	map<Position, ExpNode*> symb_E_moves; //symbolic E-move for each basis element, for each equation index

	//computes progress measure at the formula corresponding to position p
	void comp_symb_prog_meas(const Position &p, ExpNode *formula,
                                 const vector<int> &delta, const vector< vector< vector<int> > > &prog_meas_matr, vector<int> &pm_value);

	bool is_top(const vector<int> &pm_value) const;  //return 'true' if a given R(b)(i) has the Top value ('star')

public:
	Solver(const map<int,string>& mmeq, int bs, int ss, const map<Position, ExpNode*> &s_em, int lh);

	//should find the formula in the map Position:Formulae
	vector< vector < vector<int> > > solve_system_worklist();
	vector< vector < vector<int> > > solve_system_chaotic_iteration();

	//lexicographic (<) order between vectors
	static bool lex_vectors(vector<int> a, vector<int> b, unsigned int from);
	static void print_pm_matrix(vector< vector < vector<int> > > pm_matrix, int bs, int ss);
};

#endif
