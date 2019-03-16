/* Move Composer: accepts a tree representing an equation
 *                (with any number of operators),
 *                and generates its (composite) symbolic
 *                E-move as a tree of moves.
 */

#include "../include/move_composer.h"

using namespace std;

void print_composite_move(ExpNode *t){
	if (!t){
		return;
	}
	else{
		if (t->left and t->right){
			cout << t->get_node_name();
			cout << "(";
			print_composite_move(t->left);
			cout << ",";
			print_composite_move(t->right);
			cout << ")";
		}
		else{
			cout << t->get_node_name();
		}
	}
}

string MoveComposer::extract_index_from_phi(string phi_id){
	string res;
	for (int i = 4; phi_id[i] != '_'; i++){
		res.push_back(phi_id[i]);
	}

	return res;
}

string MoveComposer::extract_operator_from_phi(string phi_id){
	string res;
	int i;
	for (i = 4; phi_id[i] != '_'; i++){}

	//the operator's name starts at index 'begin'
	int begin_of_name = i + 1;

	for (string::iterator iter = phi_id.begin() + begin_of_name; iter != phi_id.end(); iter++){
		res.push_back(*iter);
	}

	return res;
}

string MoveComposer::extract_index_from_variable(string x_id){
	string res;

	for (string::iterator iter = x_id.begin() + 1; iter != x_id.end(); iter++){
		res.push_back(*iter);
	}

	return res;
}

string MoveComposer::extract_basis_from_atom(string atom){
	string res;

	int i;
	for (i = 3; atom[i] != ','; i++){
		res.push_back(atom[i]);
	}

	return res;
}

string MoveComposer::extract_index_from_atom(string atom){
	string res;

	int i;
	for (i = 3; atom[i] != ','; i++){}

	int begin_of_index = i + 1;

	for (i = begin_of_index; atom[i] != ']'; i++){
		res.push_back(atom[i]);
	}

	return res;
}


ExpNode* MoveComposer::single_composer(ExpNode *eq, int basis_element) const{
	if (eq){
		string curr_node_name = eq->get_node_name();
		//base case: variable, thus leaf
		if (curr_node_name[0] == 'x'){
			/*build a string containing the atom that corresponds to the current variable:
			 *i.e. let x12 be the variable, then the atom is [b_basis_element, 12]
			 */
			string str_var_index = extract_index_from_variable(curr_node_name);

			string str_basis_element;
			stringstream ss;
			ss << basis_element;
			str_basis_element = ss.str();

			string bracket = "[";
			string atom = bracket + "b" + "_" + str_basis_element + "," + str_var_index + "]";

			return new ExpNode(atom, 0);
		}
		//base case: propositional variable, thus leaf (true/false move)
		else if (curr_node_name[0] == 'p'){
			ExpNode *curr_move;  //a (deep) copy of the basic move of current operator
			//search for operator
			for (int i = 0; i < operators.size(); i++){
				if (curr_node_name == operators[i].get_name()
					and basis_element == operators[i].get_basis_element()){
					curr_move = operators[i].get_formula();
				}
			}
			
			return new ExpNode(curr_move->get_node_name(), 0);
		}
		else{
			ExpNode *curr_move;  //a (deep) copy of the basic move of current operator
			//search for operator
			for (int i = 0; i < operators.size(); i++){
				if (curr_node_name == operators[i].get_name()
					and basis_element == operators[i].get_basis_element()){
					curr_move = operators[i].get_formula();
				}
			}

			ExpNode *curr_move_copy = ExpNode::deep_clone(curr_move);

			//retrieve curr_move's leaves as an array
			vector<ExpNode*> leaves;
			ExpNode::get_leaves(curr_move_copy, leaves);

			/*loop on each leaf (they are obviously atoms), get basis element's index and equation index
			  current expression's node is 'eq'
			  the child on which the recursive call is performed depends on 'ind'
			 */
			for (int i = 0; i < leaves.size(); i++){
				string atom = leaves[i]->get_node_name();

				string bas_str = extract_basis_from_atom(atom);
				string ind_str = extract_index_from_atom(atom);

				int bas = atoi(bas_str.c_str());
				int ind = atoi(ind_str.c_str());

				ExpNode *expanded_formula;

				switch(ind){
					case 0:
						expanded_formula = single_composer(eq->left, bas);
					break;
					case 1:
						expanded_formula = single_composer(eq->right, bas);
					break;
					case 2:
						expanded_formula = single_composer(eq->right_1, bas);
					break;
					case 3:
						expanded_formula = single_composer(eq->right_2, bas);
					break;
					case 4:
						expanded_formula = single_composer(eq->right_3, bas);
					break;
					case 5:
						expanded_formula = single_composer(eq->right_4, bas);
					break;
				}

				ExpNode::graft_subtree(curr_move_copy, leaves[i], expanded_formula);
			}

			return curr_move_copy;
	    }
	}
}


