#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <chrono>
#include <queue>

using namespace std;

#include "Data.h"
#include "SubGradient.h"

const int INF = 0x3f3f3f3f;

int n;

struct Node {
	vector<pair<int, int>> forbidden_arcs;
	vector<pair<int, int>> arcs;
	vector<double> lambda;
	double lower_bound;
	bool feasible;

	bool operator<(const Node &p) const {
		return lower_bound < p.lower_bound;
	};

	bool operator>(const Node &p) const {
		return lower_bound > p.lower_bound;
	};
};

vector<pair<int, int>> Arcs(std::vector<Edge> &one_tree){
	std::vector<int> degree(n);
	for (int i = 0; i < one_tree.size(); i++){
		auto [u, v, w] = one_tree[i];
		degree[u]++;
		degree[v]++;
	}

	int deg = 0;
	int node = 0;
	for (int i = 0; i < n; i++){
		if (degree[i] > deg){
			deg = degree[i];
			node = i;
		}
	}

	vector<pair<int, int>> arcs;
	for (int i = 0; i < one_tree.size(); i++){
		auto [u, v, w] = one_tree[i];

		if (u == node || v == node){
			arcs.push_back({u, v});
		} 
	}

	return arcs;
}

void updateNode(Node &node, vector<vector<double>> &cost, double UB){
	vector<tuple<int, int, double>> prev_Values;
	
	for (auto [u, v] : node.forbidden_arcs){
		prev_Values.push_back({u, v, cost[u][v]});
		cost[u][v] = INF;
	}

	SubGradient grad(n);
	grad.solve(cost, UB, node.lambda);

	node.feasible = grad.feasible();
	node.lower_bound = grad.cost();
	node.arcs = Arcs(grad.s); 
	node.lambda = grad.lambda;

	// reversing costs
	for (auto [u, v, c] : prev_Values){
		cost[u][v] = c;
	}
}

double Solve_List(Node &root, vector<vector<double>> &cost, string &strategy, double upper_bound){
	list<Node> tree;
	tree.push_back(root);

	while(!tree.empty()){
		Node node;

		if (strategy == "DFS") {
			node = tree.back();
			tree.pop_back();
		}
		else {
			node = tree.front();
			tree.pop_front();
		}

		if (node.feasible){
			if (node.lower_bound < upper_bound){
				upper_bound = node.lower_bound;
			}
			continue;
		}
		
		// if (node.feasible || node.lower_bound > upper_bound) continue;
		
		// std::cout << "VALOR: " << upper_bound << " " <<  node.lower_bound << "\n";
		// std::cout << "PROIBIDOS: " << node.arcs.size() << "\n";

		// childrens
		for (int i = 0; i < node.arcs.size(); i++){
			Node aux;
			aux.forbidden_arcs = node.forbidden_arcs;
			
			pair<int, int> forbidden_arcs = node.arcs[i];

			aux.forbidden_arcs.push_back(forbidden_arcs);
			aux.lambda = node.lambda;

			updateNode(aux, cost, upper_bound);

			if (aux.lower_bound < upper_bound){ // < ou <=
				tree.push_back(aux);
			}
		}
	}

	return upper_bound;
}

double Solve_Pq(Node &root, vector<vector<double>> &cost, string &strategy, double upper_bound){
	priority_queue<Node, vector<Node>, greater<Node>> tree;
	tree.push(root);

	while(!tree.empty()){
		Node node = tree.top();
		tree.pop();

		if (node.feasible){
			if (node.lower_bound < upper_bound){
				upper_bound = node.lower_bound;
			}
			continue;
		}
		
		if (node.lower_bound > upper_bound) continue;

		// childrens
		for (int i = 0; i < node.arcs.size(); i++){
			Node aux;
			aux.forbidden_arcs = node.forbidden_arcs;
			
			pair<int, int> forbidden_arcs = node.arcs[i];

			aux.forbidden_arcs.push_back(forbidden_arcs);
			aux.lambda = node.lambda;
			updateNode(aux, cost, upper_bound);
			if (aux.lower_bound < upper_bound){
				tree.push(aux);
			}
		}
	}

	return upper_bound;
}


double Solve(string &strategy, vector<vector<double>> &cost){
	double rota = 0;
	for (int i = 1; i < n; i++){
		rota += cost[i-1][i];
	}
	rota += cost[n-1][0];

	double upper_bound = rota; 

	Node root;
	updateNode(root, cost, upper_bound);

	if (strategy == "DFS" || strategy == "BFS") upper_bound = Solve_List(root, cost, strategy, upper_bound);
	else upper_bound = Solve_Pq(root, cost, strategy, upper_bound);

	return upper_bound;
}

int main(int argc, char** argv) {

	Data data = Data(argc, argv[1]);
	data.read();

	n = (int) data.getDimension();

	vector<vector<double>> cost(n, vector<double> (n));
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			cost[i][j] = data.getDistance(i+1, j+1);
		}
	}

	string strategy = argv[2];

	int runs = 1;
    double totalTime = 0.0, totalCost = 0.0;

    for (int i = 0; i < runs; i++){
        auto start = chrono::high_resolution_clock::now();
    
        double SOL = Solve(strategy, cost);
    
        auto end = chrono::high_resolution_clock::now();
    
        chrono::duration<double> duration = end - start;
        totalTime += duration.count();
        totalCost += SOL;
    }

    cout << totalTime / runs << " " << totalCost / runs << "\n\n";


	return 0;
}