#ifndef __NMI_H_
#define __NMI_H_

#include <string>

#include "../metric.hpp"

using namespace std;
using namespace metric;

class NMI : public Metric
{
public:
    NMI() : Metric() { };

    const string name () const;
    void max (const MetricArgs &args = Arguments) const;
    void calculate (const string &edgeListFile, const NodeLookup &cores, const MetricArgs &extra_args = Arguments) const;
};

#endif // __NMI_H_
