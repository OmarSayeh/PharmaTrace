#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <queue>
#include <functional>
#include <chrono>
using namespace std;

void Graph::loadFromCSV(const string& filepath) {
    ifstream file(filepath);

    if (!file.is_open()) {
        cerr << "ERROR: Could not open file: " << filepath << endl;
        return;
    }

    string line;

    // Skip header line (drug_a,drug_b,weight)
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string drug_a, drug_b, weight_str;

        getline(ss, drug_a,  ',');
        getline(ss, drug_b,  ',');
        getline(ss, weight_str, ',');
        
        // Skip malformed rows
        if (drug_a.empty() || drug_b.empty() || weight_str.empty()) continue;

        double weight;
        try {
            weight = std::stod(weight_str);
        } catch (const std::invalid_argument&) {
            continue; // skip rows with bad weight values
        } catch (const std::out_of_range&) {
            continue; // skip rows with numbers too large
}

        // Add edge in both directions (undirected graph)
        adjList[drug_a].push_back({drug_b, weight});
        adjList[drug_b].push_back({drug_a, weight});

        edgeCount++;
    }

    file.close();
    nodeCount = adjList.size();
}

bool Graph::hasNode(const string& drug) const {
return adjList.find(drug) != adjList.end();
}

void Graph::printStats() const {
    cout << "  Nodes (unique drugs): " << nodeCount  << endl;
    cout << "  Edges (interactions): " << edgeCount  << endl;

    // Find the drug with the most connections
    string mostConnected;
    int maxDegree = 0;
    for (auto& [drug, neighbors] : adjList) {
        if ((int)neighbors.size() > maxDegree) {
            maxDegree     = neighbors.size();
            mostConnected = drug;
        }
    }
    cout << "  Most connected drug:  " << mostConnected
        << " (" << maxDegree << " connections)" << endl;
}







// Dijkstra's algorithm to find shortest path and distance between two drugs
 DijkstraResult Graph::dijkstraPath(const string& start, const string& end){
    auto startTime =chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> parent;
    int nodesVisitedDijkstra = 0;

    // Initialize distances to infinity 
    for (auto& [node, _] : adjList) {
        dist[node] = numeric_limits<double>::infinity();        
    }

    // Min-heap priority queue to explore nodes by shortest distance
    priority_queue<
      pair<double, string>,
      vector<pair<double, string>>,
      greater<pair<double, string>>> 
      pq;

    // Push the start node with distance 0 into the priority queue
    dist[start] = 0.0;
    pq.push({0.0, start});

    // Main loop of Dijkstra's algorithm, loop until the priority queue is empty
    while(!pq.empty()){
        // u is current node we are looking at, currentDist is the distance to u from start
        auto [currentDist, u] = pq.top();
        pq.pop();

        nodesVisitedDijkstra++;

        if(currentDist > dist[u]) continue; // Skip if we have already found a shorter path to u
        if(u == end) break; // Stop if we reached the end node

        // Explore neighbors of u, v is the neighbor and weight is the edge weight from u to v
        for(auto& [v, weight] : adjList[u]){
            // If shorter path to v through u is found, update distance and parent
            if(dist[u] + weight < dist[v]){
                dist[v] = dist[u] + weight;
                parent[v] = u; 
                pq.push({dist[v], v}); // Push the updated distance for v into the priority queue to be looked at later
            }
        }
    }

    // Return infinity and empty path if end node is unreachable
    if(dist[end] == numeric_limits<double>::infinity()){
      auto endTime = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
        return {dist[end], {}, nodesVisitedDijkstra, duration};
    }

    // Reconstruct the path from end to start using the parent map
    vector<string> path;
    string curr = end;
    while(curr != start){
        path.push_back(curr);
        curr = parent[curr];
    }
    path.push_back(start);

    // Reverse the path to get it from start to end
    reverse(path.begin(), path.end()); 


    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
    // Return the path from start to end and the total distance of that path
    return {dist[end], path, nodesVisitedDijkstra, duration};

}