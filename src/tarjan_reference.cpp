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

	void strong_connect(GraphNode *v, int &ind, stack<GraphNode*> &S, set<GraphNode*> &Sset,
						int &comp_counter, vector<set<GraphNode*> > &components)
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
				strong_connect(adj.find(v)->second[k], ind, S, Sset, comp_counter, components);
				v->lowlink = std::min(v->lowlink, adj.find(v)->second[k]->lowlink);
			}
			else if (adj.find(v)->second[k]->onstack == true)
			{
				v->lowlink = std::min(v->lowlink, adj.find(v)->second[k]->index);
			}
		}

		if (v->lowlink == v->index)
		{
			Sset = set<GraphNode*>();
			comp_counter++;
			cout << "SCC number " << comp_counter << ": ";
			GraphNode *w;
			do
			{
				w = S.top();
				S.pop();
				Sset.insert(w);
				w->onstack = false;
				cout << w->n << " ";
			}
			while (w != v);
			components.push_back(Sset);

			cout << endl;
		}
	}


	vector<set<GraphNode*> > tarjan()
	{
		int ind = 0;
		stack<GraphNode*> S;
		set<GraphNode*> Sset;
		int comp_counter = 0;
		vector<set<GraphNode*> > components;
		//loop on vertices
		map<GraphNode*,vector<GraphNode*> >::iterator loop_iter;
		for (loop_iter = adj.begin(); loop_iter != adj.end(); loop_iter++)
		{
			if (loop_iter->first->index < 0)
			{
				strong_connect(const_cast<GraphNode*>(loop_iter->first), ind, S, Sset, comp_counter, components);
			}
		}

		/*set<GraphNode*>::iterator set_it;
		for (int i = 0; i < components.size(); i++)
		{

			for (set_it = components[i].begin(); set_it != components[i].end(); set_it++)
			{
				cout << (*set_it)->n << " ";
			}
			cout << endl;
		}*/

		return components;
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
	adj[&g1] = {&g2};
	adj[&g2] = {&g3};
	adj[&g3] = {&g1};
	adj[&g4] = {&g2, &g3, &g5};
	adj[&g5] = {&g4, &g6};
	adj[&g6] = {&g3, &g7};
	adj[&g7] = {&g6};
	adj[&g8] = {&g7, &g8, &g5};

	Graph graph(adj);

	map<GraphNode*,vector<GraphNode*> >::iterator loop_iter;
	for (loop_iter = graph.adj.begin(); loop_iter != graph.adj.end(); loop_iter++)
	{
		cout << loop_iter->first->n << ": ";
		for (int i = 0; i < loop_iter->second.size(); i++)
		{
			cout << loop_iter->second[i]->n << " ";
		}
		cout << endl;
	}

	cout << endl << endl;
	vector<set<GraphNode*> > strongly_connected_components = graph.tarjan();
}
