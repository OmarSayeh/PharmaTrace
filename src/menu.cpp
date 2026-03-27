#include "menu.h"
#include "dijkstra.h"
#include "bellmanford.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <random>
#include <chrono>
using namespace std;

//  Helpers 

string Menu::getInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}

string Menu::normalize(const string& input) {
    string result = input;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    size_t start = result.find_first_not_of(" ");
    size_t end   = result.find_last_not_of(" ");
    if (start == string::npos) return "";
    return result.substr(start, end - start + 1);
}

void Menu::printHeader() {
    cout << "\n========================================" << endl;
    cout << "  PharmaTrace - Drug Interaction Network " << endl;
    cout << "========================================" << endl;
}

void Menu::waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
}
void Menu::printRiskAdvice(double riskScore) {
    cout << "\n  ──────────────────────────────────────────" << endl;

    if (riskScore >= 0.75) {
        cout << "  ⛔ AVOID: High interaction risk"           << endl;
        cout << "     These drugs have a strong history of"  << endl;
        cout << "     dangerous co-adverse events. Do NOT"   << endl;
        cout << "     take together without direct physician" << endl;
        cout << "     supervision."                          << endl;
    } else if (riskScore >= 0.45) {
        cout << "  ⚠  CAUTION: Moderate interaction risk"    << endl;
        cout << "     Adverse events have been reported for"  << endl;
        cout << "     this combination. Monitor closely and"  << endl;
        cout << "     inform your doctor of all medications." << endl;
    } else {
        cout << "  ✅ LOW RISK: Minimal interaction risk"     << endl;
        cout << "     Few adverse events reported for this"   << endl;
        cout << "     combination. Still inform your doctor"  << endl;
        cout << "     of all medications you are taking."     << endl;
    }

    cout << "  ──────────────────────────────────────────" << endl;
    cout << "  ⚕  DISCLAIMER: This tool is for"            << endl;
    cout << "     informational purposes only. Always"      << endl;
    cout << "     consult a licensed physician or"          << endl;
    cout << "     pharmacist before making any"             << endl;
    cout << "     medication decisions."                    << endl;
    cout << "  ──────────────────────────────────────────" << endl;
}

//  Option 1: Find path between two drugs 

void Menu::findPath() {
    cout << "\n--- Find Interaction Path ---\n" << endl;

    string drugA = normalize(getInput("Enter Drug 1: "));
    string drugB = normalize(getInput("Enter Drug 2: "));

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

    // Dijkstra 
    cout << "\nRunning Dijkstra's Algorithm..." << endl;
    AlgorithmResult d = dijkstra(graph, drugA, drugB);

    if (d.path.empty()) {
        cout << "  No path found between " << drugA << " and " << drugB << "." << endl;
    } else {
        cout << "  Path:          ";
        for (size_t i = 0; i < d.path.size(); i++) {
            cout << d.path[i];
            if (i != d.path.size() - 1) cout << " → ";
        }
        cout << endl;
        cout << "  Risk Score:    " << fixed << setprecision(4) << d.pathWeight  << endl;
        cout << "  Nodes Visited: " << d.nodesVisited                            << endl;
        cout << "  Runtime:       " << d.runtimeMs << " ms"                      << endl;
    }

    //  Bellman-Ford 
    cout << "\nRunning Bellman-Ford Algorithm..." << endl;
    AlgorithmResult bf = bellmanFord(graph, drugA, drugB);

    if (bf.hasNegativeCycle) {
        cout << "  ⚠ Negative cycle detected in graph." << endl;
    } else if (bf.path.empty()) {
        cout << "  No path found between " << drugA << " and " << drugB << "." << endl;
    } else {
        cout << "  Path:          ";
        for (size_t i = 0; i < bf.path.size(); i++) {
            cout << bf.path[i];
            if (i != bf.path.size() - 1) cout << " → ";
        }
        cout << endl;
        cout << "  Risk Score:    " << fixed << setprecision(4) << bf.pathWeight << endl;
        cout << "  Nodes Visited: " << bf.nodesVisited                           << endl;
        cout << "  Runtime:       " << bf.runtimeMs << " ms"                     << endl;
    }

    // Comparison 
    if (!d.path.empty() && !bf.path.empty()) {
        cout << "\n--- Comparison ---" << endl;
        cout << "  Dijkstra was "
             << (d.runtimeMs <= bf.runtimeMs ? "faster" : "slower")
             << " by " << abs(d.runtimeMs - bf.runtimeMs) << " ms" << endl;
        cout << "  Dijkstra visited "
             << abs(d.nodesVisited - bf.nodesVisited)
             << (d.nodesVisited <= bf.nodesVisited ? " fewer" : " more")
             << " nodes than Bellman-Ford" << endl;
    }

     // ── Risk Advice ───────────────────────────────────────────────────────────
    if (!d.path.empty()) {
        printRiskAdvice(d.pathWeight);
    }

    waitForEnter();
}


