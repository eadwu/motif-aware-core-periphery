#include <fstream>
#include <istream>

#include <string>
#include <sstream>

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
        fprintf (stdout, "usage: %s <edgelist>", argv[0]);
        return 0;
    }

    const string edgeListFile (argv[1]);

    ifstream stream{ edgeListFile };
    if (!stream)
    {
        fprintf (stderr, "Failed to open edge list at `%s`\n", edgeListFile.c_str ());
        return 1;
    }

    string u, v;
    string currentLine;
    while (getline (stream, currentLine))
    {
        stringstream line;
        line << currentLine;

        // Ignore comments
        if (currentLine[0] == '#')
            continue;

        // Ignore any labelled data on edges
        line >> u >> v;
        fprintf (stdout, "%s %s\n", u.c_str (), v.c_str ());
    }

    // print_time (
    //     "Edge List Postprocessing Time:", chrono::steady_clock::now () - tick
    // );
    return 0;
}
