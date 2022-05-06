#include "node.hpp"

void
Node::setParent (const int subgraph)
{
    this->parent = subgraph;
}

void
Node::addChild (const int subgraph)
{
    this->immediateSubgraphs.push_back (subgraph);
}

const int
Node::getParent () const
{
    return this->parent;
}

const vector<int>
Node::getChildren () const
{
    return this->immediateSubgraphs;
}

