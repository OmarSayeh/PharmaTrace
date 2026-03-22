#pragma once
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

// Represents one neighbor and the weight of the edge to it
struct Edge {
    string neighbor;
    double weight;
};

class Graph {
public:
    // adjacency list: drug name → list of (neighbor, weight)
    unordered_map<string, vector<Edge>> adjList;

    // total counts
    int edgeCount = 0;
    int nodeCount = 0;

    // Load graph from edges.csv
    void loadFromCSV(const string& filepath);
    
    // Check if a drug exists in the graph
    bool hasNode(const string& drug) const;

    // Print graph statistics
    void printStats() const;

    // Dijkstra's algorithm to find shortest path and distance between two drugs
    pair<double, vector<string>> dijkstraPath(const string& start, const string& end);
};