MoveComposer::MoveComposer(vector<Equation*> *eq_mv, int basis_size){
	//initialize the field
	equations_moves = eq_mv;
	this->basis_size = basis_size;

	//loop on eq_mv, extract moves and put them into operators
	for (unsigned int i = 0; i < eq_mv->size(); i++){
		Equation* curr_equation = (*eq_mv)[i];
		//check if equation begins with a phi identifier
		if ((curr_equation->get_id()).substr(0,3) == "phi"){
			//get the index of the basis element
			string str_index = extract_index_from_phi(curr_equation->get_id());
			int index = atoi(str_index.c_str());

			//get its name
			string name = extract_operator_from_phi(curr_equation->get_id());
			//look at the ExpNode to get the move
			ExpNode *rhs = curr_equation->get_exp_node();

			Operator op(name, index, rhs);
			//add the current operator
			operators.push_back(op);
		}
	}
}


vector<Operator> MoveComposer::get_operators() const{
	return operators;
}

map<Position, ExpNode*> MoveComposer::system_composer() const{
	/*map a position to its composite move:
	  eg) the move for basis element = 0, equation index = 0 is etc...
	 */
	map<Position, ExpNode*> composite_moves_map;
	int equation_index = 0;  //count the equations in 'equations_moves'

	for (int i = 0; i < equations_moves->size(); i++){
		if ((*equations_moves)[i]->get_id()[0] == 'x'){
			//loop over basis elements, because we need a list of moves for each basis element
			for (int basis_elem = 0; basis_elem < basis_size; basis_elem++){
				Position key(basis_elem, equation_index);
				ExpNode *composite_move = single_composer((*equations_moves)[i]->get_exp_node(), basis_elem);

				composite_moves_map[key] = composite_move;
			}

			equation_index++;
		}
	}

	/*final map will be something like:
	 * {(0,0): composite_move_tree_0_0, (0,1): composite_move_tree_0_1 etc...}
	 *
	 */

	return composite_moves_map;
}



