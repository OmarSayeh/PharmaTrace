#include <iostream>
#include "graph.h"
#include "menu.h"
using namespace std;

int main() {
    cout << "Loading graph..." << endl;

    Graph g;
    g.loadFromCSV("data/edges.csv");

    cout << "Graph loaded successfully! ("
              << g.nodeCount << " drugs, "
              << g.edgeCount << " interactions)\n" << endl;

    Menu menu(g);
    menu.run();

    return 0;
}