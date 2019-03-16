#include "../include/expr_move.h"

using namespace std;

ExpNode::ExpNode(string n_name, ExpNode* l_node, ExpNode *r_node, ExpNode *r1_node , ExpNode *r2_node,
			     ExpNode *r3_node, ExpNode *r4_node):
	  node_name(n_name), left(l_node), right(r_node), right_1(r1_node), right_2(r2_node),
	  right_3(r3_node), right_4(r4_node) {}

string ExpNode::get_node_name() const{
    return node_name;
}

void ExpNode::set_node_name(string new_name){
	node_name = new_name;
}

//can be applied only on moves
void ExpNode::edit_index(ExpNode *e, string index){
	if (e){
		//if node is an atom
		if (e->get_node_name()[0] == '['){
			string atom = e->get_node_name();
			string unchanged;
			for (int i = 0; atom[i] != ','; i++){
				unchanged.push_back(atom[i]);
			}
			e->set_node_name(unchanged + "," + index + "]");
		}
		else{
			edit_index(e->left, index);
			edit_index(e->right, index);
		}
	}
}

/*can be applied only on moves;
 * moves are ordered by basis index,
 * thus the move for b_0 is at moves[0]
 */
ExpNode* ExpNode::edit_atoms(ExpNode *e, const vector<ExpNode*> &moves){
	if (e){
		if (e->get_node_name()[0] == '['){
			string atom = e->get_node_name();
			string basis_index;
			//eg) get 12 from [b_12,1]
			for (int i = 3; atom[i] != ','; i++){
				basis_index.push_back(atom[i]);
			}

			int i_basis_index = atoi(basis_index.c_str());
			//retrieve the move at the required index
			ExpNode *res = deep_clone(moves[i_basis_index]);
			return res;
		}
		else{
			//return a deep copy
			ExpNode *newleft = edit_atoms(e->left, moves);
			ExpNode *newright = edit_atoms(e->right, moves);

			return new ExpNode(e->get_node_name(), newleft, newright);
		}
	}
}

//only for moves
ExpNode* ExpNode::deep_clone(ExpNode *src){
	if (src){
		//if it is a leaf
		if (!(src->left) and !(src->right)){
			return new ExpNode(src->get_node_name(), src->left, src->right);
		}
		//any node must have two children, because the tree represent a move
		else{
			ExpNode *newleft = deep_clone(src->left);
			ExpNode *newright = deep_clone(src->right);

			return new ExpNode(src->get_node_name(), newleft, newright);
		}
	}
}

//only for moves (or equations with unary/binary operators)
void ExpNode::get_leaves(ExpNode *&e, vector<ExpNode*> &leaves_array){
	if (e){
		if (!(e->left) and !(e->right)){
			leaves_array.push_back(e);
		}
		else{
			get_leaves(e->left, leaves_array);
			if (e->right){
				get_leaves(e->right, leaves_array);
			}
		}
	}
}

void ExpNode::graft_subtree(ExpNode *&e, ExpNode *where, ExpNode *new_subtree){
	if (e){
		if (e == where){
			e = new_subtree;
		}
		else{
			graft_subtree(e->left, where, new_subtree);
			graft_subtree(e->right, where, new_subtree);
		}
	}
}

void ExpNode::print_move(ExpNode *e){
	if (!e){
		return;
	}
	else{
		if (e->left){
			cout << e->get_node_name();
			cout << "(";
			print_move(e->left);
			if (e->right){
				cout << ",";
				print_move(e->right);
			}
			cout << ")";
		}
		else{
			cout << e->get_node_name();
		}
	}
}

//print rhs of equation in infix notation
void ExpNode::print_equation_infix(ExpNode *e, FILE *handle){
	if (!e){
		return;
	}
	else{
		if (e->left and e->right){
			//cout << "(";
			fprintf(handle, "(");
			print_equation_infix(e->left, handle);
			//cout << " " << e->get_node_name();
			fprintf(handle, " %s" , e->get_node_name().c_str());
			//cout << " ";
			fprintf(handle, " ");
			print_equation_infix(e->right, handle);
			//cout << ")";
			fprintf(handle, ")");
		}
		//prefix notation for unary operators
		else if (e->left and !(e->right)){
			//cout << "(" << e->get_node_name() << " ";
			fprintf(handle, "(%s ", e->get_node_name().c_str());
			print_equation_infix(e->left, handle);
			//cout << ")";
			fprintf(handle, ")");
		}
		else{
			//cout << e->get_node_name();
			fprintf(handle, "%s", e->get_node_name().c_str());
		}
	}
}

