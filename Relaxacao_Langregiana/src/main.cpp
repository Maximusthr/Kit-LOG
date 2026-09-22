#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <chrono>
#include <queue>
#include <deque>

#include "Data.h"
#include "SubGradient.h"

const int INF = 0x3f3f3f3f;

int n;

struct Node {
	std::vector<std::pair<int, int>> forbidden_arcs;
	std::vector<std::pair<int, int>> arcs;
	std::vector<double> lambda;
	double lower_bound;
	bool feasible;

	bool operator<(const Node &p) const {
		return lower_bound < p.lower_bound;
	};

	bool operator>(const Node &p) const {
		return lower_bound > p.lower_bound;
	};
};

std::vector<std::pair<int, int>> Arcs(std::vector<Edge> &one_tree){
	std::vector<int> degree(n);

	for (auto [u, v, _] : one_tree){
		degree[u]++;
		degree[v]++;
	}

	int pos = 0;
	int mx = 0;
	for (int i = 0; i < n; i++){
		if (degree[i] > mx){
			mx = degree[i];
			pos = i;
		}
	}

	std::vector<std::pair<int, int>> arcs;
	for (auto [u, v, _] : one_tree){
		if (u == pos || v == pos){
			arcs.push_back({u, v});
			if (arcs.size() == mx) break;
		}
	}

	return arcs;
}

void updateNode(Node &node, std::vector<std::vector<double>> &cost, double UB){
	std::vector<std::tuple<int, int, double>> prev_Values;

	for (auto [u, v] : node.forbidden_arcs){
		prev_Values.push_back({u, v, cost[u][v]});
		cost[u][v] = INF;
	}

	SubGradient grad(n);
	grad.solve(cost, UB, node.lambda);

	node.lambda = grad.lambda;
	node.lower_bound = grad.cost();
	node.feasible = grad.feasible();
	if (!node.feasible) node.arcs = Arcs(grad.s);

	for (auto [u, v, custo] : prev_Values){
		cost[u][v] = custo;
	}
}

double Solve_List(Node &root, std::vector<std::vector<double>> &cost, string &strategy, double upper_bound){
	std::deque<Node> tree; 
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
		
		if (node.feasible && node.lower_bound < upper_bound){
			upper_bound = node.lower_bound;
		}

		if (node.lower_bound >= upper_bound || node.feasible) continue;

		for (auto [u, v] : node.arcs){
			Node aux = node;
			aux.arcs.clear();

			aux.forbidden_arcs.push_back({u, v});

			updateNode(aux, cost, upper_bound);

			if (aux.lower_bound <= upper_bound){
				tree.push_back(aux);
			}
		}
	}

	return upper_bound;
}

double Solve_Pq(Node &root, std::vector<std::vector<double>> &cost, string &strategy, double upper_bound){
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> tree;
	tree.push(root);

	while(!tree.empty()){
		Node node = tree.top();
		tree.pop();

		if (node.feasible && node.lower_bound < upper_bound){
			upper_bound = node.lower_bound;
		}

		if (node.lower_bound >= upper_bound || node.feasible) continue;

		for (auto [u, v] : node.arcs){
			Node aux = node;
			aux.arcs.clear();

			aux.forbidden_arcs.push_back({u, v});

			updateNode(aux, cost, upper_bound);

			if (aux.lower_bound <= upper_bound){
				tree.push(aux);
			}
		}
	}

	return upper_bound;
}


double Solve(string &strategy, std::vector<std::vector<double>> &cost){
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

	std::vector<std::vector<double>> cost(n, std::vector<double> (n));
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