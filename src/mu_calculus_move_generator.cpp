#include "../include/mu_calculus_move_generator.h"

MuCalculusMoveGenerator::MuCalculusMoveGenerator(const char *lts_file, int basis_size)
{
	this->lts_file = new char[50];
	this->lts_file = lts_file;

	this->basis_size = basis_size;
}

vector<string>
MuCalculusMoveGenerator::generate_box_diamond_move(bool isbox)
{
	try
	{
		AldebaranParser parser(lts_file);

		parser.parse_input();
		map<int,vector<int> > lts = parser.get_lts_edges();

		//vector of moves
		vector<string> moves;

		string op; //either 'or' or 'and', to connect atoms
		string name;
		if (isbox)
		{
			op = "and";
			name = "box";
		}
		else
		{
			op = "or";
			name = "diamond";
		}

		map<int,vector<int> >::iterator loop_iter;
		for (loop_iter = lts.begin(); loop_iter != lts.end(); loop_iter++)
		{
			string current_move;  //move to be built as string
			string move_identifier = "phi";
			string equation;

			string str_basis_element;
			stringstream ss;
			ss << loop_iter->first;
			str_basis_element = ss.str();

			move_identifier = move_identifier + "_" + str_basis_element + "_" + name;

			equation = move_identifier + " " + "=" + " ";

			//consider only states with successors
			if (loop_iter->second.size() > 0){
				for (unsigned int i = 0; i < loop_iter->second.size(); i++)
				{
					string str_successor;
					stringstream sss;
					sss << loop_iter->second[i];
					str_successor = sss.str();

					/*build the whole move use 0 as placeholder for index:
					eg) phi_0_box = [b_1,999] and */
					//if it's not the last successor: add "and/or"
					if (i < loop_iter->second.size() - 1)
					{
						equation = equation + "[b_" + str_successor + "," + "0" + "] " + op + " ";
					}
					//last successor: without "and/or"
					else
					{
						equation = equation + "[b_" + str_successor + "," + "0" + "]";
					}
				}
			moves.push_back(equation);
			}
		}

		return moves;
	}

	catch(AldebaranNotFoundException a)
	{
		throw(AldebaranNotFoundException());
	}
}
