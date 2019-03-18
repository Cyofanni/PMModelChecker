#include "../include/dependency_graph.h"

//accepts a position and its formula
void
DependencyGraph::add_to_pred(Position pos, ExpNode *formula)
{
	//use 'get_leaves' on 'formula' from ExpNode
	vector<ExpNode*> leaves;
	ExpNode::get_leaves(formula, leaves);

	for (int i = 0; i < leaves.size(); i++)
	{
		string atom = leaves[i]->get_node_name();
		if (atom != "true" and atom != "false")
		{   //because atoms 'true' and 'false' don't contain a game position
			string j_str = MoveComposer::extract_basis_from_atom(atom);
			string k_str = MoveComposer::extract_index_from_atom(atom);
			int j = atoi(j_str.c_str());
			int k = atoi(k_str.c_str());
			Position p(j, k);  //indices of current atom in formula
			map<Position,vector<Position> >::iterator itr = pred.find(p);
			if (itr != pred.end())
			{
				itr->second.push_back(pos);  //update predecessors list with given (b,i) in 'pos'
			}
		}
	}
}

DependencyGraph::DependencyGraph(const map<Position,ExpNode*> &symb_E_moves, int basis_size, int system_size)
{
	for (int i = 0; i < system_size; i++)
	{
		for (int j = 0; j < basis_size; j++)
		{
			Position p(j, i);
			pred[p]; //add keys to pred without specifying their values
		}
	}
	for (int i = 0; i < system_size; i++)
	{
		for (int j = 0; j < basis_size; j++)
		{
			Position p(j, i);
			/*cout << j << " " << i << endl;
			map<Position,ExpNode*>::const_iterator iter = symb_E_moves.find(p);
			ExpNode::print_move(iter->second);
			cout << endl << endl;*/
			add_to_pred(p, symb_E_moves.find(p)->second);
		}
	}
}

map<Position,vector<Position> >
DependencyGraph::get_dependency_graph() const
{
	return pred;
}

void
DependencyGraph::print_dependency_graph()
{
	map<Position,vector<Position> >::iterator loop_iter;
	for (loop_iter = pred.begin(); loop_iter != pred.end(); loop_iter++)
	{
		cout << "Position: (" <<  loop_iter->first.get_b_element() << ","
		     << loop_iter->first.get_eq_index() << ")" << ": ";
		cout << "Predecessors:{";
		for (int i = 0; i < loop_iter->second.size(); i++)
		{
			cout << " Position:(" <<  loop_iter->second[i].get_b_element() << ","
		             << loop_iter->second[i].get_eq_index() << ")" << ",";
		}
		cout << "}";
		cout << endl << endl;
	}
}


/*int main(){
	map<Position,ExpNode*> symb_E_moves;
	Position p_0_0(0, 0);
	ExpNode *formula_0_0_leaf_0 = new ExpNode("[b_0,0]", 0);
	ExpNode *formula_0_0_leaf_1 = new ExpNode("[b_1,0]", 0);
	ExpNode *formula_0_0 = new ExpNode("or", formula_0_0_leaf_0, formula_0_0_leaf_1);
	symb_E_moves[p_0_0] = formula_0_0;

	Position p_0_1(0, 1);
	ExpNode *formula_0_1_leaf_0 = new ExpNode("[b_0,0]", 0);
	ExpNode *formula_0_1_leaf_1 = new ExpNode("[b_1,0]", 0);
	ExpNode *formula_0_1_leaf_2 = new ExpNode("[b_1,1]", 0);
	ExpNode *formula_0_1_right = new ExpNode("and", formula_0_1_leaf_1, formula_0_1_leaf_2);
	ExpNode *formula_0_1 = new ExpNode("and", formula_0_0_leaf_0, formula_0_1_right);
	symb_E_moves[p_0_1] = formula_0_1;

	Position p_1_0(1, 0);
	ExpNode *formula_1_0_leaf_0 = new ExpNode("[b_0,0]", 0);
	ExpNode *formula_1_0_leaf_1 = new ExpNode("[b_1,1]", 0);
	ExpNode *formula_1_0 = new ExpNode("or", formula_1_0_leaf_0, formula_1_0_leaf_1);
	symb_E_moves[p_1_0] = formula_1_0;

	Position p_1_1(1, 1);
	ExpNode *formula_1_1_leaf_0 = new ExpNode("[b_1,0]", 0);
	ExpNode *formula_1_1_leaf_1 = new ExpNode("[b_1,1]", 0);
	ExpNode *formula_1_1 = new ExpNode("or", formula_1_1_leaf_0, formula_1_1_leaf_1);
	symb_E_moves[p_1_1] = formula_1_1;


	DependencyGraph depgraph(symb_E_moves, 2, 2);
	depgraph.print_dependency_graph();
}*/
