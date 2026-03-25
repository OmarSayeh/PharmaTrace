#include "menu.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <random>
#include <chrono>
using namespace std;

// Helpers 

string Menu::getInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}

// Convert user input to uppercase to match graph node format
string Menu::normalize(const string& input) {
    string result = input;
    transform(result.begin(), result.end(),
                   result.begin(), ::toupper);
    // trim leading/trailing spaces
    size_t start = result.find_first_not_of(" ");
    size_t end   = result.find_last_not_of(" ");
    if (start == string::npos) return "";
    return result.substr(start, end - start + 1);
}

void Menu::printHeader() {
    cout << "\n======================================" << endl;
    cout << "   PharmaTrace - Drug Interaction Network  " << endl;
    cout << "======================================" << endl;
}

void Menu::waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
}

//  Option 1: Find path between two drugs 

void Menu::findPath() {
    cout << "\n--- Find Interaction Path ---\n" << endl;

    string drugA = normalize(getInput("Enter Drug 1: "));
    string drugB = normalize(getInput("Enter Drug 2: "));

    // Validate both drugs exist in graph
    if (!graph.hasNode(drugA)) {
        cout << "\n⚠ '" << drugA << "' not found in database." << endl;
        cout << "  Tip: use option 3 to search for a drug name." << endl;
        waitForEnter();
        return;
    }
    if (!graph.hasNode(drugB)) {
        cout << "\n⚠ '" << drugB << "' not found in database." << endl;
        cout << "  Tip: use option 3 to search for a drug name." << endl;
        waitForEnter();
        return;
    }

    cout << "\nRunning Dijkstra's Algorithm..." << endl;
    // ── TODO : replace stub with real Dijkstra call ──────────────────
    // DijkstraResult d = dijkstra(graph, drugA, drugB);
    auto result = graph.dijkstraPath(drugA, drugB);
    if(result.path.empty()){
        cout<<"No path found between "<<drugA<<" and "<<drugB<<"."<<endl;
    }else{
        // Print path
        cout<<"  Path: ";
        for(size_t i=0; i<result.path.size(); i++){
            cout<<result.path[i];
            if(i != result.path.size()-1) cout<<" → ";
        }
        cout<<endl;
        // Print risk score
        cout<<"  Risk Score: "<<result.distance<<endl;
        // Print nodes visited
        cout<<"  Nodes Visited: "<<result.nodesVisited<<endl;
        // Print runtime
        cout<<"  Runtime: "<<result.durationMs<<" ms"<<endl;

    }
    
    auto bfResult = graph.bellmanFordPath(drugA, drugB);

    if (bfResult.path.empty()) {
        cout << "No path found between " << drugA << " and " << drugB << "." << endl;
    } else {
        cout << "  Path: ";
        for (size_t i = 0; i < bfResult.path.size(); i++) {
            cout << bfResult.path[i];
            if (i != bfResult.path.size() - 1) cout << " → ";
        }
        cout << endl;
        cout << "  Risk Score: " << bfResult.distance << endl;
        cout << "  Nodes Visited: " << bfResult.nodesVisited << endl;
        cout << "  Runtime: " << bfResult.durationMs << " ms" << endl;
    }
    waitForEnter();
}

// Option 2: Show all interactions for a drug 

void Menu::showInteractions() {
    cout << "\n--- Show Drug Interactions ---\n" << endl;

    string drug = normalize(getInput("Enter drug name: "));

    if (!graph.hasNode(drug)) {
        cout << "\n⚠ '" << drug << "' not found in database." << endl;
        cout << "  Tip: use option 3 to search for a drug name." << endl;
        waitForEnter();
        return;
    }

    auto& neighbors = graph.adjList[drug];

    // Sort by weight descending (highest risk first)
    vector<Edge> sorted = neighbors;
    sort(sorted.begin(), sorted.end(),
              [](const Edge& a, const Edge& b) {
                  return a.weight > b.weight;
              });

    cout << "\nTop 10 interactions for " << drug
              << " (" << neighbors.size() << " total):\n" << endl;

    cout << "  " << left
              << setw(35) << "Drug"
              << setw(10) << "Risk Score"
              << endl;
    cout << "  " << string(45, '-') << endl;

    int count = 0;
    for (auto& edge : sorted) {
        cout << "  " << left
                  << setw(35) << edge.neighbor
                  << setw(10) << fixed
                  << setprecision(4) << edge.weight
                  << endl;
        if (++count == 10) break;
    }

    waitForEnter();
}

