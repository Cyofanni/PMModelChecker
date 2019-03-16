#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include "position.h"

using namespace std;

class GraphNode{
private:
	Position pos;
	int index;
	int lowlink;
	bool defined;
	bool on_stack;
public:
	GraphNode(){}
	GraphNode(Position p){
		 pos = p;
		 index = -1;
		 lowlink = -1;
		 defined = false;
		 on_stack = false;
	}
	
	Position get_position() const{
		return pos;
	}
	int get_index() const{
		return index;
	}
	void set_index(int i){
		index = i;
	}
	void set_lowlink(int l){
		lowlink = l;
	}
	bool is_defined() const{
		return defined;
	}
	void set_defined(bool def){
		defined = def;
	}
	bool is_on_stack() const{
		return on_stack;
	}
	void set_on_stack(bool os){
		on_stack = os;
	}	
};

bool operator<(const GraphNode &g1, const GraphNode &g2){
   return (g1.get_position() < g2.get_position());
}

class Graph{
private:
	int size;
	vector< vector<GraphNode> > adjacency_lists;  //a list for each vertex, mapping vertices to lists
	
	Graph(int s){
		size = s;
		adjacency_lists = vector< vector<GraphNode> >(s);
	}

	void strong_connect(GraphNode &g, int &ind, stack<GraphNode> &st){
		g.set_index(ind);
		g.set_lowlink(ind);
		ind += 1;
		st.push(g);
		g.set_on_stack(true);
		
	}

public:
	vector< vector<GraphNode> > strong_conn_components(){		
		int ind = 0;
		stack<GraphNode> st;
		//loop on vertices
		for (int i = 0; i < adjacency_lists.size(); i++){
			if (adjacency_lists[i][0].is_defined() == false){
				strong_connect(adjacency_lists[i][0], ind, st);				
			}
		}
	}
};


int main(){

}
