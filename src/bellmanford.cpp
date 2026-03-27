#include "bellmanford.h"
#include <limits>
#include <algorithm>
#include <chrono>
using namespace std;

AlgorithmResult bellmanFord(const Graph& graph,
                            const string& start,
                            const string& target) {
    auto startTime = chrono::high_resolution_clock::now();

    unordered_map<string, double> dist;
    unordered_map<string, string> parent;
    int nodesVisited = 0;

    for (auto& [node, _] : graph.adjList)
        dist[node] = numeric_limits<double>::infinity();

    dist[start] = 0.0;

    // Relax edges nodeCount - 1 times
    for (int i = 0; i < graph.nodeCount - 1; i++) {
        bool updated = false;

        for (auto& [u, neighbors] : graph.adjList) {
            if (dist[u] == numeric_limits<double>::infinity()) continue;

            for (auto& edge : neighbors) {
                nodesVisited++;
                double newDist = dist[u] + edge.weight;

                if (newDist < dist[edge.neighbor]) {
                    dist[edge.neighbor]   = newDist;
                    parent[edge.neighbor] = u;
                    updated = true;
                }
            }
        }

        if (!updated) break; // early exit if no updates
    }

    auto endTime = chrono::high_resolution_clock::now();
    long long ms = chrono::duration_cast<chrono::milliseconds>
                   (endTime - startTime).count();

    // Negative cycle check
    for (auto& [u, neighbors] : graph.adjList) {
        if (dist[u] == numeric_limits<double>::infinity()) continue;
        for (auto& edge : neighbors) {
            if (dist[u] + edge.weight < dist[edge.neighbor])
                return {numeric_limits<double>::infinity(), {}, nodesVisited, ms, true};
        }
    }

    // Unreachable
    if (dist[target] == numeric_limits<double>::infinity())
        return {dist[target], {}, nodesVisited, ms};

    // Reconstruct path
    vector<string> path;
    string curr = target;
    while (curr != start) {
        path.push_back(curr);
        curr = parent[curr];
    }
    path.push_back(start);
    reverse(path.begin(), path.end());

    return {dist[target], path, nodesVisited, ms};
}