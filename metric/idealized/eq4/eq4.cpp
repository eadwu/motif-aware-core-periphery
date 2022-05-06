#include <chrono>

#include <fstream>
#include <istream>

#include "./eq4.hpp"

using namespace std;

const string
IdealCPEQ4::name () const
{
    return "C/P EQ4 Approx.";
}

const double K = 0.5;

void
IdealCPEQ4::max (const MetricArgs &args) const
{
    const auto nNodes = args.nNodes;
    const auto nCores = args.nCores;

    // Connections between core nodes
    const auto cliqueConnections = nCores * (nCores - 1);
    // Connections between core and periphery nodes
    const auto coreConnections = nCores * (nNodes - nCores);

    metric::Summary.maxScore = cliqueConnections + 2 * coreConnections * K;
}

void
IdealCPEQ4::calculate (
    const string &edgeListFile, const NodeLookup &cores,
    const MetricArgs &extra_args
) const
{
    /*
    Calculation derived from `Models of core/periphery structures`
        Stephen P. Borgatti, Martin G. Everett

    Essentially tries to see if the structure is a one-to-one match to
    an idealized core/periphery structure where each core is connected
    to each other (clique) and cores and connected to peripheries.

    A particular note is that the exact specification is made for
    undirected networks, so while it can still be calculated for
    directed networks, the resulting score will most likely be quite
    low (or lower).
     */
    long rho = 0;
    const auto tick = chrono::steady_clock::now ();

    ifstream stream{ edgeListFile };
    if (!stream)
        fprintf (stderr, "Failed to open `%s` for reading\n", edgeListFile.c_str ());

    NodeID u, v;
    while (stream >> u >> v)
    {
        rho = rho + (isCoreNode (cores, u) & isCoreNode (cores, v));
        rho = rho + (isCoreNode (cores, u) | isCoreNode (cores, v)) * K;
    }

    // print_time (
    //     "Idealized Metric Calculation Time:", chrono::steady_clock::now () - tick
    // );

    metric::Summary.score = rho;
}
