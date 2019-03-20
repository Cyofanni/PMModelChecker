#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <set>

using namespace std;


class GraphNode
{
public:
	int n;
	int index;
	int lowlink;
	bool onstack;

	GraphNode(int n = 0)
	{
		 this->n = n;
		 index = -1;
		 lowlink = 0;
		 onstack = false;
	}

	GraphNode(const GraphNode &g)
	{
		this->n = g.n;
		this->index = g.index;
		this->lowlink = g.lowlink;
		this->onstack = g.onstack;
	}
	GraphNode& operator=(const GraphNode &g)
	{
		this->n = g.n;
		this->index = g.index;
		this->lowlink = g.lowlink;
		this->onstack = g.onstack;
	}
	bool operator==(const GraphNode &g)
	{
		return (this->n == g.n);
	}
	bool operator!=(const GraphNode &g)
	{
		return (this->n != g.n);
	}
};


bool operator<(const GraphNode &g1, const GraphNode &g2)
{
   return (g1.n < g2.n);
}


class Graph
{
public:
	map<GraphNode*,vector<GraphNode*> > adj;

	Graph(map<GraphNode*,vector<GraphNode*> > adj)
	{
		this->adj = adj;
	}

	void strong_connect(GraphNode *v, int &ind, stack<GraphNode*> &S)
	{
		v->index = ind;
		v->lowlink = ind;
		ind += 1;
		S.push(v);
		v->onstack = true;

		vector<GraphNode*> &v_predecessors = adj.find(v)->second;

		for (int k = 0; k < adj.find(v)->second.size(); k++)
		{
			if (adj.find(v)->second[k]->index < 0)
			{
				strong_connect(adj.find(v)->second[k], ind, S);
				v->lowlink = std::min(v->lowlink, adj.find(v)->second[k]->lowlink);
			}
			else if (adj.find(v)->second[k]->onstack == true)
			{
				v->lowlink = std::min(v->lowlink, adj.find(v)->second[k]->index);
			}
		}

		if (v->lowlink == v->index)
		{
			cout << "Current SCC: ";
			GraphNode *w;
			do{
				w = S.top();
				S.pop();
				w->onstack = false;
				cout << w->n << " ";
			}
			while (w != v);
			cout << endl;
		}
	}


	void tarjan(){
		int ind = 0;
		stack<GraphNode*> S;

		//loop on vertices
		map<GraphNode*,vector<GraphNode*> >::iterator loop_iter;
		for (loop_iter = adj.begin(); loop_iter != adj.end(); loop_iter++){
			if (loop_iter->first->index < 0)
			{
				strong_connect(const_cast<GraphNode*>(loop_iter->first), ind, S);
				cout << endl;
			}
		}
	}
};


int main(){
	GraphNode g1(1);
	GraphNode g2(2);
	GraphNode g3(3);
	GraphNode g4(4);
	GraphNode g5(5);
	GraphNode g6(6);
	GraphNode g7(7);
	GraphNode g8(8);

	map<GraphNode*,vector<GraphNode*> > adj;
	adj[&g1] = {&g3, &g2};
	adj[&g2] = {&g1};
	adj[&g3] = {&g1};
	adj[&g4] = {&g3, &g2, &g5};
	adj[&g5] = {&g4, &g6};
	adj[&g6] = {&g3, &g7};
	adj[&g7] = {&g6};
	adj[&g8] = {&g5, &g7, &g8};

	Graph graph(adj);

	map<GraphNode*,vector<GraphNode*> >::iterator loop_iter;
	for (loop_iter = graph.adj.begin(); loop_iter != graph.adj.end(); loop_iter++){
		cout << loop_iter->first->n << ": ";
		for (int i = 0; i < loop_iter->second.size(); i++){
			cout << loop_iter->second[i]->n << " ";
		}
		cout << endl;
	}

	cout << endl << endl;
	graph.tarjan();
}
