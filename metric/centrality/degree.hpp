#ifndef __DEGREE_CENTRALITY_H_
#define __DEGREE_CENTRALITY_H_

#include <string>

#include "../metric.hpp"

using namespace std;
using namespace metric;

class DegreeCentrality : public Metric
{
public:
    DegreeCentrality() : Metric() { };

    const string name () const;
    void max (const MetricArgs &args = Arguments) const;
    void calculate (const string &edgeListFile, const NodeLookup &cores, const MetricArgs &extra_args = Arguments) const;
};

#endif // __DEGREE_CENTRALITY_H_
