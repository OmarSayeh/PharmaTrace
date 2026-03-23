#pragma once
#include "graph.h"
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

struct Dijkstraresult {
    bool pathfound = false;
    double totalrisk = 0.0;
    int nodesvisited = 0;
    vector<string> path;
};
class Minheap {
private:
    struct Heapnode {
        string drug;
        double dist;
    };

    vector<Heapnode> heap;
    unordered_map<string, int> position;

    void Heapifyup(int index);
    void Heapifydown(int index);
    void Swapnodes(int i, int j);

public:
    bool empty() const;
    bool contains(const string& drug) const;
    void push(const string& drug, double dist);
    HeapNode extractmin();
    void decreaseKey(const string& drug, double newdist);
};

Dijkstraresult dijkstra(const Graph& graph, const string& start, const string& target);
string formatPath(const vector<string>& path);
