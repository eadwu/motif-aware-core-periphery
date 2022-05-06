#include <stdio.h> // tmpfile

#include <fstream> // fstream
#include <istream> // ifstream
#include <ostream> // ofstream
#include <iostream> // stderr

#include <string>
#include <sstream> // stringstream

#include <set>

#include <graphviz/gvc.h>

#define CORE_NODE_COLOR "#78909C"
#define SUBGRAPH_BORDER_COLOR "#FF00FF"

using namespace std;

#include "./graphviz.cpp"

Agraph_t *
dotizeEdgeList (
    Agraph_t *g, const string graphFilePath,
    set<string> coreNodes
)
{
    cout << "Converting from edge list..." << endl;

    // Input stream containing edge list contents
    ifstream stream{ graphFilePath };
    if (!stream)
        fprintf (stderr, "Failed to open `%s` for reading\n", graphFilePath.c_str ());

    // Read until EOF
    while (stream)
    {
        // Fetch current edge, a -> b
        string a, b;
        stream >> a >> b;

        // Skip trailing inputs
        if (a.length () < 1 || b.length () < 1)
            continue;

        const bool isUCore = coreNodes.find (a) != coreNodes.end ();
        const bool isVCore = coreNodes.find (b) != coreNodes.end ();

        // if ((isUCore && isVCore) || (!isUCore && !isVCore))
        //     continue;

        // Create nodes in the event they do not exist
        Agnode_t *u = agnode (g, &a[0], true);
        Agnode_t *v = agnode (g, &b[0], true);

        const string uColor = isUCore ? CORE_NODE_COLOR : "black";
        const string vColor = isVCore ? CORE_NODE_COLOR : "black";

        _agsafeset (u, "style", "filled", "");
        _agsafeset (u, "color", uColor, "");
        _agsafeset (u, "shape", "hexagon", "ellipse");

        _agsafeset (v, "style", "filled", "");
        _agsafeset (v, "color", vColor, "");
        _agsafeset (v, "shape", "hexagon", "ellipse");

        // Agraph_t *h = agsubg (g, "clusterCORE", true);
        // _agsafeset (h, "color", SUBGRAPH_BORDER_COLOR, "black");

        // if (isUCore)
        //     agsubnode (h, u, true);

        // if (isVCore)
        //     agsubnode (h, v, true);

        // Create edge
        ostringstream edgeName;
        edgeName << "E" << a << "_" << b;
        agedge (g, u, v, &edgeName.str ()[0], true);
    }

    fprintf (
        stdout, "Parsed edge list, found `%d` vertices and `%d` edges\n",
        agnnodes (g), agnedges (g)
    );
    return g;
}

set<string>
defineSubgraphs (
    Agraph_t *g, const string hierarchyFilePath,
    const string targetSubgraph
)
{
    set<string> coreNodes;

    cout << "Defining subgraphs..." << endl;

    // Input stream containing hierarchy from quark decomposition
    ifstream stream{ hierarchyFilePath };
    if (!stream)
        fprintf (stderr, "Failed to open `%s` for reading\n", hierarchyFilePath.c_str ());

    // Read until EOF
    while (stream)
    {
        // First 6 numbers are:
        // subgraph-id, K value, |V|, |E|, edge density,
        //   and 1 (0) if the quark is (not) a leaf in the hierarchy
        // Next number is the parent quark [id]
        // Followed by the subgraph of node ids
        // Each line is closed with a -1.
        string subgraphID, quarkNumber, vertices, edges, edgeDensity, isLeaf;
        int parentSubgraph;

        stream >> subgraphID >> quarkNumber >> vertices >> edges;
        stream >> edgeDensity >> isLeaf >> parentSubgraph;

        string node;
        for (stream >> node; stream && node != "-1"; stream >> node)
        {
            // Query the existing node in the graph
            Agnode_t *n = agnode (g, &node[0], true);

            // Mark nodes in target cluster
            if (quarkNumber == targetSubgraph)
            {
                // Agnode_t *n = agnode (g, &node[0], true);

                // Create subgraph [cluster] instance
                ostringstream subgraphName;
                subgraphName << "cluster" << quarkNumber;
                Agraph_t *h = agsubg (g, &subgraphName.str ()[0], true);
                _agsafeset (h, "color", SUBGRAPH_BORDER_COLOR, "black");

                // Add the node to the subgraph
                agsubnode (h, n, true);
                coreNodes.insert (node);

                // Adjust node style
                _agsafeset (n, "root", "true", "false");
            }
        }
    }

    return coreNodes;
}

int
main (int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf (stderr, "usage: %s <graph> <hierarchy> <subgraph>\n", argv[0]);
        exit (1);
    }

    const string graphFilePath (argv[1]);
    const string hierarchyFilePath (argv[2]);
    const string subgraphID (argv[3]);

    // Graphviz context
    GVC_t *gvc = gvContext ();
    // Create top-level directed graph
    Agraph_t *g = agopen ((char * const)"G", Agdirected, NULL);

    // Convert graph to DOT syntax, or graphviz's representation
    set<string> coreNodes = defineSubgraphs (g, hierarchyFilePath, subgraphID);
    dotizeEdgeList (g, graphFilePath, coreNodes);

    // Graph attributes
    // Determines if and how node overlaps should be removed.
    _agsafeset (g, "overlap", "scale", "false");
    // Controls how, and if, edges are represented.
    _agsafeset (g, "splines", "true", "false");
    // _agsafeset (g, "splines", "polyline", "false");
    // _agsafeset (g, "splines", "ortho", "false");

    // Lay out directed graph
    cout << "Generating layout..." << endl;
    // gvLayout (gvc, g, "dot");
    // gvLayout (gvc, g, "neato");

    gvLayout (gvc, g, "fdp");
    // gvLayout (gvc, g, "sfdp");

    // gvLayout (gvc, g, "twopi");
    // gvLayout (gvc, g, "circo");

    // Save to file
    cout << "Saving to output..." << endl;
    gvRenderFilename (gvc, g, "png", "graph.png");

    // Free up context
    gvFreeLayout (gvc, g);
    agclose (g);
    return gvFreeContext (gvc);
}