/*int main(){
	yyparse();   //run the parser
    vector<Equation*> *eqs = root->get_equations();  //get system + moves

	MoveComposer mc(eqs, 3);*/

	//TEST 1: "bop" operator
	/*ExpNode *move_b0_bop_root = new ExpNode("or", 0);
	ExpNode *move_b0_bop_left = new ExpNode("and", 0);
	move_b0_bop_root->left = move_b0_bop_left;
	ExpNode *move_b0_bop_leaf_1 = new ExpNode("[b_0,0]", 0);
	ExpNode *move_b0_bop_leaf_2 = new ExpNode("[b_1,1]", 0);
	ExpNode *move_b0_bop_leaf_3 = new ExpNode("[b_2,0]", 0);
	move_b0_bop_root->right = move_b0_bop_leaf_3;
	move_b0_bop_left->left = move_b0_bop_leaf_1;
	move_b0_bop_left->right = move_b0_bop_leaf_2;

	ExpNode *move_b1_bop_root = new ExpNode("or", 0);
	ExpNode *move_b1_bop_leaf_1 = new ExpNode("[b_1,0]", 0);
	ExpNode *move_b1_bop_leaf_2 = new ExpNode("[b_2,1]", 0);
	move_b1_bop_root->left = move_b1_bop_leaf_1;
	move_b1_bop_root->right = move_b1_bop_leaf_2;

	ExpNode *move_b2_bop_root = new ExpNode("and", 0);
	ExpNode *move_b2_bop_right = new ExpNode("or", 0);
	move_b2_bop_root->right = move_b2_bop_right;
	ExpNode *move_b2_bop_leaf_1 = new ExpNode("[b_0,0]", 0);
	ExpNode *move_b2_bop_leaf_2 = new ExpNode("[b_1,1]", 0);
	ExpNode *move_b2_bop_leaf_3 = new ExpNode("[b_2,0]", 0);
	move_b2_bop_root->left = move_b2_bop_leaf_1;
	move_b2_bop_right->left = move_b2_bop_leaf_2;
	move_b2_bop_right->right = move_b2_bop_leaf_3;

	Operator op1("bop", 0, move_b0_bop_root);
	Operator op2("bop", 1, move_b1_bop_root);
	Operator op3("bop", 2, move_b2_bop_root);

	ExpNode *exp_leaf_1 = new ExpNode("x1", 0);
	ExpNode *exp_leaf_2 = new ExpNode("x2", 0);
	ExpNode *exp_leaf_3 = new ExpNode("x3", 0);
	ExpNode *exp_leaf_4 = new ExpNode("x4", 0);
	ExpNode *exp_left = new ExpNode("bop", exp_leaf_1, exp_leaf_2);
	ExpNode *exp_right = new ExpNode("bop", exp_leaf_3, exp_leaf_4);
	ExpNode *exp_root = new ExpNode("bop", exp_left, exp_right);

	mc.operators.push_back(op1);
	mc.operators.push_back(op2);
	mc.operators.push_back(op3);

	ExpNode *res = mc.single_composer(exp_root, 0);
	cout << endl;
	print_composite_move(res);
	cout << endl;*/

	//TEST 2: plain boolean operators
	/*ExpNode *move_b0_and_leaf_1 = new ExpNode("[b_0,0]", 0);
	ExpNode *move_b0_and_leaf_2 = new ExpNode("[b_0,1]", 0);
	ExpNode *move_b0_and_root = new ExpNode("and", move_b0_and_leaf_1, move_b0_and_leaf_2);

	ExpNode *move_b0_or_leaf_1 = new ExpNode("[b_0,0]", 0);
	ExpNode *move_b0_or_leaf_2 = new ExpNode("[b_0,1]", 0);
	ExpNode *move_b0_or_root = new ExpNode("or", move_b0_or_leaf_1, move_b0_or_leaf_2);

	Operator op1("and", 0, move_b0_and_root);
	Operator op2("or", 0, move_b0_or_root);

	mc.operators.push_back(op1);
	mc.operators.push_back(op2);

	ExpNode *exp_leaf_1 = new ExpNode("x1", 0);
	ExpNode *exp_leaf_2 = new ExpNode("x2", 0);
	ExpNode *exp_leaf_3 = new ExpNode("x3", 0);
	ExpNode *exp_leaf_4 = new ExpNode("x4", 0);
	ExpNode *exp_left = new ExpNode("and", exp_leaf_1, exp_leaf_2);
	ExpNode *exp_right = new ExpNode("and", exp_leaf_3, exp_leaf_4);
	ExpNode *exp_root = new ExpNode("or", exp_leaf_1, exp_right);

	ExpNode *res = mc.single_composer(exp_root, 0);
	cout << endl;
	print_composite_move(res);
	cout << endl;*/


	//TEST 3: unary "box" operator
	/*ExpNode *move_b0_and_leaf_1 = new ExpNode("[b_0,0]", 0);
	ExpNode *move_b0_and_leaf_2 = new ExpNode("[b_0,1]", 0);
	ExpNode *move_b0_and_root = new ExpNode("and", move_b0_and_leaf_1, move_b0_and_leaf_2);

	ExpNode *move_b1_and_leaf_1 = new ExpNode("[b_1,0]", 0);
	ExpNode *move_b1_and_leaf_2 = new ExpNode("[b_1,1]", 0);
	ExpNode *move_b1_and_root = new ExpNode("and", move_b1_and_leaf_1, move_b1_and_leaf_2);

	ExpNode *move_b0_box_leaf_1 = new ExpNode("[b_0,0]", 0);
	ExpNode *move_b0_box_leaf_2 = new ExpNode("[b_1,0]", 0);
	ExpNode *move_b0_box_root = new ExpNode("and", move_b0_box_leaf_1, move_b0_box_leaf_2);

	Operator op1("and", 0, move_b0_and_root);
	Operator op2("and", 1, move_b1_and_root);
	Operator op3("box", 0, move_b0_box_root);

	mc.operators.push_back(op1);
	mc.operators.push_back(op2);
	mc.operators.push_back(op3);


	ExpNode *exp_leaf_3 = new ExpNode("x3", 0);
	ExpNode *exp_leaf_4 = new ExpNode("x4", 0);
	ExpNode *exp_left = new ExpNode("and", exp_leaf_3, exp_leaf_4);
	ExpNode *exp_root = new ExpNode("box", exp_left);

	ExpNode *res = mc.single_composer(exp_root, 0);
	cout << endl;
	print_composite_move(res);
	cout << endl;*/

/*	return 0;
}*/
