#include "../include/solver.h"

Solver::Solver() {}

Solver::Solver(const map<int,string>& mmeq, int bs, int ss, const map<Position, ExpNode*>& s_em, int lh)
{
	min_max_eq = mmeq;
	basis_size = bs;
	system_size = ss;
	symb_E_moves = s_em;
	lattice_height = lh;
}

void
Solver::comp_symb_prog_meas(const Position &p, ExpNode *formula,
                                 const vector<int> &delta, const vector< vector< vector<int> > > &prog_meas_matr, vector<int> &pm_value)
{
	//base case
	if (!(formula->left) and !(formula->right))
	{
		string atom = formula->get_node_name();
		if (atom != "true" and atom != "false")
		{   //if it is an atom like [b_0,1] etc...
			//get indices of the logical atom
			string h_str = MoveComposer::extract_basis_from_atom(atom);
			string k_str = MoveComposer::extract_index_from_atom(atom);

			int h = atoi(h_str.c_str());
			int k = atoi(k_str.c_str());

			if (!is_top(prog_meas_matr[h][k]))
			{
				for (int i = p.get_eq_index(); i < system_size; i++)
				{
					pm_value[i] = prog_meas_matr[h][k][i];
				}
				for (int i = 0; i < p.get_eq_index(); i++)
				{
					pm_value[i] = 0;       //0's before equation index
				}
				for (int i = p.get_eq_index(); i < system_size; i++)
				{
					pm_value[i] = pm_value[i] + delta[i];
				}
			}
			else if (is_top(prog_meas_matr[h][k]))
			{
				for (int i = 0; i < system_size; i++)
				{
					pm_value[i] = lattice_height + 1;     //return a * value
				}
			}

			return;
		}
		else if (atom == "true")
		{  //if it is a 'true' 'atom'
			for (int i = 0; i < system_size; i++)
			{
				pm_value[i] = 0;
			}

			return;
		}
		else if (atom == "false")
		{  //if it is a 'true' 'atom'
			for (int i = 0; i < system_size; i++)
			{
				pm_value[i] = INT_MAX;
			}

			return;
		}
	}
	//recursive cases
	else
	{
		vector<int> pm_value_1(system_size, 0);
		comp_symb_prog_meas(p, formula->left, delta, prog_meas_matr, pm_value);
		comp_symb_prog_meas(p, formula->right, delta, prog_meas_matr, pm_value_1);

		if (formula->get_node_name() == "or")
		{	//it's a minimum
			if (lex_vectors(pm_value, pm_value_1, 0))
			{
				//pm_value = pm_value;   useless statement, written for clarity
			}
			else
			{
				pm_value = pm_value_1;
			}
		}
		else if (formula->get_node_name() == "and")
		{  //it's a maximum
			if (lex_vectors(pm_value, pm_value_1, 0))
			{
				pm_value = pm_value_1;
			}
			else
			{
				//pm_value = pm_value;   useless statement, written for clarity
			}
		}
	}
}

//return 'true' if a given R(b)(i) has the Top value ('star')
bool
Solver::is_top(const vector<int> &pm_value) const
{
	bool istop = false;
	for (int i = 0; i < pm_value.size(); i++)
	{
		if (pm_value[i] > lattice_height)
		{
			istop = true;
			break;
		}
	}

	return istop;
}

