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

int n;
vector<vector<double>> g;

struct Subsequence {
    double T, C;
    int W;
    int first, last;
    inline static Subsequence Concatenate(Subsequence &sigma_1, Subsequence &sigma_2){
        Subsequence sigma;
        double temp = g[sigma_1.last][sigma_2.first];
        sigma.W = sigma_1.W + sigma_2.W;
        sigma.T = sigma_1.T + temp + sigma_2.T;
        sigma.C = sigma_1.C + sigma_2.W * (sigma_1.T + temp) + sigma_2.C;
        sigma.first = sigma_1.first;
        sigma.last = sigma_2.last;
        
        return sigma;
    }
};

vector<vector<Subsequence>> subseq_matrix;

struct Solution {
    vector<int> sequence;
    double cost = 0.0;
};

struct InsertionInfo{
    int InsertNode;
    double cost;
};


Solution ILS(int maxIter, int maxIterIls);

Solution Construction();
vector<InsertionInfo> InsertionCost(Solution &s, set<int> &CL);
void chooseRandomNodes(Solution &s, vector<bool> &elements);
void RemainingNodes(set<int> &CL, vector<bool> &elements);
void InsertingSolution(Solution &s, vector<InsertionInfo> &node, int selected);

void LocalSearch (Solution &s);
bool bestImprovementSwap(Solution &s);
bool bestImprovement2Opt(Solution &s);
bool bestImprovementOrOpt (Solution &s, int block);

void UpdateAllSubseq(Solution s);
void UpdateSpecificSubseq(Solution s, int new_start, int new_end);

Solution Pertubation(Solution SOL);

int main(int argc, char** argv) {

    auto data = Data(argc, argv[1]);
    data.read();
    n = data.getDimension();

    g.resize(n + 1, vector<double> (n + 1));
    subseq_matrix.resize(n+1, vector<Subsequence>(n+1));

    for (int i = 1; i <= n; i++){
        for (int j = 1; j <= n; j++){
            g[i][j] = data.getDistance(i, j);
        }
    }

    int maxIter = 10;
    int maxIterILS = min(100, n);

    int runs = 1;
    double totalTime = 0.0, totalCost = 0.0;

    for (int i = 0; i < runs; i++){
        auto start = chrono::high_resolution_clock::now();
    
        Solution SOL = ILS(maxIter, maxIterILS);
    
        auto end = chrono::high_resolution_clock::now();
    
        chrono::duration<double> duration = end - start;
        totalTime += duration.count();
        totalCost += SOL.cost;
    }

    cout << totalTime / runs << " " << totalCost / runs << "\n\n";

    return 0;
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


Solution Construction(){
    Solution s;
    s.cost = 0;

    uniform_int_distribution<int> value(0, 25);
    double alpha = (double) value(rng) / 100;
    
    vector<bool> elements(n+1);
    elements[1] = true;
    
    s.sequence.push_back(1);
        
    set<int> CL;
    RemainingNodes(CL, elements);
        
    while (!CL.empty()){
        vector<InsertionInfo> cost_Insertion;
        
        int r = s.sequence.back();
        
        for (auto node = CL.begin(); node != CL.end(); node++){
            double cost = g[r][*node];
            cost_Insertion.push_back({*node, cost});
        }
        
        sort(cost_Insertion.begin(), cost_Insertion.end(), [&](InsertionInfo x, InsertionInfo y){
            return x.cost < y.cost;
        });

        int range = (int) ceil(alpha * (cost_Insertion.size()-1));
        if (range < 1) range = 1;

        int selected = rand() % range;
        s.sequence.push_back(cost_Insertion[selected].InsertNode);

        CL.erase(cost_Insertion[selected].InsertNode);
    }

    s.sequence.push_back(1);

    UpdateAllSubseq(s);

    s.cost = subseq_matrix[0][n].C;

    cout << s.cost << "\n";

    return s;
}


vector<InsertionInfo> InsertionCost(Solution &s, set<int> &CL){
    vector<InsertionInfo> cost_Insertion((s.sequence.size() - 1) * CL.size());

    int l = 0;
    for (int a = 0; a < s.sequence.size() - 1; a++){
        int i = s.sequence[a];
        int j = s.sequence[a+1];
        for (auto k : CL){
            cost_Insertion[l].cost = g[i][k] + g[j][k] - g[i][j];
            cost_Insertion[l].InsertNode = k;
            // cost_Insertion[l].RemovedEdge = a;
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
        if (!elements[i]) {
            CL.insert(i);
            elements[i] = true;
        }
    }
}


void InsertingSolution(Solution &s, vector<InsertionInfo> &node, int selected){
    s.cost += node[selected].cost;
    
    // s.sequence.insert(s.sequence.begin() + node[selected].RemovedEdge + 1, node[selected].InsertNode);
}


void LocalSearch (Solution &s){
    // vector<int> NL = {1, 2, 3, 4, 5};
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
                improved = bestImprovement2Opt(s);
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
            // NL = {1, 2, 3, 4, 5};
            NL = {1, 2, 3, 4, 5};
        }
        else {
            NL.erase(NL.begin() + choice);
        }
    }
}


