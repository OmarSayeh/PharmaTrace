#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
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