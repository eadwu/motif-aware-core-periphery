#ifndef __METRIC_H_
#define __METRIC_H_

#include <vector>
#include <unordered_map>

#include "main.hpp"

using namespace std;

namespace metric
{
typedef struct MetricSummary {
    double score;
    double maxScore;
} MetricSummary;
constexpr MetricSummary DEFAULT_METRICSUMMARY = MetricSummary{};

typedef struct MetricArgs {
    size_t nNodes;
    size_t nCores;
} MetricArgs;
constexpr MetricArgs DEFAULT_METRICARGS = MetricArgs{};

extern MetricSummary Summary;
extern MetricArgs Arguments;

class Metric {
public:
    Metric () { }

    virtual const string name () const = 0;
    virtual void max (const MetricArgs &args = Arguments) const = 0;
    virtual void calculate (const string &edgeListFile, const NodeLookup &cores, const MetricArgs &extra_args = Arguments) const = 0;
};

namespace
{
    inline const bool
    isCoreNode (const NodeLookup &cores, const NodeID node)
    {
        return cores.find (node) != cores.end ();
    }
}
}

#endif // __METRIC_H_
