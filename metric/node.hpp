#ifndef __NODE_H_
#define __NODE_H_

#include <vector>

using namespace std;

class Node
{
private:
    int parent;
    vector<int> immediateSubgraphs;

public:
    int K;

    Node () {};

    void setParent (const int subgraph);
    void addChild (const int subgraph);

    const int getParent () const;
    const vector<int> getChildren () const;
};

#endif // __NODE_H_
