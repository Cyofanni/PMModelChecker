//main file
#include "../include/move_composer.h"
#include "../include/solver.h"
#include "../include/input_checker.h"
#include "../include/mu_calculus_move_generator.h"
#include <stdio.h>
#include <string.h>



int main(int argc, char *argv[]){
	/*"General Mode":
		./pm_model_checker -ge system.txt basis_size
		in this mode, all the moves are provided by the user, and the
		lts file is not needed.
	 */

	/*"Mu-calculus Mode":
		./pm_model_checker -mu system.txt basis_size lts.aut
		(observe the -mu flag)
		in this mode, the tool appends automatically the basic symbolic E-moves
		for 'diamond' and 'box' operators to the file "system.txt", which is
		parsed later.
	 */

	if (argc < 2){
		fprintf(stderr, "usage: ./pm_model_checker [flag] [args]\n");
		return 1;
	}
	else{
		if (argc < 4){
			fprintf(stderr, "usage: ./pm_model_checker [flag] [args]\n");
			return 1;
		}
		else{
			if (strcmp(argv[1], "-ge") == 0){
				yyin = fopen(argv[2], "r");
				if (yyin){
					//run the parser and check if parsing succeeded
					if (yyparse() != 0){
						return 1;
					}

					vector<Equation*> *eqs_moves = root->get_equations();  //get system + moves

					//input sanitization
					InputChecker ic(eqs_moves, atoi(argv[3]));
					vector<string> missing_moves = ic.get_missing_moves();

					if (missing_moves.size() == 0){
						map<int,string> min_max_eq; //type o each equation ("=min" or "=max")

						int eq_counter = 0; //count actual equations (like x1 =min etc...) and discard moves
						for (int i = 0; i < eqs_moves->size(); i++){
							if ((*eqs_moves)[i]->get_id()[0] == 'x'){
								string min_or_max = (*eqs_moves)[i]->get_eq_type();
								min_max_eq[eq_counter];
								min_max_eq[eq_counter] = min_or_max;
								eq_counter++;
							}
						}

						MoveComposer mv_comp(eqs_moves, atoi(argv[3]));
						map<Position,ExpNode*> composite_moves = mv_comp.system_composer();
						map<Position,ExpNode*>::iterator loop_iter;
						for (loop_iter = composite_moves.begin(); loop_iter != composite_moves.end(); loop_iter++){
							cout << "Position:" << "(" << (loop_iter->first).get_b_element() << "," << (loop_iter->first).get_eq_index() << "):";
							cout << "  Move: ";
							ExpNode::print_equation_infix(loop_iter->second, stdout);
							cout << endl;
						}
						cout << endl;

						//create a Solver object and solve the system
						Solver slv(min_max_eq, atoi(argv[3]), min_max_eq.size(), composite_moves, 1);
						vector< vector < vector<int> > > final_matrix_worklist = slv.solve_system_worklist();
						cout << "WORKLIST'S OUTCOME:" << endl;
						Solver::print_pm_matrix(final_matrix_worklist, atoi(argv[3]), eq_counter);

						cout << endl << endl;
						vector< vector < vector<int> > > final_matrix_chaotic = slv.solve_system_chaotic_iteration();
						cout << "CHAOTIC ITERATION'S OUTCOME:" << endl;
						Solver::print_pm_matrix(final_matrix_chaotic, atoi(argv[3]), eq_counter);
					}

					else{
						for (int i = 0; i < missing_moves.size(); i++){
							fprintf(stderr, "error: missing move: ");
							cout << missing_moves[i] << endl;
						}
						return 1;
					}
				}
				else{
					fprintf(stderr, "error: file with equational system not found\n");
					return 1;
				}
    		}

    		else if (strcmp(argv[1], "-mu") == 0){
				try{
					MuCalculusMoveGenerator muc_gen(argv[4], atoi(argv[3]));
					vector<string> box_moves = muc_gen.generate_box_diamond_move(true);
					vector<string> diamond_moves = muc_gen.generate_box_diamond_move(false);
					yyin = fopen(argv[2], "a");
					//add computed moves to input system
					if (yyin){
						for (int i = 0; i < box_moves.size(); i++){
							const char *curr_box_move = box_moves[i].c_str();
							const char *curr_diamond_move = diamond_moves[i].c_str();
							fprintf(yyin, "%s\n", curr_box_move);
							fprintf(yyin, "%s\n", curr_diamond_move);
						}
						fclose(yyin);
						yyin = fopen(argv[2], "r");
						//then parse it as usual
						//run the parser and check if parsing succeeded
						if (yyparse() != 0){
							return 1;
						}

						vector<Equation*> *eqs_moves = root->get_equations();  //get system + moves

						//input sanitization
						InputChecker ic(eqs_moves, atoi(argv[3]));
						vector<string> missing_moves = ic.get_missing_moves();

						if (missing_moves.size() == 0){
							map<int,string> min_max_eq; //type o each equation ("=min" or "=max")

							int eq_counter = 0; //count actual equations (like x1 =min etc...) and discard moves
							for (int i = 0; i < eqs_moves->size(); i++){
								if ((*eqs_moves)[i]->get_id()[0] == 'x'){
									string min_or_max = (*eqs_moves)[i]->get_eq_type();
									min_max_eq[eq_counter];
									min_max_eq[eq_counter] = min_or_max;
									eq_counter++;
								}
							}

							MoveComposer mv_comp(eqs_moves, atoi(argv[3]));
							map<Position,ExpNode*> composite_moves = mv_comp.system_composer();
							map<Position,ExpNode*>::iterator loop_iter;
							for (loop_iter = composite_moves.begin(); loop_iter != composite_moves.end(); loop_iter++){
								cout << "Position:" << "(" << (loop_iter->first).get_b_element() << "," << (loop_iter->first).get_eq_index() << "):";
								cout << "  Move: ";
								ExpNode::print_move(loop_iter->second);
								cout << endl;
							}
							cout << endl;

							//create a Solver object and solve the system
							Solver slv(min_max_eq, atoi(argv[3]), min_max_eq.size(), composite_moves, 1);
							vector< vector < vector<int> > > final_matrix_worklist = slv.solve_system_worklist();
							cout << "WORKLIST'S OUTCOME:" << endl;
							Solver::print_pm_matrix(final_matrix_worklist, atoi(argv[3]), eq_counter);

							cout << endl << endl;
							vector< vector < vector<int> > > final_matrix_chaotic = slv.solve_system_chaotic_iteration();
							cout << "CHAOTIC ITERATION'S OUTCOME:" << endl;
							Solver::print_pm_matrix(final_matrix_chaotic, atoi(argv[3]), eq_counter);
						}

						else{
							for (int i = 0; i < missing_moves.size(); i++){
								fprintf(stderr, "error: missing move: ");
								cout << missing_moves[i] << endl;
							}
							return 1;
						}
					}
					else{
						fprintf(stderr, "error: file with equational system not found\n");
						return 1;
					}
				}
				catch(AldebaranNotFoundException a){
					fprintf(stderr, "error: lts file not found\n");
					return 1;
				}
			}
			else if (strcmp(argv[1], "-normalize") == 0){
				yyin = fopen(argv[2], "r");
				FILE *normalization_output = fopen(argv[3], "w");
				if (yyin){
					//run the parser and check if parsing succeeded
					if (yyparse() != 0){
						return 1;
					}

					vector<Equation*> *eqs = root->get_equations();  //get system + moves
					vector<Equation*> normalized_equations;
					/*ExpNode *rhs = ((*eqs)[0])->get_exp_node();
					string left_id = ((*eqs)[0])->get_id();
					string eq_type = ((*eqs)[0])->get_eq_type();
					ExpNode::print_equation_infix(rhs);
					cout << endl;
					ExpNode::print_equation_infix(((*eqs)[1])->get_exp_node());
					cout << endl;
					ExpNode::print_equation_infix(((*eqs)[2])->get_exp_node());
					cout << endl;*/

					vector<Equation*> normalized_system = EqSystem::mucalculus_normalizer_system(*eqs);

					for (int i = 0; i < normalized_system.size(); i++){
						ExpNode *rhs_1 = (normalized_system[i])->get_exp_node();
						string left_id_1 = (normalized_system[i])->get_id();
						string eq_type_1 = (normalized_system[i])->get_eq_type();

						const char *left_id_file = left_id_1.c_str();
						const char *eq_type_file = eq_type_1.c_str();
						fprintf(normalization_output, "%s ", left_id_file);
						fprintf(normalization_output, "%s ", eq_type_file);
						ExpNode::print_equation_infix(rhs_1, normalization_output);
						fprintf(normalization_output, "\n");
					}
				}
			}


			else{
				fprintf(stderr, "error: wrong flag, use -ge or -mu\n");
				return 1;
			}
		}
	}
}
