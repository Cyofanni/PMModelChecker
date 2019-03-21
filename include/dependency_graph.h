//represents the Dependency Graph described in [Baldan 2018]
#ifndef DEPENDENCY_GRAPH_H
#define DEPENDENCY_GRAPH_H

#include <iostream>
#include <list>
#include <stack>
#include <map>
#include <set>
#include "position.h"
#include "expr_move.h"
#include "move_composer.h"

using namespace std;

class DependencyGraph
{
private:
	//adjacency lists for each position
	map<Position*,vector<Position*> > pred;
	/*Predecessor map: {{(b,i): [(b',j),(b'',k) etc...]}}
	  maps a position to its predecessors
	 */

	//gets position and its formula
	
	map<Position*,vector<Position*> >::iterator find_pos_in_pred(const Position& pos);
	
	void
	add_to_pred(const Position &pos, ExpNode *formula);
	
	void
	strong_connect(Position *p, int &ind, stack<Position*> &st, set<Position*> &Sset,
						int &comp_counter, vector<set<Position*> > &components);
	
public:
	//build adjacency lists inside the constructor
	DependencyGraph(const map<Position,ExpNode*> &symb_E_moves, int basis_size, int system_size);
	~DependencyGraph();

	map<Position*,vector<Position*> >
	get_dependency_graph() const;
	
	vector<set<Position*> >
	tarjan();

	void
	print_dependency_graph();
};

#endif
