#ifndef __MODULARITY_H_
#define __MODULARITY_H_

#include <string>

#include "../metric.hpp"

using namespace std;
using namespace metric;

class Modularity : public Metric
{
public:
    Modularity() : Metric() { };

    const string name () const;
    void max (const MetricArgs &args = Arguments) const;
    void calculate (const string &edgeListFile, const NodeLookup &cores, const MetricArgs &extra_args = Arguments) const;
};

#endif // __MODULARITY_H_
