#include "../include/aldebaran_parser.h"

AldebaranParser::AldebaranParser(){}

AldebaranParser::AldebaranParser(const char *input_path){
	input.open(input_path);
	if (input.fail()){
		throw(AldebaranNotFoundException());
	}
}

void AldebaranParser::parse_input(){
	string line;
	//read 'aut_header' line (the first line in an .aut file)
	getline(input, line);

	//get number of states from 'aut_header', after second comma
	int number_of_states;
	int count_commas = 0;
	int i = 0;
	string str_number_of_states;
	while (count_commas != 2){
		if (line[i] == ','){
			count_commas++;
		}
		i++;
	}

	while (line[i] != ')'){
		str_number_of_states.push_back(line[i]);
		i++;
	}
	number_of_states = atoi(str_number_of_states.c_str());

	//add keys to lts_edges
	for (int key = 0; key < number_of_states; key++){
		lts_edges[key];
	}

	//triple (from, label, to)
	int from;
	string label;
	int to;
	while (getline(input, line)){
		//strip whitespaces
		std::string::iterator end_pos = std::remove(line.begin(), line.end(), ' ');
		line.erase(end_pos, line.end());

		int char_counter; //counts characters in current line
		string str_from;
		//position 0: "("
		for (char_counter = 1; line[char_counter] != ','; char_counter++){
			str_from.push_back(line[char_counter]);
		}
		from = atoi(str_from.c_str());

		label = string();
		for (char_counter = char_counter + 2; line[char_counter] != '"'; char_counter++){
			label.push_back(line[char_counter]);
		}

		string str_to;
		for (char_counter = char_counter + 2; line[char_counter] != ')'; char_counter++){
			str_to.push_back(line[char_counter]);
		}
		to = atoi(str_to.c_str());

		map<int,vector<int> >::iterator itr = lts_edges.find(from);
		if (itr != lts_edges.end()){
		    itr->second.push_back(to);   //update predecessors list with given (b,i)
		}
	}
}

map<int, vector<int> > AldebaranParser::get_lts_edges() const{
	return lts_edges;
}
