#include "menu.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <iomanip>
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
    cout << "  Path:          " << drugA << " → " << drugB        << endl;
    cout << "  Risk Score:    [stub]"                              << endl;
    cout << "  Nodes Visited: [stub]"                              << endl;
    cout << "  Runtime:       [stub] ms"                           << endl;
    // ───────────────────────────────────────────────────────────────────────

    cout << "\nRunning Bellman-Ford Algorithm..." << endl;
    // ── TODO : replace stub with real Bellman-Ford call ──────────────
    // BellmanFordResult b = bellmanFord(graph, drugA, drugB);
    cout << "  Path:          " << drugA << " → " << drugB        << endl;
    cout << "  Risk Score:    [stub]"                              << endl;
    cout << "  Nodes Visited: [stub]"                              << endl;
    cout << "  Runtime:       [stub] ms"                           << endl;
    // ───────────────────────────────────────────────────────────────────────

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
    cout << "\n--- Algorithm Benchmark ---\n" << endl;
    cout << "Runs 1000 random drug pair queries and compares" << endl;
    cout << "Dijkstra vs Bellman-Ford performance.\n"         << endl;

    // ── TODO : plug in real benchmark once algorithms are done
    cout << "  [stub] Benchmark not yet implemented."         << endl;
    cout << "  Come back once both algorithms are done."      << endl;
    // ────────────────────────────────────────────────────────────────────────

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