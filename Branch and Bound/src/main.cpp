#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <chrono>
#include <queue>

// #include <bits/stdc++.h> // trocar depois
using namespace std;

#include "Data.h"
#include "hungarian.h"

const int INF = 0x3f3f3f3f;

int n;

struct Node {
	vector<pair<int, int>> forbidden_arcs;
	vector<vector<int>> subtours;
	double lower_bound;
	int chosen;
	bool feasible;

	bool operator<(const Node &p) const {
		return lower_bound < p.lower_bound;
	};

	bool operator>(const Node &p) const {
		return lower_bound > p.lower_bound;
	};
};

vector<vector<int>> Subtours(hungarian_problem_t &p){
	vector<vector<int>> subtours;
	
	vector<bool> vis(n);
	for (int i = 0; i < n; i++){
		if (vis[i]) continue;

		int node = i;
		int first = i;

		vector<int> tour; 

		do {
			tour.push_back(node);
			vis[node] = true;
	
			for (int j = 0; j < n; j++){
				if (p.assignment[node][j]){
					node = j;
					break;
				}
			}
		} while (first != node);

		tour.push_back(node);

		subtours.push_back(tour);
	}

	sort(subtours.begin(), subtours.end(), [&](vector<int> a, vector<int> b){
		if ((int) a.size() != (int) b.size()) return (int) a.size() < (int) b.size();
		return a[0] < b[0];
	});

	return subtours;
}

int getChosen() {
	// lowest index of subtours
	return 0;
}

bool isFeasible(Node& node) { 
	return (node.subtours.size() == 1);
}

void updateNode(Node& node, double **cost){
	vector<tuple<int, int, double>> prev_Values;
	for (auto [u, v] : node.forbidden_arcs){
		prev_Values.push_back({u, v, cost[u][v]});
		cost[u][v] = INF;
	}
	
	hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	hungarian_init(&p, cost, n, n, mode);

	node.lower_bound = hungarian_solve(&p);

	node.subtours = Subtours(p); 
	node.chosen = getChosen(); 
	node.feasible = isFeasible(node); 

	hungarian_free(&p);

	// reversing costs
	for (auto [u, v, c] : prev_Values){
		cost[u][v] = c;
	}
}

double Solve_List(Node &root, double **cost, string strategy, double upper_bound){
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
		
		// childrens
		for (int i = 0; i < node.subtours[node.chosen].size() - 1; i++){
			Node aux;
			aux.forbidden_arcs = node.forbidden_arcs;
			
			pair<int, int> forbidden_arcs = {node.subtours[node.chosen][i], node.subtours[node.chosen][i+1]};

			aux.forbidden_arcs.push_back(forbidden_arcs);
			updateNode(aux, cost);
			if (aux.lower_bound <= upper_bound){
				tree.push_back(aux);
			}
		}
	}

	return upper_bound;
}

double Solve_Pq(Node &root, double **cost, string &strategy, double upper_bound){
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
		
		// childrens
		for (int i = 0; i < node.subtours[node.chosen].size() - 1; i++){
			Node aux;
			aux.forbidden_arcs = node.forbidden_arcs;
			
			pair<int, int> forbidden_arcs = {node.subtours[node.chosen][i], node.subtours[node.chosen][i+1]};

			aux.forbidden_arcs.push_back(forbidden_arcs);
			updateNode(aux, cost);
			if (aux.lower_bound <= upper_bound){
				tree.push(aux);
			}
		}
	}

	return upper_bound;
}


double Solve(string &strategy, double **cost){
	Node root;
	updateNode(root, cost);

	double upper_bound = INF; // Construction?

	if (strategy == "DFS" || strategy == "BFS") upper_bound = Solve_List(root, cost, strategy, upper_bound);
	else upper_bound = Solve_Pq(root, cost, strategy, upper_bound);

	return upper_bound;
}

int main(int argc, char** argv) {

	Data data = Data(argc, argv[1]);
	data.read();

	n = (int) data.getDimension();

	double **cost = new double*[n];
	for (int i = 0; i < n; i++){
		cost[i] = new double[n];
		for (int j = 0; j < n; j++){
			cost[i][j] = data.getDistance(i+1,j+1);
		}
	}

	string strategy = argv[2];

	int runs = 10;
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

	for (int i = 0; i < n; i++) delete [] cost[i];
	delete [] cost;

	return 0;
}