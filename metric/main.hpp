#ifndef __MAIN_H_
#define __MAIN_H_

#include <string>
#include <vector>
#include <unordered_map>

#include <chrono>
#include <stdio.h>

#include "node.hpp"

using namespace std;

typedef int64_t NodeID;
typedef int64_t SubgraphID;
typedef uint64_t QuarkNumber;

typedef vector<vector<bool>> AdjacencyMatrix;
typedef unordered_map<SubgraphID, Node> QuarkHierarchy;
typedef unordered_map<SubgraphID, bool> SubgraphLookup;
typedef unordered_map<SubgraphID, int64_t> SubgraphDepth;
typedef unordered_map<NodeID, bool> NodeLookup;

typedef chrono::duration<double> timestamp;

inline void
print_time (const string &str, timestamp t)
{
    fprintf (stdout, "%s %.6lf\n", str.c_str (), t.count ());
    fflush (stdout);
}

#endif // __MAIN_H_
