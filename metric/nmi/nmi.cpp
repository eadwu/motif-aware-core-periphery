#include <chrono>

#include <cmath>
#include <queue>
#include <random>
#include <forward_list>
#include <unordered_map>

#include <fstream>
#include <istream>

#include "./nmi.hpp"

using namespace std;

// p_CC >= p_CP > p_PP
// p_CC is the probability of a connection between core nodes
const double p_CC = 0.8;
// p_CP is the probability of a connection between core/periphery
const double p_CP = 0.6 * p_CC; // 3 / 5 = 0.6
// p_PP is the probability of a connection between periphery nodes
const double p_PP = 0.05;

const size_t N_RUNS = 10;

static default_random_engine rng;
static uniform_real_distribution<double> distribution (0.0, 1.0);

const string
NMI::name () const
{
    return "Normalized Mutual Information";
}

void
NMI::max (const MetricArgs &args) const
{
    // Range of possible values are [0, 1]
    metric::Summary.maxScore = 1;
}

void
calculateOneShot (
    const string &edgeListFile, const NodeLookup &cores,
    const MetricArgs &extra_args
)
{
    const auto tick = chrono::steady_clock::now ();

    const size_t nNodes = extra_args.nNodes;
    const size_t nCores = extra_args.nCores;
    const size_t nPeriphery = nNodes - nCores;

    // Only two partitions in the C/P structure, one partition contains
    // the core nodes and the other contains the periphery nodes
    // Let 0 and 1 denote the core and periphery partitions respectively
    const int CORE_PARTITION = 0;
    const int PERIPHERY_PARTITION = 1;

    // Construct Synthetic Graph
    ifstream stream{ edgeListFile };
    if (!stream)
        fprintf (stderr, "Failed to open `%s` for reading\n", edgeListFile.c_str ());

    NodeID u, v;
    unordered_map<NodeID, forward_list<NodeID>> syntheticGraph;
    while (stream >> u >> v)
    {
        const bool isUCore = isCoreNode (cores, u);
        const bool isVCore = isCoreNode (cores, v);

        // Only way for a core node to become a periphery node is
        // for it to become disconnected
        if (!isUCore || !isVCore)
            continue;

        // Keep core-to-core edge if it is lucky (<= p_CC)
        // Equivalently a 1-p_CC chance of core-to-core edges being deleted
        const double p = distribution (rng);
        if (p <= p_CC)
        {
            if (syntheticGraph.find (u) == syntheticGraph.end ())
                syntheticGraph[u] = forward_list<NodeID> ();

            if (syntheticGraph.find (v) == syntheticGraph.end ())
                syntheticGraph[v] = forward_list<NodeID> ();

            syntheticGraph[u].push_front (v);
            syntheticGraph[v].push_front (u);
        }
    }

    // Determine largest connected component size
    size_t maxCoreSize = 0;

    // vector<bool> is specialized as a bitset
    vector<bool> visited (nNodes+1);
    for (auto it = cores.begin (); it != cores.end (); ++it)
    {
        const NodeID coreNode = it->first;

        // Part of previous connected component
        if (visited[coreNode])
            continue;

        queue<NodeID> unexplored;
        unexplored.push (coreNode);
        visited[coreNode] = true;

        size_t componentSize = 0;
        while (!unexplored.empty ())
        {
            queue<NodeID> frontier;

            while (!unexplored.empty ())
            {
                const NodeID node = unexplored.front ();
                unexplored.pop ();
                componentSize++;

                // No neighbors in the synthetic graph
                if (syntheticGraph.find (node) == syntheticGraph.end ())
                    continue;

                const forward_list<NodeID> neighbors = syntheticGraph.at (node);

                for (auto iter = neighbors.begin (); iter != neighbors.end ();
                    ++iter)
                {
                    const NodeID neighbor = *iter;

                    // Already explored
                    if (visited[neighbor])
                        continue;

                    visited[neighbor] = true;
                    frontier.push (neighbor);
                }
            }

            unexplored = frontier;
        }

        // Update maxCoreSize
        maxCoreSize = componentSize > maxCoreSize
            ? componentSize
            : maxCoreSize;
    }

    // C is the confusion matrix whose element C[i][j] is the number of
    // nodes of community i of the partition A that are also in the
    // community j of the partition B.
    // C[0][0] = core nodes shared among the the two cores
    // C[0][1] = core nodes in periphery of other partition
    // C[1][0] = periphery nodes in the core of the other partition (not possible)
    // C[1][1] = periphery nodes shared among the two peripheries
    const size_t syntheticPeripheryNodes = nNodes - maxCoreSize;
    const size_t coreDifference = nCores > maxCoreSize
        ? nCores - maxCoreSize : maxCoreSize - nCores;

    size_t C[2][2] = {
        {min (nCores, maxCoreSize), coreDifference},
        {0, min (nPeriphery, syntheticPeripheryNodes)},
    };

    const size_t rowSum[2] = { C[0][0] + C[0][1], C[1][0] + C[1][1] };
    const size_t colSum[2] = { C[0][0] + C[1][0], C[0][1] + C[1][1] };

    double mutualInformation = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            double nodesInPartition = (double)C[i][j];
            double logComponent = (nodesInPartition * nNodes) / (rowSum[i] * colSum[j]);

            mutualInformation += -2 * nodesInPartition * (
                logComponent - 0 <= 1e-8 ? 0 : log (logComponent)
            );
        }
    }

    double entropyA = 0;
    double entropyB = 0;
    for (int i = 0; i < 2; i++)
    {
        double rowISum = (double)rowSum[i];
        double colISum = (double)colSum[i];

        double normalizedRowSum = rowISum / nNodes;
        double normalizedColSum = colISum / nNodes;

        entropyA += rowISum * (normalizedRowSum - 0 <= 1e-8 ? 0 : log (normalizedRowSum));
        entropyB += colISum * (normalizedColSum - 0 <= 1e-8 ? 0 : log (normalizedColSum));
    }

    double nmi = mutualInformation / (entropyA + entropyB);

    // print_time (
    //     "Metric Calculation Time:", chrono::steady_clock::now () - tick
    // );

    metric::Summary.score = nmi;
}

void
NMI::calculate (
    const string &edgeListFile, const NodeLookup &cores,
    const MetricArgs &extra_args
) const
{
    // double nmi = 0;
    // for (auto i = 0; i < N_RUNS; i++)
    // {
    //     calculateOneShot (edgeListFile, cores, extra_args);
    //     nmi = nmi + metric::Summary.score;
    // }

    // metric::Summary.score = nmi / N_RUNS;
    calculateOneShot (edgeListFile, cores, extra_args);
}