vector< vector < vector<int> > >
Solver::solve_system_worklist()
{
	vector< vector < vector<int> > > prog_meas_matr(basis_size, vector<vector<int> >(system_size, vector<int>(system_size)));

	DependencyGraph depgraph_obj(symb_E_moves, basis_size, system_size);
	map<Position,vector<Position> > depgraph = depgraph_obj.get_dependency_graph();

	for (int i = 0; i < basis_size; i++)
	{	//loop on basis elements, +1 because empty set is added
		for (int j = 0; j < system_size; j++)   //loop on number of equations
		{
			for (int k = 0; k < system_size; k++)
			{  //loop on number of equations
				prog_meas_matr[i][j][k] = 0;
			}
		}
	}

	vector<Position> worklist(basis_size*system_size, Position(0,0));
	for (int i = 0; i < basis_size; i++)
	{
		for (int j = 0; j < system_size; j++)
		{
			Position p(i, j);
			worklist[i*system_size + j] = p;
		}
	}

	int iteration_counter = 0;

	while (worklist.size() > 0)
	{
		Position v = worklist[0];
		worklist.erase(worklist.begin());
		int j = v.get_b_element();
		int k = v.get_eq_index();

		//work on prog_meas_matr[j][k] only if it has not the top value
		if (!is_top(prog_meas_matr[j][k]))
		{
			vector<int> y(system_size, 0);

			//compute the delta
			vector<int> delta;
			map<int,string>::const_iterator it = min_max_eq.find(v.get_eq_index());
			string fixpoint = it->second;   //"=min" or "=max"
			if (fixpoint == "=max")
			{		//null vector
				for (int c = 0; c < system_size; c++)
				{
					delta.push_back(0);
				}
			}
			else if (fixpoint == "=min")
			{	//some 1's
				for (int c = 0; c < system_size; c++)
				{
					if (c != v.get_eq_index())
					{
						delta.push_back(0);
					}
					else
					{
						delta.push_back(1);
					}
				}
			}

			//get the formula corresponding to current position 'v'
			map<Position,ExpNode*>::const_iterator formula_iter = symb_E_moves.find(v);
			ExpNode *formula = formula_iter->second;
			/*cout << "(" << v.get_b_element() << "," << v.get_eq_index() << ")" << endl;
			ExpNode::print_move(formula);
			cout << endl;*/
			vector<int> pm_value(system_size, 0);	//store progress measure's value

			comp_symb_prog_meas(v, formula, delta, prog_meas_matr, pm_value);    //compute functional Phi_j_k
			y = pm_value;

			if (y != prog_meas_matr[j][k])
			{
				//if y is a 'top vector', then obviously  y != prog_meas_matr[j][k]
				/*if (is_top(y)){
					for (int c = 0; c < system_size; c++){
						prog_meas_matr[j][k][c] = lattice_height + 1;
					}
				}*/
				//else{
					Position p(j, k);
					map<Position,vector<Position> >::const_iterator iter = depgraph.find(p);  //find dependencies of (b_j, k)
					if (iter != depgraph.end())
					{
						for (int i = 0; i < iter->second.size(); i++)
						{   //loop on dependencies of (b_j, k)
							Position p1 = iter->second[i];
							//worklist.insert(worklist.begin(), p1);   //add each dependency to the worklist
							worklist.push_back(p1);
						}
						prog_meas_matr[j][k] = y;
					}
				//}
			}
		}
		else if (is_top(prog_meas_matr[j][k]))
		{
			for (int c = 0; c < system_size; c++)
			{
				prog_meas_matr[j][k][c] = lattice_height + 1;
			}
		}
		/*cout << "CURRENT MATRIX IS:" << endl;
		for (int i = 0; i < basis_size; i++){	//loop on basis elements
			for (int j = 0; j < system_size; j++)   //loop on number of equations
			{
				cout << "[ ";
				for (int k = 0; k < system_size; k++){  //loop on number of equations
					cout << prog_meas_matr[i][j][k] << " ";
				}
				cout << "]";
			}
			cout << endl;
		}
		cout << "**************************************" << endl << endl;*/

		iteration_counter++;
	}

	cout << "WORKLIST'S NUMBER OF ITERATIONS: " << iteration_counter << endl;
	return prog_meas_matr;
}


