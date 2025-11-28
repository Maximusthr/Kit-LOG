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
    int noInserido;
    int arestaRemovida;
    double custo;
};

int n;
vector<vector<double>> g;

vector<InsertionInfo> calcularCustoInsercao(Solution &s, set<int> &CL){
    vector<InsertionInfo> custoInsercao((s.sequence.size() - 1) * CL.size());

    int l = 0;
    for (int a = 0; a < s.sequence.size() - 1; a++){
        int i = s.sequence[a];
        int j = s.sequence[a+1];
        for (auto k : CL){
            custoInsercao[l].custo = g[i][k] + g[j][k] - g[i][j];
            custoInsercao[l].noInserido = k;
            custoInsercao[l].arestaRemovida = a;
            l++;
        }
    }

    return custoInsercao;
}

void escolher3NosAleatorios(Solution &s, vector<bool> &elementos){
    uniform_int_distribution<int> dist(2, n);

    int achou = 0;
    while (achou != 3){
        int valor = dist(rng);

        if (!elementos[valor]){
            achou++;
            elementos[valor] = true;
            s.sequence.push_back(valor);
        }
    }
}

void nosRestantes(set<int> &CL, vector<bool> &elementos){
    for (int i = 1; i <= n; i++){
        if (!elementos[i]) CL.insert(i);
    }
}

void inserirNaSolucao(Solution &s, vector<InsertionInfo> &no, int selecionado){
    s.cost += no[selecionado].custo;
    
    s.sequence.insert(s.sequence.begin() + no[selecionado].arestaRemovida + 1, no[selecionado].noInserido);
}

Solution Construcao(){
    Solution s;
    
    s.cost = 0;

    vector<bool> elementos(n+1);
    elementos[1] = true;
    
    s.sequence.push_back(1);
    escolher3NosAleatorios(s, elementos);
    s.sequence.push_back(1);

    for (int i = 0; i < s.sequence.size() - 1; i++){
        s.cost += g[s.sequence[i]][s.sequence[i+1]];
    }
    
    set<int> CL;
    nosRestantes(CL, elementos);
   
    while (!CL.empty()){
        vector<InsertionInfo> custoInsercao = calcularCustoInsercao(s, CL);

        sort(custoInsercao.begin(), custoInsercao.end(), [&](InsertionInfo x, InsertionInfo y){
            return x.custo < y.custo;
        });

        double alpha = (double) rand() / RAND_MAX;

        int selecionado = rand() % ((int) ceil (alpha * (custoInsercao.size() - 1)));

        inserirNaSolucao(s, custoInsercao, selecionado);

        CL.erase(custoInsercao[selecionado].noInserido);
    }

    return s;
}

// // estrutura de vizinhança swap
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

            // corner case (adjacentes)
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

bool bestImprovementOrOpt (Solution &s, int bloco){

    double bestDelta = 0;
    int best_i, best_j;

    for (int i = 1; i < s.sequence.size() - bloco; i++){
        int atual_i = s.sequence[i];
        int bloco_atual = s.sequence[i + bloco - 1];
        int prox_i = s.sequence[i + bloco];
        int ant_i = s.sequence[i - 1];

        for (int j = 0; j < s.sequence.size() - 1; j++){
            if (j + 1 == i) {
                j += bloco;
                continue;
            }
            
            int j_atual = s.sequence[j];
            int j_prox = s.sequence[j + 1];

            double delta = - g[j_atual][j_prox] - g[ant_i][atual_i] - g[bloco_atual][prox_i]
                           + g[j_atual][atual_i] + g[bloco_atual][j_prox] + g[ant_i][prox_i];

            if (delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    if (bestDelta < 0){

        if (best_i > best_j){
            rotate(s.sequence.begin() + best_j + 1, s.sequence.begin() + best_i, s.sequence.begin() + best_i + bloco);
        }
        else if (best_i < best_j){
            rotate(s.sequence.begin() + best_i, s.sequence.begin() + best_i + bloco, s.sequence.begin() + best_j + 1);
        }

        
        s.cost = s.cost + bestDelta;
        return true;
    }

    return false;
}

void BuscaLocal (Solution &s){
    vector<int> NL = {1, 2, 3, 4, 5};
    // vector<int> NL = {3, 4, 5};
    bool improved = false;

    while (!NL.empty()){
        uniform_int_distribution<int> aleat(0, NL.size()-1);

        int escolhido = aleat(rng);

        switch(NL[escolhido]){
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
            NL.erase(NL.begin() + escolhido);
        }
    }
}

Solution ILS(int maxIter, int maxIterIls){
    Solution bestOfAll;

    bestOfAll.cost = INF;
    for (int i = 0; i < maxIter; i++){
        Solution s = Construcao();
        Solution best = s;

        int iterILS = 0;

        while(iterILS <= maxIterIls){
            BuscaLocal(s);

            if (s.cost < best.cost){
                best = s;
                iterILS = 0;
            }
            
        //     s = Perturbacao(best);

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
    int maxIterILS = (n/2 ? n >= 150 : n);

    Solution fim = ILS(maxIter, maxIterILS);

    cout << fim.cost << "\n";
    for (int i = 0; i < fim.sequence.size(); i++){
        cout << fim.sequence[i] << " \n"[i == fim.sequence.size() - 1];
    }

    // cout << "Dimension: " << n << endl;
    // cout << "DistanceMatrix: " << endl;
    // data.printMatrixDist();


    // cout << "Exemplo de Solucao s = ";
    // double cost = 0.0;
    // for (size_t i = 1; i < n; i++) {
    //     cout << i << " -> ";
    //     cost += data.getDistance(i, i+1);
    // }
    // cost += data.getDistance(n, 1);
    // cout << n << " -> " << 1 << endl;
    // cout << "Custo de S: " << cost << endl;

    return 0;
}
