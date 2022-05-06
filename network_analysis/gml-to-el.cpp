#include <stdio.h>

extern "C"
{
#include "./third_party/readgml/network.h"
#include "./third_party/readgml/readgml.h"
}

#include <string>

#include <chrono>
#include <stdio.h>

using namespace std;

typedef chrono::duration<double> timestamp;

inline void
print_time (const string &str, timestamp t)
{
    fprintf (stdout, "%s %.6lf\n", str.c_str (), t.count ());
    fflush (stdout);
}

int
main (const int argc, const char ** const argv)
{
    const auto tick = chrono::steady_clock::now ();

    if (argc != 2)
    {
        fprintf (stdout, "usage: %s <gmlFile>", argv[0]);
        return 0;
    }

    const char * const gmlFile = argv[1];
    FILE * fp = fopen (gmlFile, "r");
    if (fp == NULL)
    {
        fprintf (stderr, "Failed to open GML file at `%s`\n", gmlFile);
        return 1;
    }

    NETWORK gmlNetwork = NETWORK{};
    if (read_network (&gmlNetwork, fp))
    {
        fprintf (stderr, "Failed to read network into NETWORK struct\n");
        return 2;
    }

    const VERTEX * vertex = gmlNetwork.vertex;
    for (size_t i = 0; i < gmlNetwork.nvertices; i++, vertex++)
    {
        const size_t vertexI = vertex->id;
        const EDGE * edge = vertex->edge;

        for (size_t j = 0; j < vertex->degree; j++, edge++)
        {
            const size_t vertexJ = edge->target;

            fprintf (stdout, "%ld %ld\n", vertexI, vertexJ);
        }
    }

    free_network (&gmlNetwork);
    fclose (fp);

    // print_time (
    //     "GML Postprocessing Time:", chrono::steady_clock::now () - tick
    // );
    return 0;
}
