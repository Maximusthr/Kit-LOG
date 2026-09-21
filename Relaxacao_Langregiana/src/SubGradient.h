#ifndef SubGradient_H
#define SubGradient_H

#include "MST.h"

struct SubGradient {
    std::vector<double> lambda;
    double eps, eps_min;
    int k, k_max;
    int n;
    double weight;
    bool degree_check;

    std::vector<Edge> s;

    SubGradient(int _n);

    double cost();
    bool feasible();
    std::vector<Edge> edges();

    double OneTree(std::vector<std::vector<double>> &matrix, std::vector<Edge> &S, std::vector<double> &lamb, double obj);
    void solve(std::vector<std::vector<double>> &matrix, double upper_bound, std::vector<double> &lamb);
};

#endif