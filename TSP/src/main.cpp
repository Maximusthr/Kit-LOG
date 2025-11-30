#include "Data.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <set>
#include <vector>
#include <cassert>
#include <unistd.h>

using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

const int INF = (int)1e9;

struct Solution {
    vector<int> sequence;
    double cost = 0.0;
};

struct InsertionInfo{
    int InsertNode;
    int RemovedEdge;
    double cost;
};

int n;
vector<vector<double>> g;

vector<InsertionInfo> InsertionCost(Solution &s, set<int> &CL){
    vector<InsertionInfo> cost_Insertion((s.sequence.size() - 1) * CL.size());

    int l = 0;
    for (int a = 0; a < s.sequence.size() - 1; a++){
        int i = s.sequence[a];
        int j = s.sequence[a+1];
        for (auto k : CL){
            cost_Insertion[l].cost = g[i][k] + g[j][k] - g[i][j];
            cost_Insertion[l].InsertNode = k;
            cost_Insertion[l].RemovedEdge = a;
            l++;
        }
    }

    return cost_Insertion;
}

void chooseRandomNodes(Solution &s, vector<bool> &elements){
    uniform_int_distribution<int> dist(2, n);

    int found = 0;
    while (found < 3){
        int value = dist(rng);

        if (!elements[value]){
            found++;
            elements[value] = true;
            s.sequence.push_back(value);
        }
    }
}

void RemainingNodes(set<int> &CL, vector<bool> &elements){
    for (int i = 1; i <= n; i++){
        if (!elements[i]) CL.insert(i);
    }
}

void inserirNaSolucao(Solution &s, vector<InsertionInfo> &node, int selected){
    s.cost += node[selected].cost;
    
    s.sequence.insert(s.sequence.begin() + node[selected].RemovedEdge + 1, node[selected].InsertNode);
}

Solution Construction(){
    Solution s;
    
    s.cost = 0;

    vector<bool> elements(n+1);
    elements[1] = true;
    
    s.sequence.push_back(1);
    chooseRandomNodes(s, elements);
    s.sequence.push_back(1);

    for (int i = 0; i < s.sequence.size() - 1; i++){
        s.cost += g[s.sequence[i]][s.sequence[i+1]];
    }
    
    set<int> CL;
    RemainingNodes(CL, elements);
   
    while (!CL.empty()){
        vector<InsertionInfo> cost_Insertion = InsertionCost(s, CL);

        sort(cost_Insertion.begin(), cost_Insertion.end(), [&](InsertionInfo x, InsertionInfo y){
            return x.cost < y.cost;
        });

        double alpha = (double) rand() / RAND_MAX;

        int selected = rand() % ((int) ceil (alpha * (cost_Insertion.size() - 1)));

        inserirNaSolucao(s, cost_Insertion, selected);

        CL.erase(cost_Insertion[selected].InsertNode);
    }

    return s;
}

