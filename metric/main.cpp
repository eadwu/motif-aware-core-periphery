#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <deque>
#include <memory>

#include <nlohmann/json.hpp>

#include "main.hpp"
#include "metric.hpp"

#include "nmi/nmi.hpp"
#include "centrality/degree.hpp"
#include "modularity/modularity.hpp"
#include "idealized/idealized.hpp"

using namespace std;
using json = nlohmann::json;

// https://stackoverflow.com/questions/194465/how-to-parse-a-string-to-an-int-in-c/6154614
#include <climits>
enum STR2INT_ERROR { SUCCESS, OVERFLOW, UNDERFLOW, INCONVERTIBLE };
STR2INT_ERROR
str2int (int &i, char const *s, int base = 0)
{
    char *end;
    long  l;
    errno = 0;
    l = strtol(s, &end, base);
    if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
        return OVERFLOW;
    }
    if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
        return UNDERFLOW;
    }
    if (*s == '\0' || *end != '\0') {
        return INCONVERTIBLE;
    }
    i = l;
    return SUCCESS;
}

MetricSummary metric::Summary = MetricSummary{};
MetricArgs metric::Arguments = MetricArgs{};

typedef struct BasicGraphStatistics {
    long vertices;
    long edges;
} BasicGraphStatistics;

void
retrieveGraphStatistics (
    const string &graphFilePath, BasicGraphStatistics &graphStatistics
)
{
    bool hasZeroNode = false;
    const auto tick = chrono::steady_clock::now ();

    graphStatistics.vertices = 0;
    graphStatistics.edges = 0;

    ifstream stream{ graphFilePath };
    if (!stream)
        fprintf (stderr, "Failed to read graph file at `%s`\n", graphFilePath.c_str ());

    string currentLine;
    while (getline (stream, currentLine))
    {
        // Comment
        if (currentLine[0] == '#')
            continue;

        stringstream line;
        line << currentLine;

        long u, v;
        line >> u >> v;



        graphStatistics.edges++;
        graphStatistics.vertices = max (max (graphStatistics.vertices, u), v);
        hasZeroNode = hasZeroNode || u == 0 || v == 0;
    }

    graphStatistics.vertices = graphStatistics.vertices + hasZeroNode;

    print_time (
        "Basic Statistics Retrieval Time:", chrono::steady_clock::now () - tick
    );
}

const QuarkHierarchy
constructHierarchy (const string &nucleiFilePath)
{
    QuarkHierarchy hierarchy;
    const auto tick = chrono::steady_clock::now ();

    ifstream stream{ nucleiFilePath };
    if (!stream)
        fprintf (stderr, "Failed to open `%s` for reading\n", nucleiFilePath.c_str ());

    // First 6 numbers are:
    // subgraph-id, K value, |V|, |E|, edge density,
    //   and 1 (0) if the quark is (not) a leaf in the hierarchy
    // Next number is the parent quark [id]
    // Followed by the subgraph of node ids
    // Each line is closed with a -1.
    SubgraphID subgraphID;
    QuarkNumber quarkNumber;
    string vertices, edges, edgeDensity, isLeaf;
    SubgraphID parentSubgraph;

    while (stream >> subgraphID)
    {
        stream
            >> quarkNumber >> vertices >> edges >> edgeDensity >> isLeaf
            >> parentSubgraph;

        const bool isTopLevel = parentSubgraph == -1;

        // Ensure Node instance exists
        if (hierarchy.find (subgraphID) == hierarchy.end ())
            hierarchy[subgraphID] = Node ();
        if (!isTopLevel && hierarchy.find (parentSubgraph) == hierarchy.end ())
            hierarchy[parentSubgraph] = Node ();

        // Store hierarchy relationships
        hierarchy.at (subgraphID).K = quarkNumber;
        hierarchy.at (subgraphID).setParent (parentSubgraph);
        if (!isTopLevel)
            hierarchy.at (parentSubgraph).addChild (subgraphID);

        // Ignore rest of the contents
        NodeID node;
        for (stream >> node; stream && node != -1; stream >> node)
            ;
    }

    print_time (
        "Hierarchy Construction Time:", chrono::steady_clock::now () - tick
    );
    return hierarchy;
}