//get maximum variable index in single equation
int ExpNode::get_max_index_in_equation(ExpNode *e){
	int max_index = -1;
	vector<ExpNode*> leaves;
	get_leaves(e, leaves);

	for (int i = 0; i < leaves.size(); i++){
		string var = leaves[i]->get_node_name();
		var.erase(0, 1);
		int var_int = atoi(var.c_str());
		if (var_int > max_index){
			max_index = var_int;
		}
	}

	return max_index;
}

Equation::Equation(string name, string type, ExpNode *expression):
	  id(name), eq_type(type), exp(expression) {}

string Equation::get_id() const{
    return id;
}

string Equation::get_eq_type() const{
    return eq_type;
}

ExpNode* Equation::get_exp_node() const{
    return exp;
}

EqSystem::EqSystem(vector<Equation*> *equation_list): equations(equation_list) {}

vector<Equation*>* EqSystem::get_equations() const{
    return equations;
}

void EqSystem::add_equation(Equation *eq){
	equations->push_back(eq);
}

//normalize a full system
vector<Equation*> EqSystem::mucalculus_normalizer_system(vector<Equation*> system){
	vector<Equation*> res;
	vector<Equation*> curr_normalized_equations = vector<Equation*>();  //store normalized equations for current iteration

	int max_index_used_in_system = -1;
	for (int i = 0; i < system.size(); i++){
		int curr_max = ExpNode::get_max_index_in_equation(system[i]->get_exp_node());
		if (curr_max > max_index_used_in_system){
			max_index_used_in_system = curr_max;
		}
	}

	for (int i = 0; i < system.size(); i++){
		ExpNode *rhs = (system[i])->get_exp_node();
		string left_id = (system[i])->get_id();
		string eq_type = (system[i])->get_eq_type();

		//store maximum index of normalized equations for current iteration
		int max_curr_normalized_equations = max_index_used_in_system;
		if (curr_normalized_equations.size() == 0){
			//max_curr_normalized_equations = ExpNode::get_max_index_in_equation(system[i]->get_exp_node());
			max_curr_normalized_equations = max_index_used_in_system;
		}
		else{
			for (int j = 0; j < curr_normalized_equations.size(); j++){
				int curr_max = ExpNode::get_max_index_in_equation(curr_normalized_equations[j]->get_exp_node());
				if (curr_max > max_curr_normalized_equations){
					max_curr_normalized_equations = curr_max;
				}
			}
		}
		if (max_curr_normalized_equations > max_index_used_in_system){
			max_index_used_in_system = max_curr_normalized_equations;
		}

		curr_normalized_equations = vector<Equation*>();
		Equation::mucalculus_normalizer_equation(rhs, left_id, eq_type, max_curr_normalized_equations, curr_normalized_equations);
		for (int k = 0; k < curr_normalized_equations.size(); k++){
			res.push_back(curr_normalized_equations[k]);
		}
	}

	return res;
}


/*eg) let x0 =min x1 and (x2 or x3) be the equation:
 *    eq_rhs is the right-hand side of the
 *    equation that has to be normalized.
 */