bool bestImprovementSwap(Solution &s){
    double bestDelta = 0;
    int best_i, best_j;

    for (int i = 1; i < s.sequence.size() - 1; i++){
        int vi = s.sequence[i];
        int vi_next = s.sequence[i+1];
        int vi_prev = s.sequence[i-1];

        for (int j = i + 1; j < s.sequence.size() - 1; j++){
            int vj = s.sequence[j];
            int vj_next = s.sequence[j+1];
            int vj_prev = s.sequence[j-1];
            double delta = 0;

            // corner case
            if (i + 1 == j){
                delta = - g[vi_prev][vi] - g[vi][vj] - g[vj][vj_next] 
                        + g[vi_prev][vj] + g[vj][vi] + g[vi][vj_next];
            }
            else {
                delta = - g[vi_prev][vi] - g[vi][vi_next] - g[vj_prev][vj] - g[vj][vj_next]
                        + g[vi_prev][vj] + g[vi][vj_next] + g[vj_prev][vi] + g[vj][vi_next];
            }

            if (delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    if (bestDelta < 0){
        swap(s.sequence[best_i], s.sequence[best_j]);
        s.cost = s.cost + bestDelta;
        return true;
    }

    return false;
}

bool bestImprovement20pt(Solution &s){
    double bestDelta = 0;
    int best_i, best_j;

    for (int i = 0; i < s.sequence.size() - 3; i++){
        int cur = s.sequence[i];
        int next = s.sequence[i+1];
        
        for (int j = i + 3; j < s.sequence.size(); j++){
            int prev_last = s.sequence[j-1];
            int last = s.sequence[j];

            double delta = - g[cur][next] - g[prev_last][last] + g[cur][prev_last] + g[next][last];

            if (delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    if (bestDelta < 0){
        reverse(s.sequence.begin() + best_i + 1, s.sequence.begin() + best_j);
        s.cost = s.cost + bestDelta;
        return true;
    }

    return false;
}

bool bestImprovementOrOpt (Solution &s, int block){

    double bestDelta = 0;
    int best_i, best_j;

    for (int i = 1; i < s.sequence.size() - block; i++){
        int cur_i = s.sequence[i];
        int block_i = s.sequence[i + block - 1];
        int next_i = s.sequence[i + block];
        int prev_i = s.sequence[i - 1];

        for (int j = 0; j < s.sequence.size() - 1; j++){
            if (j + 1 == i) {
                j += block;
                continue;
            }
            
            int cur_j = s.sequence[j];
            int next_j = s.sequence[j + 1];

            double delta = - g[cur_j][next_j] - g[prev_i][cur_i] - g[block_i][next_i]
                           + g[cur_j][cur_i] + g[block_i][next_j] + g[prev_i][next_i];

            if (delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    if (bestDelta < 0){

        if (best_i > best_j){
            rotate(s.sequence.begin() + best_j + 1, s.sequence.begin() + best_i, s.sequence.begin() + best_i + block);
        }
        else if (best_i < best_j){
            rotate(s.sequence.begin() + best_i, s.sequence.begin() + best_i + block, s.sequence.begin() + best_j + 1);
        }

        
        s.cost = s.cost + bestDelta;
        return true;
    }

    return false;
}

void LocalSearch (Solution &s){
    vector<int> NL = {1, 2, 3, 4, 5};
    bool improved = false;

    while (!NL.empty()){
        uniform_int_distribution<int> random(0, NL.size()-1);

        int choice = random(rng);

        switch(NL[choice]){
            case 1:
                improved = bestImprovementSwap(s);
                break;
            case 2:
                improved = bestImprovement20pt(s);
                break;
            case 3:
                improved = bestImprovementOrOpt(s, 1);
                break;
            case 4:
                improved = bestImprovementOrOpt(s, 2);
                break;
            case 5:
                improved = bestImprovementOrOpt(s, 3);
                break;
        }

        if (improved){
            NL = {1, 2, 3, 4, 5};
        }
        else {
            NL.erase(NL.begin() + choice);
        }
    }
}

Solution Pertubation(Solution SOL){
    
    int sizes = SOL.sequence.size();
    
    int beg = 2;
    int end = (sizes + 9)/10;
    
    auto Intersect = [&](int x1, int y1, int x2, int y2) -> bool {
        // true = intersect
        return ((y2 >= x1 && y2 <= y1) || (y1 >= x2 && y1 <= y2));
    };
    
    pair<int, int> seg_1, seg_2;
    
    do {
        uniform_int_distribution<int> random_1(beg, end);
        uniform_int_distribution<int> random_2(beg, end);
        
        int v1 = random_1(rng);
        int v2 = random_2(rng);

        uniform_int_distribution<int> SEG_ELEM_1(1, sizes - v1 - 2);
        uniform_int_distribution<int> SEG_ELEM_2(1, sizes - v2 - 2);

        seg_1.first = SEG_ELEM_1(rng);
        seg_1.second = seg_1.first + v1 - 1;

        
        seg_2.first = SEG_ELEM_2(rng);
        seg_2.second = seg_2.first + v2 - 1;

    } while (Intersect(seg_1.first, seg_1.second, seg_2.first, seg_2.second));

    Solution aux = SOL;
    
    if (seg_1.first > seg_2.first) swap(seg_1, seg_2);

    int prev_i = aux.sequence[seg_1.first - 1];
    int cur_i_left = aux.sequence[seg_1.first];
    int cur_i_right = aux.sequence[seg_1.second];
    int next_i = aux.sequence[seg_1.second + 1];

    int prev_j = aux.sequence[seg_2.first - 1];
    int cur_j_left = aux.sequence[seg_2.first];
    int cur_j_right = aux.sequence[seg_2.second];
    int next_j = aux.sequence[seg_2.second + 1];

    double delta = 0.0;

    // corner case
    if (seg_1.second + 1 == seg_2.first){
        delta = - g[prev_i][cur_i_left] - g[cur_i_right][next_i] - g[cur_j_right][next_j]
                + g[prev_i][cur_j_left] + g[cur_i_right][next_j] + g[cur_j_right][cur_i_left];
    }
    else {
        delta = - g[prev_i][cur_i_left] - g[cur_i_right][next_i]
                - g[prev_j][cur_j_left] - g[cur_j_right][next_j]
                + g[prev_i][cur_j_left] + g[cur_i_right][next_j]
                + g[prev_j][cur_i_left] + g[cur_j_right][next_i];
    }
   

    aux.cost = aux.cost + delta;

    vector<int> elements;
    elements.reserve(sizes);

    // [0, i_l)
    elements.insert(elements.end(), aux.sequence.begin(), aux.sequence.begin() + seg_1.first);
    
    // [j_l, j_r]
    elements.insert(elements.end(), aux.sequence.begin() + seg_2.first, aux.sequence.begin() + seg_2.second + 1);

    // (i_r, j_l)
    if (seg_1.second + 1 != seg_2.first){
        elements.insert(elements.end(), aux.sequence.begin() + seg_1.second + 1, aux.sequence.begin() + seg_2.first);        
    }

    // [i_l, i_r]
    elements.insert(elements.end(), aux.sequence.begin() + seg_1.first, aux.sequence.begin() + seg_1.second + 1);

    // (j_r, n)
    elements.insert(elements.end(), aux.sequence.begin() + seg_2.second + 1, aux.sequence.end());

    aux.sequence = elements;

    return aux;
}

Solution ILS(int maxIter, int maxIterIls){
    Solution bestOfAll;

    bestOfAll.cost = INF;
    for (int i = 0; i < maxIter; i++){
        Solution s = Construction();
        Solution best = s;

        int iterILS = 0;

        while(iterILS <= maxIterIls){
            LocalSearch(s);

            if (s.cost < best.cost){
                best = s;
                iterILS = 0;
            }
            
            s = Pertubation(best);

            iterILS++;
        }
        
        if (best.cost < bestOfAll.cost){
            bestOfAll = best;
        }
    }

    return bestOfAll;
}


int main(int argc, char** argv) {

    auto data = Data(argc, argv[1]);
    data.read();
    n = data.getDimension();

    g.resize(n + 1, vector<double> (n + 1));

    for (int i = 1; i <= n; i++){
        for (int j = 1; j <= n; j++){
            g[i][j] = data.getDistance(i, j);
        }
    }

    int maxIter = 50;
    int maxIterILS = (n >= 150 ? n/2 : n);

    auto start = chrono::high_resolution_clock::now();

    Solution SOL = ILS(maxIter, maxIterILS);

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start;

    cout << "TEMPO: " << duration.count() << "\n";

    cout << SOL.cost << "\n";
    for (int i = 0; i < SOL.sequence.size(); i++){
        cout << SOL.sequence[i] << " \n"[i == SOL.sequence.size() - 1];
    }

    return 0;
}
