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

Solution Perturbacao(Solution SOL){
    
    // cout << "Entrou" << "\n";

    int tamanho = SOL.sequence.size();
    
    int inicio = 2;
    int fim = (tamanho + 9)/10;
    
    auto Intersec = [&](int x1, int y1, int x2, int y2) -> bool {
        // true = tem interseção
        return ((y2 >= x1 && y2 <= y1) || (y1 >= x2 && y1 <= y2));
        // return max(x1, x2) <= min(y1, y2);
    };
    
    pair<int, int> seg_1, seg_2;
    
    do {
        uniform_int_distribution<int> aleat_1(inicio, fim);
        uniform_int_distribution<int> aleat_2(inicio, fim);
        
        int v1 = aleat_1(rng);
        int v2 = aleat_2(rng);

        uniform_int_distribution<int> SEG_ELEM_1(1, tamanho - v1 - 2);
        uniform_int_distribution<int> SEG_ELEM_2(1, tamanho - v2 - 2);

        seg_1.first = SEG_ELEM_1(rng);
        seg_1.second = seg_1.first + v1 - 1;

        
        seg_2.first = SEG_ELEM_2(rng);
        seg_2.second = seg_2.first + v2 - 1;

    } while (Intersec(seg_1.first, seg_1.second, seg_2.first, seg_2.second));

    Solution aux = SOL;
    
    // cout << "Anterior: " << aux.cost << "\n";
    
    // if (seg_1.second > seg_2.second) swap(seg_1, seg_2);
    if (seg_1.first > seg_2.first) swap(seg_1, seg_2);


    int i_atual_ant = aux.sequence[seg_1.first - 1];
    int i_atual_L = aux.sequence[seg_1.first];
    int i_atual_R = aux.sequence[seg_1.second];
    int i_atual_prox = aux.sequence[seg_1.second + 1];

    int j_atual_ant = aux.sequence[seg_2.first - 1];
    int j_atual_L = aux.sequence[seg_2.first];
    int j_atual_R = aux.sequence[seg_2.second];
    int j_atual_prox = aux.sequence[seg_2.second + 1];

    double delta = 0.0;

    // corner case
    if (seg_1.second + 1 == seg_2.first){
        delta = - g[i_atual_ant][i_atual_L] - g[i_atual_R][i_atual_prox] - g[j_atual_R][j_atual_prox]
                + g[i_atual_ant][j_atual_L] + g[i_atual_R][j_atual_prox] + g[j_atual_R][i_atual_L];
    }
    else {
        delta = - g[i_atual_ant][i_atual_L] - g[i_atual_R][i_atual_prox]
                - g[j_atual_ant][j_atual_L] - g[j_atual_R][j_atual_prox]
                + g[i_atual_ant][j_atual_L] + g[i_atual_R][j_atual_prox]
                + g[j_atual_ant][i_atual_L] + g[j_atual_R][i_atual_prox];
    }
   

    aux.cost = aux.cost + delta;

    // cout << "Novo: " << aux.cost << "\n\n";

    vector<int> elementos;
    elementos.reserve(tamanho);

    // [0, i_l)
    elementos.insert(elementos.end(), aux.sequence.begin(), aux.sequence.begin() + seg_1.first);
    
    // [j_l, j_r]
    elementos.insert(elementos.end(), aux.sequence.begin() + seg_2.first, aux.sequence.begin() + seg_2.second + 1);

    // (i_r, j_l)
    if (seg_1.second + 1 != seg_2.first){
        elementos.insert(elementos.end(), aux.sequence.begin() + seg_1.second + 1, aux.sequence.begin() + seg_2.first);        
    }

    // [i_l, i_r]
    elementos.insert(elementos.end(), aux.sequence.begin() + seg_1.first, aux.sequence.begin() + seg_1.second + 1);

    // (j_r, n)
    elementos.insert(elementos.end(), aux.sequence.begin() + seg_2.second + 1, aux.sequence.end());

    aux.sequence = elementos;

    return aux;
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
            
            s = Perturbacao(best);

            // cout << s.cost << "\n\n";

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

    Solution fim = ILS(maxIter, maxIterILS);

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start;

    cout << "TEMPO: " << duration.count() << "\n";

    cout << fim.cost << "\n";
    for (int i = 0; i < fim.sequence.size(); i++){
        cout << fim.sequence[i] << " \n"[i == fim.sequence.size() - 1];
    }

    return 0;
}
