#include <chrono>

#include <unordered_map>

#include <fstream>
#include <istream>

#include "./modularity.hpp"

using namespace std;

const string
Modularity::name () const
{
    return "Modularity";
}

void
Modularity::max (const MetricArgs &args) const
{
    // Ratio of edges that are present
    metric::Summary.maxScore = 1.0;
}

void
Modularity::calculate (
    const string &edgeListFile, const NodeLookup &cores,
    const MetricArgs &extra_args
) const
{
    const auto tick = chrono::steady_clock::now ();

    ifstream stream{ edgeListFile };
    if (!stream)
        fprintf (stderr, "Failed to open `%s` for reading\n", edgeListFile.c_str ());

    double m = 0;
    // M[0][0] = M[0] = fraction of edges that connect core-to-core
    // M[0][1] = M[1] = fraction of edges that connect core-to-periphery
    // M[1][0] = M[2] = fraction of edges that connect periphery-to-core
    // M[1][1] = M[3] = fraction of edges that connect periphery-to-periphery
    size_t edge_ratio[4] = {0};

    unordered_map<NodeID, bool> inDegreeNodes;

    NodeID u, v;
    while (stream >> u >> v)
    {
        const bool isUCore = isCoreNode (cores, u);
        const bool isVCore = isCoreNode (cores, v);

        if (isUCore && isVCore)
            edge_ratio[0]++;
        else if (isUCore || isVCore)
        {
            edge_ratio[1]++;

            if (isUCore)
                inDegreeNodes[v] = true;
            else
                inDegreeNodes[u] = true;
        }
        else
            edge_ratio[3]++;

        m++;
    }

    const double nPeripheryNodes = extra_args.nNodes - extra_args.nCores;

    const double coreCliqueConnections = extra_args.nCores * (extra_args.nCores - 1);
    const double corePeripheryConnections = extra_args.nCores * nPeripheryNodes;
    const double peripheryCliqueConnections = nPeripheryNodes * (nPeripheryNodes - 1);

    const double coreEdgeRatio = edge_ratio[0] / m * edge_ratio[0] / coreCliqueConnections;
    const double corePeripheryRatio = edge_ratio[1] / m * edge_ratio[1] / corePeripheryConnections;
    const double peripheryEdgeRatio = edge_ratio[3] / m * edge_ratio[3] / peripheryCliqueConnections;

    const double peripheryDecay = 1 - peripheryEdgeRatio;

    const double baseScore = coreEdgeRatio*2/3 + corePeripheryRatio*1/3;
    // const double modularity = baseScore - peripheryEdgeRatio;
    const double modularity = baseScore * (peripheryDecay * peripheryDecay);

    // print_time (
    //     "Metric Calculation Time:", chrono::steady_clock::now () - tick
    // );

    metric::Summary.score = modularity;
}