const SubgraphLookup
topLevelSubgraphs (const QuarkHierarchy hierarchy)
{
    SubgraphLookup subgraphs;
    const auto tick = chrono::steady_clock::now ();

    for (auto element : hierarchy)
    {
        const auto subgraphID = element.first;
        const auto subgraphNode = element.second;

        if (subgraphNode.getParent () == -1)
            subgraphs[subgraphID] = true;
    }

    print_time (
        "Root Subgraph Extraction Time:", chrono::steady_clock::now () - tick
    );
    return subgraphs;
}

const SubgraphDepth
getHierarchicalDepth (
    const QuarkHierarchy hierarchy, const SubgraphLookup topLevelGraphs,
    const bool kAsDepth = false
)
{
    int depth = 0;
    SubgraphDepth hierarchy_depth;
    const auto tick = chrono::steady_clock::now ();

    // Breadth-first search
    deque<SubgraphID> queue;
    for (const auto element : topLevelGraphs)
        queue.push_back (element.first);

    while (!queue.empty ())
    {
        auto elementsInLevel = queue.size ();
        while (elementsInLevel-- > 0)
        {
            const SubgraphID subgraphID = queue.front ();
            queue.pop_front ();

            const auto subgraphNode = hierarchy.at (subgraphID);
            hierarchy_depth[subgraphID] = kAsDepth ? subgraphNode.K : depth;

            for (const auto child : subgraphNode.getChildren ())
                queue.push_back (child);
        }

        depth++;
    }

    print_time (
        "Hierarchical Depth Computation Time:", chrono::steady_clock::now () - tick
    );
    return hierarchy_depth;
}

const SubgraphLookup
atOrBelowDepth (const SubgraphDepth hierarchicalDepth, const int threshold)
{
    SubgraphLookup subgraphs;
    const auto tick = chrono::steady_clock::now ();

    for (const auto node : hierarchicalDepth)
    {
        const auto subgraphID = node.first;
        const auto subgraphDepth = node.second;

        if (subgraphDepth >= threshold)
            subgraphs[subgraphID] = true;
    }

    print_time (
        "Subgraph Paritioning Time:", chrono::steady_clock::now () - tick
    );
    return subgraphs;
}

const NodeLookup
extractCoreNodes (const string &coreNodeLookup)
{
    NodeLookup nodes;
    const auto tick = chrono::steady_clock::now ();

    ifstream stream{ coreNodeLookup };
    if (!stream)
        fprintf (stderr, "Failed to open `%s` for reading\n", coreNodeLookup.c_str ());

    json lookupMap;
    stream >> lookupMap;
    for (auto it = lookupMap.begin (); it != lookupMap.end (); ++it)
    {
        const auto key = it.key ();
        const auto value = it.value ();

        int node;
        size_t isCoreNode;
        str2int (node, key.c_str ());
        value.get_to (isCoreNode);

        if (isCoreNode == 1 || isCoreNode == 2)
            nodes[node] = true;
    }

    print_time (
        "Core Node Identification Time:", chrono::steady_clock::now () - tick
    );
    return nodes;
}

const NodeLookup
extractCoreNodes (
    const string &nucleiFilePath, const SubgraphLookup subgraphs
)
{
    NodeLookup nodes;
    const auto tick = chrono::steady_clock::now ();

    ifstream stream{ nucleiFilePath };
    if (!stream)
        fprintf (stderr, "Failed to open `%s` for reading\n", nucleiFilePath.c_str ());

    string _empty;
    SubgraphID subgraphID, parentSubgraph;
    while (stream >> subgraphID)
    {
        for (int i = 0; i < 5; i++) stream >> _empty;
        stream >> parentSubgraph;

        const bool validSubgraph = subgraphs.find (subgraphID) != subgraphs.end ();

        NodeID node;
        for (stream >> node; stream && node != -1; stream >> node)
        {
            if (validSubgraph)
                nodes[node] = true;
        }
    }

    print_time (
        "Core Node Identification Time:", chrono::steady_clock::now () - tick
    );
    return nodes;
}

