#include "dijkstra.h"
#include <limits>
#include <algorithm>
#include <chrono>
using namespace std;

// MinHeap implementation 

void MinHeap::swapNodes(int i, int j) {
    position[heap[i].drug] = j;
    position[heap[j].drug] = i;
    swap(heap[i], heap[j]);
}

void MinHeap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[parent].dist > heap[index].dist) {
            swapNodes(parent, index);
            index = parent;
        } else break;
    }
}

void MinHeap::heapifyDown(int index) {
    int size = heap.size();
    while (true) {
        int smallest = index;
        int left     = 2 * index + 1;
        int right    = 2 * index + 2;

        if (left  < size && heap[left].dist  < heap[smallest].dist) smallest = left;
        if (right < size && heap[right].dist < heap[smallest].dist) smallest = right;

        if (smallest != index) {
            swapNodes(index, smallest);
            index = smallest;
        } else break;
    }
}

bool MinHeap::empty() const {
    return heap.empty();
}

bool MinHeap::contains(const string& drug) const {
    return position.find(drug) != position.end();
}

void MinHeap::push(const string& drug, double dist) {
    heap.push_back({drug, dist});
    int index = heap.size() - 1;
    position[drug] = index;
    heapifyUp(index);
}

MinHeap::HeapNode MinHeap::extractMin() {
    HeapNode minNode = heap[0];
    int last = heap.size() - 1;

    swapNodes(0, last);
    position.erase(minNode.drug);
    heap.pop_back();

    if (!heap.empty()) heapifyDown(0);

    return minNode;
}

void MinHeap::decreaseKey(const string& drug, double newDist) {
    int index = position[drug];
    heap[index].dist = newDist;
    heapifyUp(index);
}

// Dijkstra's algorithm 

AlgorithmResult dijkstra(const Graph& graph,
                         const string& start,
                         const string& target) {
    auto startTime = chrono::high_resolution_clock::now();

    unordered_map<string, double> dist;
    unordered_map<string, string> parent;
    int nodesVisited = 0;

    // Initialize all distances to infinity
    for (auto& [node, _] : graph.adjList)
        dist[node] = numeric_limits<double>::infinity();

    dist[start] = 0.0;

    MinHeap pq;
    pq.push(start, 0.0);

    while (!pq.empty()) {
        auto [u, currentDist] = pq.extractMin();
        nodesVisited++;

        if (u == target) break;

        for (auto& edge : graph.adjList.at(u)) {
            string v  = edge.neighbor;
            double newDist = dist[u] + edge.weight;

            if (newDist < dist[v]) {
                dist[v]   = newDist;
                parent[v] = u;

                if (pq.contains(v))
                    pq.decreaseKey(v, newDist);
                else
                    pq.push(v, newDist);
            }
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    long long ms = chrono::duration_cast<chrono::milliseconds>
                   (endTime - startTime).count();

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