int Equation::mucalculus_normalizer_equation(ExpNode *eq_rhs, string left_id, string eq_type, int max_index, vector<Equation*> &equations){
	if (eq_rhs){
		Equation *new_eq; //it will be added  to 'equations'
		string new_var = "x";

		//if current node is an variable
		if (eq_rhs->get_node_name()[0] == 'x'){
			ExpNode *new_rhs = new ExpNode(eq_rhs->get_node_name(), 0, 0);
			new_eq = new Equation(left_id, eq_type, new_rhs);

			equations.push_back(new_eq);
			return max_index;
		}
		else{
			//unary operators
			if (eq_rhs->left != 0 and eq_rhs->right == 0){
				ExpNode *left_child = eq_rhs->left;
				if (left_child->get_node_name()[0] == 'x'){
					ExpNode *new_rhs = new ExpNode(eq_rhs->get_node_name(), left_child);
					new_eq = new Equation(left_id, eq_type, new_rhs);

					equations.push_back(new_eq);
					return max_index;
				}
				else if (left_child->get_node_name()[0] != 'x'){
					int curr_left_index = max_index + 1;
					string str_var_index;
					stringstream ss;
					ss << curr_left_index;
					str_var_index = ss.str();
					new_var = new_var + str_var_index;

					ExpNode *new_left = new ExpNode(new_var, 0, 0);

					ExpNode *new_rhs = new ExpNode(eq_rhs->get_node_name(), new_left, 0);

					new_eq = new Equation(left_id, eq_type, new_rhs);
					equations.push_back(new_eq);

					return 1 + mucalculus_normalizer_equation(eq_rhs->left, new_var, eq_type, max_index + 1, equations);
				}
			}

			ExpNode *left_child = eq_rhs->left;
			ExpNode *right_child = eq_rhs->right;
			//if both children are variables
			if (left_child->get_node_name()[0] == 'x' and right_child->get_node_name()[0] == 'x'){
				ExpNode *new_rhs = new ExpNode(eq_rhs->get_node_name(), left_child, right_child);
				new_eq = new Equation(left_id, eq_type, new_rhs);

				equations.push_back(new_eq);
				return max_index;
			}
			else if (left_child->get_node_name()[0] == 'x' and right_child->get_node_name()[0] != 'x'){
				int curr_right_index = max_index + 1;
				string str_var_index;
				stringstream ss;
				ss << curr_right_index;
				str_var_index = ss.str();
				new_var = new_var + str_var_index;

				ExpNode *new_right = new ExpNode(new_var, 0, 0);

				ExpNode *new_rhs = new ExpNode(eq_rhs->get_node_name(), left_child, new_right);
				new_eq = new Equation(left_id, eq_type, new_rhs);
				equations.push_back(new_eq);

				return mucalculus_normalizer_equation(eq_rhs->right, new_var, eq_type, max_index + 1, equations);
			}
			else if (left_child->get_node_name()[0] != 'x' and right_child->get_node_name()[0] == 'x'){
				int curr_left_index = max_index + 1;
				string str_var_index;
				stringstream ss;
				ss << curr_left_index;
				str_var_index = ss.str();
				new_var = new_var + str_var_index;

				ExpNode *new_left = new ExpNode(new_var, 0, 0);

				ExpNode *new_rhs = new ExpNode(eq_rhs->get_node_name(), new_left, right_child);

				new_eq = new Equation(left_id, eq_type, new_rhs);
				equations.push_back(new_eq);

				return 1 + mucalculus_normalizer_equation(eq_rhs->left, new_var, eq_type, max_index + 1, equations);
			}
			else if (left_child->get_node_name()[0] != 'x' and right_child->get_node_name()[0] != 'x'){
				int curr_left_index = max_index + 1;
				string str_var_index;
				stringstream ss;
				ss << curr_left_index;
				str_var_index = ss.str();
				new_var = new_var + str_var_index;

				ExpNode *new_left = new ExpNode(new_var, 0, 0);
				int max_index_left = mucalculus_normalizer_equation(eq_rhs->left, new_var, eq_type, max_index + 1, equations);

				int curr_right_index = max_index_left + 1;
				string new_var_1 = "x";
				stringstream ss_1;
				ss_1 << curr_right_index;
				str_var_index = ss_1.str();
				new_var_1 = new_var_1 + str_var_index;

				ExpNode *new_right = new ExpNode(new_var_1, 0, 0);

				ExpNode *new_rhs = new ExpNode(eq_rhs->get_node_name(), new_left, new_right);

				new_eq = new Equation(left_id, eq_type, new_rhs);
				equations.push_back(new_eq);

				return mucalculus_normalizer_equation(eq_rhs->right, new_var_1, eq_type, max_index_left + 1, equations);
			}
		}
	}
}
