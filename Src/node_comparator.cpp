#include "node_comparator.h"

bool NodeGminComparator(const Node &first, const Node &second) {
    if (first.f != second.f)
        return first.f < second.f;
    if (first.g != second.g)
        return first.g < second.g;
    if (first.i != second.i)
        return first.i < second.i;
    return first.j < second.j;
}

bool NodeGmaxComparator(const Node &first, const Node &second) {
    if (first.f != second.f)
        return first.f < second.f;
    if (first.g != second.g)
        return first.g > second.g;
    if (first.i != second.i)
        return first.i < second.i;
    return first.j < second.j;
}

bool NodePosComparator::operator()(const Node &first, const Node &second) const {
    if (first.i != second.i)
        return first.i < second.i;
    if (first.j != second.j)
        return first.j < second.j;
    return false;
}
