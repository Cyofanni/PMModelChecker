#include "../include/input_checker.h"

//to be applied on actual equations, not on moves
void InputChecker::find_used_operators_equation(ExpNode *rhs, set<string> &operators) const{
	if (rhs){
		//if current node is not a variable (then it's an operator)
		if (rhs->get_node_name()[0] != 'x'){   //variables begin with the 'x' character
			operators.insert(rhs->get_node_name());
			//handle any arity (up to 6)
			find_used_operators_equation(rhs->left, operators);
			find_used_operators_equation(rhs->right, operators);
			find_used_operators_equation(rhs->right_1, operators);
			find_used_operators_equation(rhs->right_2, operators);
			find_used_operators_equation(rhs->right_3, operators);
			find_used_operators_equation(rhs->right_4, operators);
		}
	}
}

vector<string> InputChecker::get_phi_identifiers() const{
	vector<string> used_phi_id_vec; //vector of 'phi' identifier in the input
	for (int i = 0; i < eq_mv->size(); i++){
		//first 3 characters must be 'p', 'h', 'i'
		if (((*eq_mv)[i]->get_id()).compare(0, 3, "phi") == 0){
			used_phi_id_vec.push_back((*eq_mv)[i]->get_id());
		}
	}

	return used_phi_id_vec;
}

set<string> InputChecker::find_used_operators_system() const{
	set<string> used_operators;
	for (int i = 0; i < eq_mv->size(); i++){
		//if it is an actual equation
		if ((*eq_mv)[i]->get_id()[0] == 'x'){
			find_used_operators_equation((*eq_mv)[i]->get_exp_node(), used_operators);  //modifies 'used_operators'
		}
	}

	return used_operators;
}

InputChecker::InputChecker(vector<Equation*> *eq_mv, int basis_size){
	this->eq_mv = eq_mv;
	this->basis_size = basis_size;
}

vector<string> InputChecker::get_missing_moves(){
	set<string> used_operators = find_used_operators_system();
	set<string>::iterator op_it;
	vector<string> needed_phi_id_vec;  //contains needed phi identifiers

	for (op_it = used_operators.begin(); op_it != used_operators.end(); op_it++){
		for (int i = 0; i < basis_size; i++){
			//create the following needed identifier: phi_i_op
			string needed_phi_id = "phi_";

			//convert 'i' into a string
			string str_i;
			stringstream ss;
			ss << i;
			str_i = ss.str();

			needed_phi_id = needed_phi_id + str_i + "_" + *op_it;

			needed_phi_id_vec.push_back(needed_phi_id);
		}
	}

	vector<string> used_phi_id_vec = get_phi_identifiers();  //contains used phi identifiers

	vector<string> missing_phi_identifiers = vector<string>();
	//loop on needed identifers
	for (int i = 0; i < needed_phi_id_vec.size(); i++){
		//loop on used identifiers
		bool found = false;
		for (int j = 0; j < used_phi_id_vec.size() and !found; j++){
			if (needed_phi_id_vec[i] == used_phi_id_vec[j]){
				found = true;
			}
		}
		if (found == false){
			//store missing identifier in the returned vector
			missing_phi_identifiers.push_back(needed_phi_id_vec[i]);
		}
	}

	return missing_phi_identifiers;
}
