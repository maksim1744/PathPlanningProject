#ifndef NODE_COMPARATOR_H
#define NODE_COMPARATOR_H
#include "node.h"

class NodeGComparator
{
    public:
        bool operator()(const Node &first, const Node &second) const;
};


class NodePosComparator
{
    public:
        bool operator()(const Node &first, const Node &second) const;
};

#endif
