#include "SubGradient.h"
#include "MST.h"

const int INF = 0x3f3f3f3f;

SubGradient::SubGradient(int _n) : lambda(_n), n(_n) {
    eps = 1.0;
    eps_min = 1e-5;
    k = 0;
    k_max = 30;
}

double SubGradient::cost(){
    return weight;
}

bool SubGradient::feasible(){
    return degree_check;
}

std::vector<Edge> SubGradient::edges(){
    return s;
}

double SubGradient::OneTree(std::vector<std::vector<double>> &matrix, std::vector<Edge> &S, std::vector<double> &lamb, double obj){
    Edge ed1, ed2;

    ed1.w = ed2.w = INF;

    for (int i = 1; i < n; i++){
        double value = matrix[0][i] - lamb[i];

        if (value < ed1.w){
            ed1 = {0, i, value};
        }
        else if (value < ed2.w) {
            ed2 = {0, i, value};
        }
    }

    S.push_back(ed1);
    S.push_back(ed2);

    return obj + ed1.w + ed2.w;
}

void SubGradient::solve(std::vector<std::vector<double>> &matrix, double upper_bound, std::vector<double> &lamb){
    if (lamb.size() == 0) lamb.resize(n);

    degree_check = false;

    while(eps > eps_min + 1e-7 && !degree_check){
        Kruskal kruskal(n);
        kruskal.init(matrix, lamb);
        kruskal.solve();
        
        // aqui é S e W, os valores do .h são s* e w*
        std::vector<Edge> S = move(kruskal.edges);
        double W = OneTree(matrix, S, lamb, kruskal.weight); 

        // std::cout << "ENTROU" << " " << W << "\n";

        if (W > weight){
            weight = W;
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

        if (W >= upper_bound) break;

        // S não s*
        std::vector<int> degree(n);
        for (int i = 0; i < S.size(); i++){
            auto [u, v, PESO] = S[i];
            degree[u]++;
            degree[v]++;
        }

        double form = 0;
        for (int i = 1; i < n; i++){
            form += (2 - degree[i]) * (2 - degree[i]);
        }

        if (form == 0) {
            degree_check = true;
            break;
        }

        double mi = eps * ((upper_bound - W) / form);

        for (int i = 1; i < n; i++){
            lamb[i] = lamb[i] + mi * (2 - degree[i]);
        }
    }

    // if (!degree_check){
    //     std::cout << "PROIBIDO" << "\n";
    // }
}
