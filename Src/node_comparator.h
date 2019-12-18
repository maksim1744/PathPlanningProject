#ifndef NODE_COMPARATOR_H
#define NODE_COMPARATOR_H

#include "node.h"

#include <cmath>

bool NodeGminComparator(const Node &first, const Node &second);
bool NodeGmaxComparator(const Node &first, const Node &second);

struct NodePosComparator {
    bool operator()(const Node &first, const Node &second) const;
};

#endif
