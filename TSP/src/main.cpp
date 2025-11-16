#include "Data.h"
// #include <iostream>
#include <bits/stdc++.h>

using namespace std;

/*
Solution ILS(int maxIter, int maxIterIls){
    Solution bestOfAll;

    bestOfAll.cost = INF;
    for (int i = 0; i < maxIter; i++){
        Solution s = Construcao();
        Solution best = s;

        int iterILS = 0;

        while(iterIls <= maxIterIls){
            BuscaLocal (&s);

            if (s.cost < best.cost){
                best = s;
                iterIls = 0;
            }
            
            s = Perturbacao(best);

            iterIls++;
        }
        
        if (best.cost < bestOfAll.cost){
            bestOfAll = best;
        }
    }
}
*/

/*
CONSTRUCAO():


*/

// struct InsertionInfo{
//     int noInserido;
//     int arestaRemovida;
//     double custo;
// };

// vector<InsertionInfo> calcularCustoInsercao(Solution &s, vector<int> &CL){
//     vector<InsertionInfo> custoInsercao = vector<InsertionInfo> ((s.size() -1) * CL.size());

//     int l = 0;
//     for (int a = 0; a < s.sequence.size() -1; a++){
//         int i = s.sequence[a];
//         int j = s.sequence[a+1];
//         for (auto k : CL){
//             custoInsercao[l].custo = c[i][k] + c[j][k] - c[i][j];
//             custoInsercao[l].noInserido = k;
//             custoInsercao[l].arestaRemovida = a;
//             l++;
//         }
//     }

//     return custoInsercao;
// }

// Solution Construcao(){
//     Solution s;
//     s.sequence = escolher3NosAleatorios();
//     vector<int> CL = nosRestantes(); 
//     /*
//     ex: V = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
//     s.sequence = {1, 2, 9, 5, 1}
//     CL = {3, 4, 6, 7, 8, 10}
//     */

//     while (!CL.empty()){
//         vector<InsertionInfo> custoInsercao = calcularCustoInsercao(s, CL);

//         ordenarEmOrdemCrescente(custoInsercao);

//         double alpha (double) rand / RAND_MAX;

//         int selecionado = rand() % ((int) ceil (alpha * custoInsercao.size()));

//         inserirNaSolucao(s, custoInsercao[selecionado].noInserido);
//     }

//     return s;
// }



int main(int argc, char** argv) {

    auto data = Data(argc, argv[1]);
    data.read();
    size_t n = data.getDimension();

    cout << "Dimension: " << n << endl;
    cout << "DistanceMatrix: " << endl;
    data.printMatrixDist();


    cout << "Exemplo de Solucao s = ";
    double cost = 0.0;
    for (size_t i = 1; i < n; i++) {
        cout << i << " -> ";
        cost += data.getDistance(i, i+1);
    }
    cost += data.getDistance(n, 1);
    cout << n << " -> " << 1 << endl;
    cout << "Custo de S: " << cost << endl;

    return 0;
}
