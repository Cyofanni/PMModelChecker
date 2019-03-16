#ifndef ALDEBARAN_PARSER_H
#define ALDEBARAN_PARSER_H

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

class AldebaranNotFoundException{};

class AldebaranParser{
private:
	ifstream input;
	map<int,vector<int> > lts_edges;
public:
	AldebaranParser();
	AldebaranParser(const char *input_path);
	void parse_input(); //fills 'lts_edges'
	map<int,vector<int> > get_lts_edges() const;
};

#endif
