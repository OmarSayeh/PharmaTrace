#pragma once
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

struct Edge {
    string neighbor;
    double weight;
};

class Graph {
public:
    unordered_map<string, vector<Edge>> adjList;
    int edgeCount = 0;
    int nodeCount = 0;

    void loadFromCSV(const string& filepath);
    bool hasNode(const string& drug) const;
    void printStats() const; 
    
};
