#pragma once
#include "graph.h"
#include "results.h"
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class MinHeap {
private:
    struct HeapNode {
        string drug;
        double dist;
    };

    vector<HeapNode> heap;
    unordered_map<string, int> position; // drug → index in heap

    void heapifyUp(int index);
    void heapifyDown(int index);
    void swapNodes(int i, int j);

public:
    bool empty() const;
    bool contains(const string& drug) const;
    void push(const string& drug, double dist);
    HeapNode extractMin();
    void decreaseKey(const string& drug, double newDist);
};

AlgorithmResult dijkstra(const Graph& graph,
                         const string& start,
                         const string& target);