#include "Data.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <set>
#include <vector>

using namespace std;
using ll = long long;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

const int INF = (int)1e9;
const ll LINF = (ll)1e18;

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


Solution ILS(int maxIter, int maxIterIls){
    Solution bestOfAll;

    bestOfAll.cost = INF;
    for (int i = 0; i < maxIter; i++){
        Solution s = Construcao();
        Solution best = s;

        // int iterILS = 0;

        // while(iterIls <= maxIterIls){
        //     BuscaLocal (&s);

        //     if (s.cost < best.cost){
        //         best = s;
        //         iterIls = 0;
        //     }
            
        //     s = Perturbacao(best);

        //     iterIls++;
        // }
        
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
