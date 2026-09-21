#ifndef MST_H
#define MST_H

#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>

struct Edge {
    int u, v;
    double w;

    bool operator<(Edge const &p) const {
        return w < p.w;
    };
};

struct DSU {
    std::vector<int> p;
    std::vector<int> sz;

    DSU (int n);

    int get(int a);
    bool merge(int a, int b);
};

struct Kruskal {
    std::vector<Edge> g, edges;
    double weight;

    Kruskal (int n);

    DSU dsu;
    
    void init(std::vector<std::vector<double>> &cost, std::vector<double> &lambda);
    void solve();
};

#endif