bool bestImprovementSwap(Solution &s){
    double bestDelta = s.cost;
    int best_i, best_j;

    for (int i = 1; i < s.sequence.size() - 1; i++){
        for (int j = i + 1; j < s.sequence.size() - 1; j++){
            
            Subsequence sigma;
            
            // corner case
            if (i + 1 == j){
                sigma = Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][i]);
                sigma = Subsequence::Concatenate(sigma, subseq_matrix[j+1][n]);
            }
            else {
                sigma = Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][i+1]);
                sigma = Subsequence::Concatenate(sigma, subseq_matrix[j-1][i]);
                sigma = Subsequence::Concatenate(sigma, subseq_matrix[j+1][n]);
            }

            if (sigma.C < bestDelta){
                bestDelta = sigma.C;
                best_i = i;
                best_j = j;
            }
        }
    }

    if (bestDelta < s.cost){
        s.cost = bestDelta;
        swap(s.sequence[best_i], s.sequence[best_j]);
        UpdateSpecificSubseq(s, best_i, best_j);
        return true;
    }

    return false;
}


bool bestImprovement2Opt(Solution &s){  
    double bestDelta = s.cost;
    int best_i, best_j;

    Subsequence sigma_1, sigma_2;

    for (int i = 0; i < s.sequence.size() - 2; i++){
        for (int j = i + 1; j < s.sequence.size() - 1; j++){
            sigma_1 = Subsequence::Concatenate(subseq_matrix[0][i], subseq_matrix[j][i+1]);
            sigma_2 = Subsequence::Concatenate(sigma_1, subseq_matrix[j+1][n]);

            if (sigma_2.C < bestDelta){
                best_i = i;
                best_j = j;
                bestDelta = sigma_2.C;
            }
        }
    }

    if (bestDelta < s.cost){
        s.cost = bestDelta;

        reverse(s.sequence.begin() + best_i + 1, s.sequence.begin() + best_j + 1);
        UpdateAllSubseq(s);
        return true;
    }

    return false;
}


bool bestImprovementOrOpt (Solution &s, int block){

    double bestDelta = s.cost;
    int best_i, best_j;

    for (int i = 1; i < s.sequence.size() - block; i++){
        for (int j = 0; j < s.sequence.size() - 1; j++){
            if (j + 1 == i) {
                j += block;
                continue;
            }   
            
            Subsequence sigma;

            if (j < i){
                sigma = Subsequence::Concatenate(subseq_matrix[0][j], subseq_matrix[i][i + block - 1]);
                sigma = Subsequence::Concatenate(sigma, subseq_matrix[j + 1][i - 1]);
                sigma = Subsequence::Concatenate(sigma, subseq_matrix[i + block][n]);
            }
            else {
                sigma = Subsequence::Concatenate(subseq_matrix[0][i], subseq_matrix[i + block - 1][j]);
                sigma = Subsequence::Concatenate(sigma, subseq_matrix[i - 1][i + block]);
                sigma = Subsequence::Concatenate(sigma, subseq_matrix[j + 1][n]);
            }

            if (sigma.C < bestDelta){
                bestDelta = sigma.C;
                best_i = i;
                best_j = j;
            }
        }
    }

    if (bestDelta < s.cost){
        s.cost = bestDelta;

        if (best_i > best_j){
            rotate(s.sequence.begin() + best_j + 1, s.sequence.begin() + best_i, s.sequence.begin() + best_i + block);
        }
        else if (best_i < best_j){
            rotate(s.sequence.begin() + best_i, s.sequence.begin() + best_i + block, s.sequence.begin() + best_j + 1);
        }

        UpdateSpecificSubseq(s, min(best_i, best_j), max(best_j, best_i + block - 1));

        return true;
    }

    return false;
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
    
    UpdateAllSubseq(aux);
    aux.cost = subseq_matrix[0][n].C;

    return aux;
}

void UpdateAllSubseq(Solution s){

    for (int i = 0; i <= n; i++){
        int v = s.sequence[i];
        subseq_matrix[i][i].W = (i > 0);
        subseq_matrix[i][i].C = 0;
        subseq_matrix[i][i].T = 0;
        subseq_matrix[i][i].first = s.sequence[i];
        subseq_matrix[i][i].last = s.sequence[i];
    }

    for (int i = 0; i <= n; i++){
        for (int j = i + 1; j <= n; j++){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j-1], subseq_matrix[j][j]);
        }
    }

    // subsequencias invertidas
    // para o 2-opt
    for (int i = n; i >= 0; i--){
        for (int j = i - 1; j >= 0; j--){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j+1], subseq_matrix[j][j]);
        }
    }
}

void UpdateSpecificSubseq(Solution s, int new_start, int new_end){

    for (int i = new_start; i <= new_end; i++){
        int v = s.sequence[i];
        subseq_matrix[i][i].W = (i > 0);
        subseq_matrix[i][i].C = 0;
        subseq_matrix[i][i].T = 0;
        subseq_matrix[i][i].first = s.sequence[i];
        subseq_matrix[i][i].last = s.sequence[i];
    }

    for (int i = 0; i <= new_end; i++){
        for (int j = max(new_start, i + 1); j <= n; j++){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j-1], subseq_matrix[j][j]);
        }
    }

    for (int i = n; i >= new_start; i--){
        for (int j = min(new_end, i - 1); j >= 0; j--){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j+1], subseq_matrix[j][j]);
        }
    }
}