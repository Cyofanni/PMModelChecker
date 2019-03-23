#include "../include/dependency_graph.h"

map<Position*,vector<Position*> >::iterator DependencyGraph::find_pos_in_pred(const Position& pos)
{
	map<Position*,vector<Position*> >::iterator itr;
	map<Position*,vector<Position*> >::iterator res = pred.end();

	for (itr = pred.begin(); itr != pred.end(); itr++)
	{
		if (itr->first->get_b_element() == pos.get_b_element() and itr->first->get_eq_index() == pos.get_eq_index())
		{
			res = itr;
			break;
		}
	}

	return res;
}

//accepts a position and its formula
void
DependencyGraph::add_to_pred(const Position &pos, ExpNode *formula)
{
	//use 'get_leaves' on 'formula' from ExpNode
	vector<ExpNode*> leaves;
	ExpNode::get_leaves(formula, leaves);

	//find where pos is in 'pred', because 'pos' is just a value
	map<Position*,vector<Position*> >::iterator pos_iter = find_pos_in_pred(pos);

	for (int i = 0; i < leaves.size(); i++)
	{
		string atom = leaves[i]->get_node_name();
		if (atom != TRUE_ATOM and atom != FALSE_ATOM)
		{   //because atoms 'true' and 'false' don't contain a game position
			string j_str = MoveComposer::extract_basis_from_atom(atom);
			string k_str = MoveComposer::extract_index_from_atom(atom);
			int j = atoi(j_str.c_str());
			int k = atoi(k_str.c_str());
			Position p(j, k);  //indices of current atom in formula

			map<Position*,vector<Position*> >::iterator itr = find_pos_in_pred(p);

			if (itr != pred.end())
			{
				itr->second.push_back(pos_iter->first);  //update predecessors list with given (b,i) in 'pos'
			}
		}
	}
}

void
DependencyGraph::strong_connect(Position *p, int &ind, stack<Position*> &S, set<Position*> &Sset,
						int &comp_counter, vector<set<Position*> > &components)
{
	p->set_index(ind);
	p->set_lowlink(ind);
	ind += 1;
	S.push(p);
	p->set_on_stack(true);

	vector<Position*> &p_predecessors = find_pos_in_pred(Position(p->get_b_element(),p->get_eq_index())) -> second;

	for (int k = 0; k < p_predecessors.size(); k++)
	{
		if (p_predecessors[k]->get_index() < 0)
		{
			strong_connect(p_predecessors[k], ind, S, Sset, comp_counter, components);
			p->set_lowlink(std::min(p->get_lowlink(), p_predecessors[k]->get_lowlink()));
		}
		else if (pred.find(p)->second[k]->is_on_stack() == true)
		{
			p->set_lowlink(std::min(p->get_lowlink(), p_predecessors[k]->get_index()));
		}
	}

	if (p->get_lowlink() == p->get_index())
	{
		Sset = set<Position*>();
		comp_counter++;
		//cout << "SCC number " << comp_counter << ": ";
		Position *w;
		do
		{
			w = S.top();
			S.pop();
			Sset.insert(w);
			w->set_on_stack(false);
			//cout << "(" << w->get_b_element() << "," << w->get_eq_index() << ")";
		}
		while (w != p);
		components.push_back(Sset);
		//cout << endl;
	}
}

DependencyGraph::DependencyGraph(const map<Position,ExpNode*> &symb_E_moves, int basis_size, int system_size)
{
	Position *p;
	for (int i = 0; i < system_size; i++)
	{
		for (int j = 0; j < basis_size; j++)
		{
			p = new Position(j, i);
			pred[p]; //add keys to pred without specifying their values
		}
	}


	for (int i = 0; i < system_size; i++)
	{
		for (int j = 0; j < basis_size; j++)
		{
			Position pos(j, i);
			symb_E_moves.find(pos)->second;
			add_to_pred(pos, symb_E_moves.find(pos)->second);
		}
	}
}

DependencyGraph::~DependencyGraph()
{
	map<Position*,vector<Position*> >::iterator loop_iter;
	for (loop_iter = pred.begin(); loop_iter != pred.end(); loop_iter++)
	{
		delete (loop_iter->first);
	}
}

map<Position*,vector<Position*> >
DependencyGraph::get_dependency_graph() const
{
	return pred;
}

vector<set<Position*> >
DependencyGraph::tarjan()
{
	int ind = 0;
	stack<Position*> st;
	set<Position*> Sset;
	int comp_counter = 0;
	vector<set<Position*> > components;   //stores final connected components

	map<Position*,vector<Position*> >::iterator loop_iter;
	//loop on vertices
	for (loop_iter = pred.begin(); loop_iter != pred.end(); loop_iter++)
	{
		if (loop_iter->first->get_index() == -1)
		{
			vector<Position*> curr_component;
			strong_connect(const_cast<Position*>(loop_iter->first), ind, st, Sset,
						comp_counter, components);

		}
	}

	return components;
}

void
DependencyGraph::print_dependency_graph()
{
	map<Position*,vector<Position*> >::iterator loop_iter;
	for (loop_iter = pred.begin(); loop_iter != pred.end(); loop_iter++)
	{
		cout << "Position: (" <<  loop_iter->first->get_b_element() << ","
		     << loop_iter->first->get_eq_index() << ")" << ": ";
		cout << "Predecessors:{";
		for (int i = 0; i < loop_iter->second.size(); i++)
		{
			cout << " Position:(" <<  loop_iter->second[i]->get_b_element() << ","
		             << loop_iter->second[i]->get_eq_index() << ")";

			if (i < loop_iter->second.size()-1)
			{
				cout << ",";
			}
		}
		cout << " }";
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
	ExpNode *formula_0_1 = new ExpNode("and", formula_0_1_leaf_0, formula_0_1_right);
	symb_E_moves[p_0_1] = formula_0_1;

	Position p_1_0(1, 0);
	ExpNode *formula_1_0_leaf_0 = new ExpNode("[b_1,0]", 0);
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

	vector<set<Position*> > strongly_connected_components = depgraph.tarjan();
	set<Position*>::iterator set_it;
	for (int i = 0; i < strongly_connected_components.size(); i++)
	{
		for (set_it = strongly_connected_components[i].begin(); set_it != strongly_connected_components[i].end(); set_it++)
		{
			cout << "(" << (*set_it)->get_b_element() << "," << (*set_it)->get_eq_index() << ")";
		}
		cout << endl;
	}
}*/
