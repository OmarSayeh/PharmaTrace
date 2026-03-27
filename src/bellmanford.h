#pragma once
#include "graph.h"
#include "results.h"
#include <string>
using namespace std;

AlgorithmResult bellmanFord(const Graph& graph,
                            const string& start,
                            const string& target);