//Option 2: Show all interactions for a drug 

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

    vector<Edge> sorted = neighbors;
    sort(sorted.begin(), sorted.end(),
         [](const Edge& a, const Edge& b) {
             return a.weight > b.weight;
         });

    cout << "\nTop 10 interactions for " << drug
         << " (" << neighbors.size() << " total):\n" << endl;

    cout << "  " << left << setw(35) << "Drug"
                         << setw(10) << "Risk Score" << endl;
    cout << "  " << string(45, '-') << endl;

    int count = 0;
    for (auto& edge : sorted) {
        cout << "  " << left
             << setw(35) << edge.neighbor
             << setw(10) << fixed << setprecision(4) << edge.weight << endl;
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
        cout << "\n  No drugs found matching '" << query << "'" << endl;
    } else {
        sort(matches.begin(), matches.end());
        cout << "\n  Found " << matches.size()
             << " match(es) for '" << query << "':\n" << endl;
        int count = 0;
        for (auto& match : matches) {
            cout << "  " << match << endl;
            if (++count == 20) {
                cout << "  ... and " << matches.size() - 20 << " more" << endl;
                break;
            }
        }
    }

    waitForEnter();
}

//  Option 4: Benchmark 

void Menu::runBenchmark() {
    cout << "\n--- Algorithm Benchmark ---\n" << endl;
    cout << "Running 100 random drug pair queries...\n" << endl;

    if (graph.adjList.empty()) {
        cout << "  Graph not loaded." << endl;
        waitForEnter();
        return;
    }

    vector<string> drugs;
    for (auto& [drug, _] : graph.adjList)
        drugs.push_back(drug);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, drugs.size() - 1);

    long long totalDijkstraTime   = 0;
    long long totalBellmanTime    = 0;
    int       totalDijkstraNodes  = 0;
    int       totalBellmanNodes   = 0;
    int       validTests          = 0;
    const int TESTS               = 100;

    for (int i = 0; i < TESTS; i++) {
        string a = drugs[dis(gen)];
        string b = drugs[dis(gen)];
        if (a == b) continue;

        AlgorithmResult d  = dijkstra(graph, a, b);
        AlgorithmResult bf = bellmanFord(graph, a, b);

        if (d.path.empty() || bf.path.empty()) continue;

        totalDijkstraTime  += d.runtimeMs;
        totalBellmanTime   += bf.runtimeMs;
        totalDijkstraNodes += d.nodesVisited;
        totalBellmanNodes  += bf.nodesVisited;
        validTests++;
    }

    if (validTests == 0) {
        cout << "  No valid paths found for benchmark." << endl;
        waitForEnter();
        return;
    }

    cout << "  Results (" << validTests << " valid tests)\n" << endl;
    cout << "  " << left << setw(30) << "Metric"
                         << setw(15) << "Dijkstra"
                         << setw(15) << "Bellman-Ford" << endl;
    cout << "  " << string(60, '-') << endl;
    cout << "  " << left << setw(30) << "Avg Runtime (ms)"
         << setw(15) << (double)totalDijkstraTime  / validTests
         << setw(15) << (double)totalBellmanTime   / validTests << endl;
    cout << "  " << left << setw(30) << "Avg Nodes Visited"
         << setw(15) << totalDijkstraNodes / validTests
         << setw(15) << totalBellmanNodes  / validTests << endl;

    string faster = totalDijkstraTime <= totalBellmanTime ? "Dijkstra" : "Bellman-Ford";
    cout << "\n  ✓ " << faster << " was faster on average." << endl;

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
        cout << "  4. Run 5(Dijkstra vs Bellman-Ford)" << endl;
        cout << "  5. Exit\n"                                   << endl;

        choice = getInput("  Select option (1-5): ");

        if      (choice == "1") findPath();
        else if (choice == "2") showInteractions();
        else if (choice == "3") searchDrug();
        else if (choice == "4") runBenchmark();
        else if (choice == "5") {
            cout << "\nGoodbye!\n" << endl;
            break;
        } else {
            cout << "\n  Invalid option. Please enter 1-5." << endl;
            waitForEnter();
        }
    }
}