/* PMModelChecker is a model checking tool for solving fixpoint equational
 * systems by means of Progress Measures.
 */

#include "../include/move_composer.h"
#include "../include/solver.h"
#include "../include/input_checker.h"
#include "../include/mu_calculus_move_generator.h"
#include <stdio.h>
#include <string.h>



int
main(int argc, char *argv[])
{
	/*"General Mode":
		./pm_model_checker -ge system_file basis_size lattice_height basis_file
		in this mode, all the moves are provided by the user, and the
		lts file is not needed.
	 */

	/*"Mu-calculus Mode":
		./pm_model_checker -mu system_file basis_size lts_file lattice_height basis_file
		(observe the -mu flag)
		in this mode, the tool appends automatically the basic symbolic E-moves
		for 'diamond' and 'box' operators to the file "system", which is
		parsed later.
	 */

	 /*"Normalizer Mode":
	  * ./pm_model_checker -normalize system_file output_file
	  */

	if (argc < 2)
	{
		fprintf(stderr, "usage: ./pm_model_checker [flag] [args]\n");
		return 1;
	}
	else
	{
		if (argc < 4)
		{
			fprintf(stderr, "accepted command line arguments:\n");
			fprintf(stderr, "./pm_model_checker -ge [system_file] [basis_size] [lattice_height] [basis_file]\n\n");
			fprintf(stderr, "./pm_model_checker -mu [system_file] [basis_size] [lts_file] [lattice_height] [basis_file]\n\n");
			fprintf(stderr, "./pm_model_checker -normalize [system_file] [output_file]\n");
			return 1;
		}
		else
		{
			if (strcmp(argv[1], "-ge") != 0 and strcmp(argv[1], "-mu") != 0 and strcmp(argv[1], "-normalize") != 0)
			{
				fprintf(stderr, "error: unrecognized flag, use -ge, -mu or -normalize\n");
				return 1;
			}
			//read [basis_file] and store data into a vector
			ifstream basis_input;
			vector<string> basis_names;

			if (strcmp(argv[1], "-normalize") != 0)
			{
				if (argc >= 6)
				{
					if (strcmp(argv[1], "-ge") == 0)
					{
						basis_input.open(argv[5]);
					}
					else if (strcmp(argv[1], "-mu") == 0)
					{
						basis_input.open(argv[6]);
					}
					if (basis_input.fail())
					{
						fprintf(stderr, "error: basis file not found\n");
						return 1;
					}
					else
					{
						string line;
						while (getline(basis_input, line))
						{
							std::string::iterator end_pos = std::remove(line.begin(), line.end(), ' ');
							line.erase(end_pos, line.end());
							std::string::iterator end_pos_1 = std::remove(line.begin(), line.end(), '\t');
							line.erase(end_pos_1, line.end());

							basis_names.push_back(line);
						}
					}
				}
				else
				{
					fprintf(stderr, "error: basis file not found\n");
					return 1;
				}
			}

			bool general_mode = false;
			vector<Equation*> *eqs_moves;
			if (strcmp(argv[1], "-ge") == 0)
			{
				general_mode = true;
				yyin = fopen(argv[2], "r");
				if (yyin)
				{
					//run the parser and check if parsing succeeded
					if (yyparse() != 0)
					{
						return 1;
					}
					fclose(yyin);

					eqs_moves = root->get_equations();  //get system + moves
				}
				else
				{
					fprintf(stderr, "error: file with equational system not found\n");
					return 1;
				}
    		}

    		else if (strcmp(argv[1], "-mu") == 0)
    		{
				try
				{
					MuCalculusMoveGenerator muc_gen(argv[4], atoi(argv[3]));
					vector<string> box_moves = muc_gen.generate_box_diamond_move(true);
					vector<string> diamond_moves = muc_gen.generate_box_diamond_move(false);
					yyin = fopen(argv[2], "a");
					//add computed moves to input system
					if (yyin)
					{
						for (int i = 0; i < box_moves.size(); i++)
						{
							const char *curr_box_move = box_moves[i].c_str();
							const char *curr_diamond_move = diamond_moves[i].c_str();
							fprintf(yyin, "%s\n", curr_box_move);
							fprintf(yyin, "%s\n", curr_diamond_move);
						}
						fclose(yyin);
						yyin = fopen(argv[2], "r");
						//then parse it as usual
						//run the parser and check if parsing succeeded
						if (yyparse() != 0)
						{
							return 1;
						}

						eqs_moves = root->get_equations();  //get system + moves
					}
					else
					{
						fprintf(stderr, "error: file with equational system not found\n");
						return 1;
					}
				}
				catch(AldebaranNotFoundException a)
				{
					fprintf(stderr, "error: lts file not found\n");
					return 1;
				}
			}

			if (strcmp(argv[1], "-normalize") != 0)
			{
				//input sanitization
				InputChecker ic(eqs_moves, atoi(argv[3]));
				vector<string> missing_moves = ic.get_missing_moves();
				int out_of_bound = ic.find_out_of_bound_index();

				if (missing_moves.size() == 0 and out_of_bound == -1)
				{
					map<int,string> min_max_eq; //type o each equation ("=min" or "=max")

					int eq_counter = 0; //count actual equations (like x1 =min etc...) and discard moves
					for (int i = 0; i < eqs_moves->size(); i++)
					{
						if ((*eqs_moves)[i]->get_id()[0] == 'x')
						{
							string min_or_max = (*eqs_moves)[i]->get_eq_type();
							min_max_eq[eq_counter];
							min_max_eq[eq_counter] = min_or_max;
							eq_counter++;
						}
					}

					MoveComposer mv_comp(eqs_moves, atoi(argv[3]));
					map<Position,ExpNode*> composite_moves = mv_comp.system_composer();
					map<Position,ExpNode*>::iterator loop_iter;
					/*for (loop_iter = composite_moves.begin(); loop_iter != composite_moves.end(); loop_iter++)
					{
						cout << "Position:" << "(" << (loop_iter->first).get_b_element() << "," << (loop_iter->first).get_eq_index() << "):";
						cout << "  Move: ";
						ExpNode::print_equation_infix(loop_iter->second, stdout);
						cout << endl;
					}
					cout << endl;*/

					//create a Solver object and solve the system
					int lattice_height;
					if (general_mode)
					{
						lattice_height = atoi(argv[4]);
					}
					else
					{
						lattice_height = atoi(argv[5]);
					}

					Solver slv(min_max_eq, atoi(argv[3]), min_max_eq.size(), composite_moves, lattice_height);

					vector< vector < vector<int> > > final_matrix_worklist = slv.solve_system_worklist();
					cout << "WORKLIST'S OUTCOME:" << endl;
					Solver::print_pm_matrix(final_matrix_worklist, atoi(argv[3]), eq_counter);
					Solver::pretty_print_pm_matrix(final_matrix_worklist, atoi(argv[3]), eq_counter, lattice_height, basis_names);

					cout << endl << endl;
					vector< vector < vector<int> > > final_matrix_chaotic = slv.solve_system_chaotic_iteration();
					cout << "CHAOTIC ITERATION'S OUTCOME:" << endl;
					Solver::print_pm_matrix(final_matrix_chaotic, atoi(argv[3]), eq_counter);
					Solver::pretty_print_pm_matrix(final_matrix_worklist, atoi(argv[3]), eq_counter, lattice_height, basis_names);

					for (loop_iter = composite_moves.begin(); loop_iter != composite_moves.end(); loop_iter++)
					{
						delete loop_iter->second;
					}

					for (int i = 0; i < eqs_moves->size(); i++)
					{
						delete (*eqs_moves)[i];
						delete (*eqs_moves)[i]->get_exp_node();
					};
					delete eqs_moves;
				}
				else
				{
					if (missing_moves.size() != 0)
					{
						for (int i = 0; i < missing_moves.size(); i++)
						{
							fprintf(stderr, "error: missing move: ");
							cerr << missing_moves[i] << endl;
						}
					}
					if (out_of_bound > -1)
					{
						fprintf(stderr, "error: out of bound index in equation %d\n", out_of_bound + 1);
					}


					return 1;
				}
			}

			if (strcmp(argv[1], "-normalize") == 0)
			{
				yyin = fopen(argv[2], "r");
				if (yyin)
				{
					//run the parser and check if parsing succeeded
					if (yyparse() != 0)
					{
						return 1;
					}
					fclose(yyin);

					FILE *normalization_output = fopen(argv[3], "w");

					vector<Equation*> *eqs = root->get_equations();  //get system + moves
					vector<Equation*> normalized_equations;

					vector<Equation*> normalized_system = EqSystem::mucalculus_normalizer_system(*eqs);

					for (int i = 0; i < normalized_system.size(); i++)
					{
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

					for (int i = 0; i < eqs->size(); i++)
					{
						delete (*eqs)[i];
						delete (*eqs)[i]->get_exp_node();
					}
					delete eqs;

					for (int i = 0; i < normalized_system.size(); i++)
					{
						delete normalized_system[i];
						delete normalized_system[i]->get_exp_node();
					}

					fclose(normalization_output);
				}
				else
				{
					fprintf(stderr, "error: file with equational system not found\n");
					return 1;
				}
			}
		}
	}
}
