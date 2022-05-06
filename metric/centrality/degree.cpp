#include <chrono>

#include <unordered_map>

#include <fstream>
#include <istream>

#include "./degree.hpp"

using namespace std;

const string
DegreeCentrality::name () const
{
    return "Degree Centrality";
}

void
DegreeCentrality::max (const MetricArgs &args) const
{
    const auto nNodes = args.nNodes;
    const auto nCores = args.nCores;

    // The maximum in-degree of the subgraph is the number of nodes
    //   not part of the subgraph (excluding internal connections)
    metric::Summary.maxScore = nNodes - nCores;
}

void
DegreeCentrality::calculate (
    const string &edgeListFile, const NodeLookup &cores,
    const MetricArgs &extra_args
) const
{
    const auto tick = chrono::steady_clock::now ();

    unordered_map<NodeID, bool> cpInNodes;
    unordered_map<NodeID, uint64_t> inDegree;

    ifstream stream{ edgeListFile };
    if (!stream)
        fprintf (stderr, "Failed to open `%s` for reading\n", edgeListFile.c_str ());

    NodeID u, v;
    while (stream >> u >> v)
    {
        const bool isUCore = isCoreNode (cores, u);
        const bool isVCore = isCoreNode (cores, v);

        // Directed edge from u to v
        // Would increase the in-degree of v
        if (!isUCore && !isVCore)
        {
            if (inDegree.find (v) == inDegree.end ())
                inDegree[v] = 0;

            inDegree[v]++;
        }

        // Edge u -> v, so u is the in-neighbor if v is a core node
        //      Don't care if u and v are both cores, since they are in
        //      the same subgraph
        if (!isUCore && isVCore)
            cpInNodes[u] = true;
    }

    size_t in_neighbors = cpInNodes.size ();
    // print_time (
    //     "Metric Calculation Time:", chrono::steady_clock::now () - tick
    // );

    metric::Summary.score = in_neighbors;
}
