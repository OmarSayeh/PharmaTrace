#include <iostream>
#include "graph.h"
using namespace std;    

int main() {
    cout << "================================" << endl;
    cout << "  PharmaTrace - Loading Graph...    " << endl;
    cout << "================================" << endl;

    Graph g;
    g.loadFromCSV("data/edges.csv");

    cout << "\nGraph loaded successfully!" << endl;
    g.printStats();

    // Quick lookup test
    string testDrug = "ASPIRIN";
    if (g.hasNode(testDrug)) {
        cout << "\n'" << testDrug << "' found in graph with "
                  << g.adjList[testDrug].size() << " connections." << endl;

        cout << "Top 5 interactions:" << endl;
        int count = 0;
        for (auto& edge : g.adjList[testDrug]) {
            cout << "  " << testDrug << " -- "
            << edge.neighbor << "  (weight: "
                      << edge.weight << ")" << endl;
            if (++count == 5) break;
        }
    } else {
        cout << "\n'" << testDrug << "' not found in graph." << endl;
    }

    return 0;
}