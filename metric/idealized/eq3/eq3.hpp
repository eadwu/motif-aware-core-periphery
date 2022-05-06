#ifndef __IDEALIZED_EQ3_H_
#define __IDEALIZED_EQ3_H_

#include <string>

#include "../../metric.hpp"

using namespace std;
using namespace metric;

class IdealCPEQ3 : public Metric
{
public:
    IdealCPEQ3() : Metric() { };

    const string name () const;
    void max (const MetricArgs &args = Arguments) const;
    void calculate (const string &edgeListFile, const NodeLookup &cores, const MetricArgs &extra_args = Arguments) const;
};

#endif // __IDEALIZED_EQ3_H_