int
main (int argc, char **argv)
{
    const auto tick = chrono::steady_clock::now ();

    if (argc != 3 && argc != 4)
    {
        fprintf (stderr, "usage: %s <graph> <nuclei> [<cores>]", argv[0]);
        return 1;
    }

    const string graphFilePath (argv[1]);
    const string nucleiFilePath (argv[2]);
    const bool fixedCoreNodes = argc == 4;

    auto graphStat = BasicGraphStatistics{};
    const auto hierarchy = constructHierarchy (nucleiFilePath);
    const auto subgraphs = topLevelSubgraphs (hierarchy);
    const auto hierarchicalDepth = getHierarchicalDepth (hierarchy, subgraphs);
    retrieveGraphStatistics (graphFilePath, graphStat);

    // Determine maximum depth of the hierarchy for a cleaner output
    long maxDepth = 0;
    for (auto graph : hierarchicalDepth)
        maxDepth = max (maxDepth, graph.second);

    fprintf (stdout, "Statistics:\n");
    fprintf (stdout, "    Graph Statistics:\n");
    fprintf (stdout, "        Vertices         : %ld\n", graphStat.vertices);
    fprintf (stdout, "        Edges            : %ld\n", graphStat.edges);
    fprintf (stdout, "    Maximum Depth        : %ld\n", maxDepth);
    fprintf (stdout, "    Total Subgraphs      : %ld\n", hierarchy.size ());
    fprintf (stdout, "    Top Level Subgraphs  : %ld\n", subgraphs.size ());
    fprintf (stdout, "    Partitioned Subgraphs: %ld\n", hierarchicalDepth.size ());
    fprintf (stdout, "    CSV                  : %s,%s,%s",
             "Depth (From Top-Level Graph)", "# Subgraphs (Total Nested Graph)",
             "# Nodes in Core");

    // Depth starts at one since 0-quarks are just the entire graph
    long depth = 1;
    const shared_ptr<Metric> metrics[] = {
        shared_ptr<Metric> (new IdealCPEQ2 ()),
        shared_ptr<Metric> (new IdealCPEQ3 ()),
        shared_ptr<Metric> (new IdealCPEQ4 ()),
        shared_ptr<Metric> (new DegreeCentrality ()),
        shared_ptr<Metric> (new NMI ()),
        shared_ptr<Metric> (new Modularity ())
    };

    for (const auto &metric : metrics)
    {
        // fprintf (stdout, ",%s %s", metric->name ().c_str (), "Ratio");
        fprintf (stdout, ",%s %s,%s %s,%s %s",
                metric->name ().c_str (), "Score",
                metric->name ().c_str (), "Max Score",
                metric->name ().c_str (), "Ratio");
    }
    fprintf (stdout, "\n");

    do
    {
        const auto partitionedSubgraphs = atOrBelowDepth (hierarchicalDepth, depth);
        const auto coreNodes = fixedCoreNodes ? extractCoreNodes (argv[3])
            : extractCoreNodes (nucleiFilePath, partitionedSubgraphs);

        metric::Arguments.nNodes = graphStat.vertices;
        metric::Arguments.nCores = coreNodes.size ();

        fprintf (stdout, "    CSV : %ld,%ld,%ld",
                depth, partitionedSubgraphs.size (), coreNodes.size ());
        for (const auto &metric : metrics)
        {
            metric->max (metric::Arguments);
            metric->calculate (graphFilePath, coreNodes);

            const auto score = metric::Summary.score;
            const auto max = metric::Summary.maxScore;

            fprintf (stdout, ",%.4f,%.4f,%.8f", score, max, score / max);
            // fprintf (stdout, ",%.4f", score / max);
        }
        fprintf (stdout, "\n");

        json j_coreNodes = json::object();
        for (const auto kvPair : coreNodes)
            j_coreNodes[to_string (kvPair.first)] = kvPair.second;
        // fprintf (stdout, "    JSON : %s\n", j_coreNodes.dump ().c_str ());

        stringstream filename;
        filename << "QuarkD" << depth << ".json";
        ofstream output{ filename.str () };
        output << j_coreNodes;
        output.close ();
    } while (++depth <= maxDepth && !fixedCoreNodes);

    print_time (
        "Total Runtime:", chrono::steady_clock::now () - tick
    );
    return 0;
}