// Option 3: Search for a drug 

void Menu::searchDrug() {
    cout << "\n--- Search Drug ---\n" << endl;

    string query = normalize(getInput("Enter search term: "));

    vector<string> matches;
    for (auto& [drug, _] : graph.adjList) {
        if (drug.find(query) != string::npos) {
            matches.push_back(drug);
        }
    }

    if (matches.empty()) {
        cout << "\n No drugs found matching '" << query << "'" << endl;
    } else {
        sort(matches.begin(), matches.end());
        cout << "\n Found " << matches.size()
                  << " match(es) for '" << query << "':\n" << endl;
        int count = 0;
        for (auto& match : matches) {
            cout << "  " << match << endl;
            if (++count == 20) {
                cout << "  ... and " << matches.size() - 20
                          << " more" << endl;
                break;
            }
        }
    }

    waitForEnter();
}

// Option 4: Benchmark 

void Menu::runBenchmark() {
    void Menu::runBenchmark() {
    cout << "\n--- Algorithm Benchmark ---\n" << endl;
    cout << "Running 100 random drug pair queries...\n" << endl;

    if (graph.adjList.empty()) {
        cout << "Graph not loaded.\n";
        waitForEnter();
        return;
    }

    vector<string> drugs;
    for (auto& [drug, _] : graph.adjList) {
        drugs.push_back(drug);
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, drugs.size() - 1);
    long long totalDijkstraTime = 0;
    long long totalBellmanTime = 0;
    int totalDijkstraVisited = 0;
    int totalBellmanVisited = 0;
    int validTests = 0;
    int TESTS = 100;
        
    for (int i = 0; i < TESTS; i++) {
        string a = drugs[dis(gen)];
        string b = drugs[dis(gen)];

        if (a == b) continue;

        auto d = graph.dijkstraPath(a, b);
        auto bf = graph.bellmanFordPath(a, b);

        if (d.path.empty() || bf.path.empty()) continue;

        totalDijkstraTime += d.durationMs;
        totalBellmanTime += bf.durationMs;
        totalDijkstraVisited += d.nodesVisited;
        totalBellmanVisited += bf.nodesVisited;

        validTests++;
    }

    if (validTests == 0) {
        cout << "No valid paths found for benchmark.\n";
        waitForEnter();
        return;
    }

    cout << "Benchmark Results (" << validTests << " valid tests)\n" << endl;
    cout << "Dijkstra Average Runtime: "
         << (double)totalDijkstraTime / validTests << " ms\n";
    cout << "Bellman-Ford Average Runtime: "
         << (double)totalBellmanTime / validTests << " ms\n";
    cout << "Dijkstra Avg Nodes Visited: "
         << totalDijkstraVisited / validTests << endl;
    cout << "Bellman-Ford Avg Nodes Visited: "
         << totalBellmanVisited / validTests << endl;
    waitForEnter();
}

//  Main loop 

void Menu::run() {
    string choice;
    
    while (true) {
        printHeader();
        cout << "  Loaded: " << graph.nodeCount << " drugs, "
                  << graph.edgeCount << " interactions\n" << endl;
        cout << "  1. Find interaction path between two drugs"  << endl;
        cout << "  2. Show all interactions for a drug"         << endl;
        cout << "  3. Search for a drug"                        << endl;
        cout << "  4. Run benchmark (Dijkstra vs Bellman-Ford)" << endl;
        cout << "  5. Exit\n"                                   << endl;

        choice = getInput("  Select option (1-5): ");

        if      (choice == "1") findPath();
        else if (choice == "2") showInteractions();
        else if (choice == "3") searchDrug();
        else if (choice == "4") runBenchmark();
        else if (choice == "5") {
            cout << "\nGoodbye!\n" << endl;
            break;
        }
        else {
            cout << "\n Invalid option. Please enter 1-5." << endl;
            waitForEnter();
        }
    }
}