vector< vector < vector<int> > >
Solver::solve_system_chaotic_iteration()
{
	vector< vector < vector<int> > > prog_meas_matr(basis_size, vector<vector<int> >(system_size, vector<int>(system_size)));

	for (int i = 0; i < basis_size; i++)
	{	//loop on basis elements
		for (int j = 0; j < system_size; j++)   //loop on number of equations
		{
			for (int k = 0; k < system_size; k++)
			{  //loop on number of equations
				prog_meas_matr[i][j][k] = 0;
			}
		}
	}

	vector<vector<int> > temp_vars(basis_size*system_size, vector<int>(system_size));

	int iteration_counter = 0;

	bool changed = true;
	while (changed)
	{
		/*if (iteration_counter > 50){
			break;
		}*/
		for (int i = 0; i < basis_size; i++)
		{
			for (int j = 0; j < system_size; j++)
			{
				temp_vars[i*system_size + j] = prog_meas_matr[i][j];
			}
		}

		for (int i = 0; i < basis_size; i++)
		{
			for (int j = 0; j < system_size; j++)
			{
				if (!is_top(prog_meas_matr[i][j]))
				{
					Position p(i, j);
					//compute the delta
					vector<int> delta;
					map<int,string>::const_iterator it = min_max_eq.find(p.get_eq_index());
					string fixpoint = it->second;   //"=min" or "=max"
					if (fixpoint == "=max")
					{		//null vector
						for (int k = 0; k < system_size; k++)
						{
							delta.push_back(0);
						}
					}
					else if (fixpoint == "=min")
					{	//some 1's
						for (int k = 0; k < system_size; k++)
						{
							if (k != p.get_eq_index())
							{
								delta.push_back(0);
							}
							else
							{
								delta.push_back(1);
							}
						}
					}
					map<Position,ExpNode*>::const_iterator formula_iter = symb_E_moves.find(p);
					ExpNode *formula = formula_iter->second;
					vector<int> pm_value(system_size, 0);	//store progress measure's value
					comp_symb_prog_meas(p, formula, delta, prog_meas_matr, pm_value);

					/*if (is_top(pm_value)){
						for (int c = 0; c < system_size; c++){
							prog_meas_matr[i][j][c] = lattice_height + 1;
						}
					}*/
					//else{
						prog_meas_matr[i][j] = pm_value;
					//}
				}
				else if (is_top(prog_meas_matr[i][j]))
				{
					for (int c = 0; c < system_size; c++)
					{
						prog_meas_matr[i][j][c] = lattice_height + 1;
					}
				}
			}
		}

		changed = false;
		for (int i = 0; i < basis_size and changed == false; i++)
		{
			for (int j = 0; j < system_size; j++)
			{
				if(temp_vars[i*system_size + j] != prog_meas_matr[i][j])
				{
					changed = true;
				}
			}
		}

		/*cout << "CURRENT MATRIX IS:" << endl;
		for (int i = 0; i < basis_size; i++){	//loop on basis elements
			for (int j = 0; j < system_size; j++)   //loop on number of equations
			{
				cout << "[ ";
				for (int k = 0; k < system_size; k++){  //loop on number of equations
					cout << prog_meas_matr[i][j][k] << " ";
				}
				cout << "]";
			}
			cout << endl;
		}
		cout << "**************************************" << endl << endl;*/


		/*if (changed == false){
			break;
		}*/

		iteration_counter++;
	}

	cout << "CHAOTIC NUMBER OF ITERATIONS: " << iteration_counter << endl;
	return prog_meas_matr;
}


bool
Solver::lex_vectors(vector<int> a, vector<int> b, unsigned int from)
{
	//want to know whether a is less (strictly) than b
	if (a[from] < b[from])
	{
		return true;
	}
	else if (a[from] > b[from])
	{
		return false;
	}
	else
	{
		if (from < a.size()-1)
		{
			return lex_vectors(a, b, from+1);
		}
		else
		{
			return false;
		}
	}
}

void
Solver::print_pm_matrix(vector< vector < vector<int> > > pm_matrix, int bs, int ss)
{
	cout << "PROGRESS MEASURE MATRIX IS:" << endl;
	for (int i = 0; i < bs; i++)
	{	//loop on basis elements
		for (int j = 0; j < ss; j++)
		{
			cout << "[ ";
			for (int k = 0; k < ss; k++)
			{  //loop on number of equations
				cout << pm_matrix[i][j][k] << " ";
			}
			cout << "]";
		}
		cout << endl;
	}
}

void
Solver::pretty_print_pm_matrix(vector< vector < vector<int> > > pm_matrix, int bs, int ss,
										int lh, const vector<string> &basis_names)
{
	cout << "PROGRESS MEASURE MATRIX IS:" << endl;
	for (int i = 0; i < bs; i++)
	{	//loop on basis elements
		for (int j = 0; j < ss; j++)
		{
			cout << "[";
			bool istop = false;
			for (int k = 0; k < ss and !istop; k++)
			{
				if (pm_matrix[i][j][k] > lh)
				{
					istop = true;
				}
			}
			if (istop)
			{
				cout << "DON'T KNOW (*)";
			}
			else
			{
				cout << basis_names[i] << " <= " << "u_" << j;
			}
			cout << "]";
		}
		cout << endl;
	}
}
