#include "MST.h"

DSU::DSU(int n) : p(n), sz(n, 1) {
    iota(p.begin(), p.end(), 0);
}

int DSU::get(int a){
    if (a != p[a]) p[a] = get(p[a]);
    return p[a];
}

bool DSU::merge(int a, int b){
    a = get(a), b = get(b);
    if (a == b) return false;
    if (sz[a] > sz[b]) std::swap(a, b);
    p[a] = b;
    sz[b] += sz[a];
    return true;
}

Kruskal::Kruskal(int n) : dsu(n) {
    weight = 0.0;
};

void Kruskal::init(std::vector<std::vector<double>> &cost, std::vector<double> &lambda){
    int n = cost.size();

    // o nó 0 não aparece
    for (int i = 1; i < n; i++){
        for (int j = i+1; j < n; j++){
            g.push_back({i, j, cost[i][j] - lambda[i] - lambda[j]});
        }
    }
}

void Kruskal::solve(){
    int n = g.size();

    std::sort(g.begin(), g.end());

    for (int i = 0; i < n; i++){
        auto [u, v, w] = g[i];
        if (dsu.merge(dsu.get(u), dsu.get(v))){
            edges.push_back({u, v, w});
            weight += w;
        }
    }
}