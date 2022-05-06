#ifndef __IDEALIZED_EQ2_H_
#define __IDEALIZED_EQ2_H_

#include <string>

#include "../../metric.hpp"

using namespace std;
using namespace metric;

class IdealCPEQ2 : public Metric
{
public:
    IdealCPEQ2() : Metric() { };

    const string name () const;
    void max (const MetricArgs &args = Arguments) const;
    void calculate (const string &edgeListFile, const NodeLookup &cores, const MetricArgs &extra_args = Arguments) const;
};

#endif // __IDEALIZED_EQ2_H_
