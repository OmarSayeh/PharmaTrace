#pragma once
#include "graph.h"
#include <string>

class Menu {
public:
    Graph& graph;
    Menu(Graph& g) : graph(g) {}
    // Main entry point
    void run();

private:
    // Menu options
    void findPath();
    void showInteractions();
    void searchDrug();
    void runBenchmark();
    void printRiskAdvice(double riskScore);
    // Helpers
    string getInput(const string& prompt);
    string normalize(const string& input);
    void printHeader();
    void waitForEnter();
};