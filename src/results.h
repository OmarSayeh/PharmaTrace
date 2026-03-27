#pragma once
#include <vector>
#include <string>
using namespace std;

struct AlgorithmResult {
    double pathWeight;
    vector<string> path;
    int nodesVisited;
    long long runtimeMs;
    bool hasNegativeCycle = false; // Bellman-Ford only
};