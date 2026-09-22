#include "SubGradient.h"
#include "MST.h"

const int INF = 0x3f3f3f3f;

SubGradient::SubGradient(int _n) : n(_n), lambda(n) {
    eps = 1.0;
    eps_min = 1e-5;
    k = 0;
    k_max = 30;
    best_W = 0.0;
}

double SubGradient::cost(){
    return best_W;
}

bool SubGradient::feasible(){
    return degree_check;
}

std::vector<Edge> SubGradient::Edges(){
    return s;
}

double SubGradient::OneTree(std::vector<std::vector<double>> &matrix, std::vector<Edge> &S, std::vector<double> &lamb){
    Edge ed1, ed2;

    ed1.w = ed2.w = INF;

    for (int i = 1; i < n; i++){
        double value = matrix[0][i] - lamb[i];

        if (value < ed1.w) {
            ed2 = ed1;
            ed1 = {0, i, value};
        }
        else if (value < ed2.w) {
            ed2 = {0, i, value};
        }
    }

    S.push_back(ed1);
    S.push_back(ed2);

    return ed1.w + ed2.w;
}

void SubGradient::solve(std::vector<std::vector<double>> &matrix, double UB, std::vector<double> &lamb){
    if (lamb.empty()) lamb.resize(n);

    degree_check = false;

    while(eps > eps_min){
        Kruskal kruskal(n);
        kruskal.init(matrix, lamb);
        kruskal.solve();
        
        // aqui é S e W, os valores do .h são s* e w*
        std::vector<Edge> S = move(kruskal.edges);
        double W = kruskal.weight + OneTree(matrix, S, lamb); 

        // colocar eps no best_W nao sei se afetou tanto
        if (W > best_W){
            best_W = W;
            lambda = lamb;
            s = S;
            k = 0;
        }
        else {
            k++;
            if (k >= k_max){
                k = 0;
                eps /= 2;
            }
        }

        if (W >= UB) break;
        
        // S não s*
        std::vector<int> degree(n);
        for (int i = 0; i < S.size(); i++){
            auto [u, v, PESO] = S[i];
            degree[u]++;
            degree[v]++;
        }
        
        double form = 0.0;
        for (int i = 1; i < n; i++){
            form += (2 - degree[i]) * (2 - degree[i]);
        }
        
        if (form < 1e-9) {
            degree_check = true;
            break;
        }

        double mi = eps * ((UB - W) / form);

        for (int i = 1; i < n; i++){
            lamb[i] = lamb[i] + mi * (2 - degree[i]);
        }
    }
}
