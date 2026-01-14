// #include <iostream>
#include <bits/stdc++.h> // trocar depois
using namespace std;

#include "Data.h"
#include "hungarian.h"

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

const int INF = 0x3f3f3f3f;

int n;

// struct Solution {
//     vector<int> sequence;
//     double cost = 0.0;
// };

// struct InsertionInfo{
//     int InsertNode;
//     int RemovedEdge;
//     double cost;
// };

// vector<InsertionInfo> InsertionCost(Solution &s, set<int> &CL){
//     vector<InsertionInfo> cost_Insertion((s.sequence.size() - 1) * CL.size());

//     int l = 0;
//     for (int a = 0; a < s.sequence.size() - 1; a++){
//         int i = s.sequence[a];
//         int j = s.sequence[a+1];
//         for (auto k : CL){
//             cost_Insertion[l].cost = g[i][k] + g[j][k] - g[i][j];
//             cost_Insertion[l].InsertNode = k;
//             cost_Insertion[l].RemovedEdge = a;
//             l++;
//         }
//     }

//     return cost_Insertion;
// }


// void chooseRandomNodes(Solution &s, vector<bool> &elements){
//     uniform_int_distribution<int> dist(2, n);

//     int found = 0;
//     while (found < 3){
//         int value = dist(rng);

//         if (!elements[value]){
//             found++;
//             elements[value] = true;
//             s.sequence.push_back(value);
//         }
//     }
// }


// void RemainingNodes(set<int> &CL, vector<bool> &elements){
//     for (int i = 1; i <= n; i++){
//         if (!elements[i]) CL.insert(i);
//     }
// }


// void InsertingSolution(Solution &s, vector<InsertionInfo> &node, int selected){
//     s.cost += node[selected].cost;
    
//     s.sequence.insert(s.sequence.begin() + node[selected].RemovedEdge + 1, node[selected].InsertNode);
// }


// double Construction(){
//     Solution s;
    
//     s.cost = 0;

//     vector<bool> elements(n+1);
//     elements[1] = true;
    
//     s.sequence.push_back(1);
//     chooseRandomNodes(s, elements);
//     s.sequence.push_back(1);

//     for (int i = 0; i < s.sequence.size() - 1; i++){
//         s.cost += g[s.sequence[i]][s.sequence[i+1]];
//     }
    
//     set<int> CL;
//     RemainingNodes(CL, elements);
   
//     while (!CL.empty()){
//         vector<InsertionInfo> cost_Insertion = InsertionCost(s, CL);

//         sort(cost_Insertion.begin(), cost_Insertion.end(), [&](InsertionInfo x, InsertionInfo y){
//             return x.cost < y.cost;
//         });

//         double alpha = (double) rand() / RAND_MAX;

//         int selected = rand() % ((int) ceil (alpha * (cost_Insertion.size() - 1)));

//         InsertingSolution(s, cost_Insertion, selected);

//         CL.erase(cost_Insertion[selected].InsertNode);
//     }

//     return s;
// }



struct Node {
	vector<pair<int, int>> forbidden_arcs;
	vector<vector<int>> subtours;
	double lower_bound;
	int chosen;
	bool feasible;

	bool operator<(Node &p) const {
		return lower_bound < p.lower_bound;
	};

	// pra priority queue?
};

vector<vector<int>> Subtours(hungarian_problem_t &p){
	vector<vector<int>> subtours;
	
	vector<bool> vis(n+1);
	for (int i = 1; i <= n; i++){
		if (vis[i]) continue;

		int node = i;
		int first = i;

		vector<int> tour; 

		do {
			tour.push_back(node);
			vis[node] = true;
	
			for (int j = 1; j <= n; j++){
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

	node.subtours = Subtours(p); // detectar o conjunto de subtours
	node.chosen = getChosen(); // pegar o indice do menor subtour
	node.feasible = isFeasible(node); // verificar viabilidade

	hungarian_free(&p);

	// precisamos reverter os custos
	for (auto [u, v, c] : prev_Values){
		cost[u][v] = c;
	}
}


double Solve(string strategy, double **cost){
	Node root;
	updateNode(root, cost);

	double upper_bound = INF; // construção?

	if (strategy == "DFS" || strategy == "BFS") upper_bound = Solve_List(root, cost, strategy, upper_bound);
	else upper_bound = Solve_Pq(root, cost, strategy, upper_bound);

	return upper_bound;
}

double Solve_List(Node &root, double **cost, string strategy, double upper_bound){
	list<Node> tree;
	tree.push_back(root);

	while(!tree.empty()){
		Node node;

		if (strategy == "DFS"){
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
		
		// adiciona os filhos
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

double Solve_Pq(Node &root, double **cost, string strategy, double upper_bound){
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
		
		// adiciona os filhos
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

	hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	hungarian_init(&p, cost, n, n, mode); // Carregando o problema

	double obj_value = hungarian_solve(&p);
	cout << "Obj. value: " << obj_value << endl;

	cout << "Assignment" << endl;
	hungarian_print_assignment(&p);

	hungarian_free(&p);
	for (int i = 0; i < n; i++) delete [] cost[i];
	delete [] cost;

	return 0;
}