#ifndef SubGradient_H
#define SubGradient_H

#include "MST.h"

struct SubGradient {
    int n;
    int k, k_max;
    double best_W;
    bool degree_check;
    double eps, eps_min;
    std::vector<double> lambda;

    std::vector<Edge> s;

    SubGradient(int _n);

    double cost();
    bool feasible();
    std::vector<Edge> Edges();

    double OneTree(std::vector<std::vector<double>> &matrix, std::vector<Edge> &S, std::vector<double> &lamb);
    void solve(std::vector<std::vector<double>> &matrix, double upper_bound, std::vector<double> &lamb);
};

#endif