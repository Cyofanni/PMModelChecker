//represents the Dependency Graph described in [Baldan 2018]
#ifndef DEPENDENCY_GRAPH_H
#define DEPENDENCY_GRAPH_H

#include <iostream>
#include <list>
#include <stack>
#include <map>
#include "position.h"
#include "expr_move.h"
#include "move_composer.h"

using namespace std;

class DependencyGraph{
private:
    //adjacency lists for each position
	map<Position,vector<Position> > pred;
	/*Predecessor map: {{(b,i): [(b',j),(b'',k) etc...]}}
	  maps a position to its predecessors
	*/

	//gets position and its formula
	void add_to_pred(Position pos, ExpNode *formula);
public:
	//build adjacency lists inside the constructor
	DependencyGraph(const map<Position,ExpNode*> &symb_E_moves, int basis_size, int system_size);

	map<Position,vector<Position> > get_dependency_graph() const;

	void print_dependency_graph();
};

#endif
