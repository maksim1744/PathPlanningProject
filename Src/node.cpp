#include "node.h"

Node::Node(std::pair<int, int> coords) :
    i(coords.first),
    j(coords.second) {}

std::pair<int, int> Node::position() const {
    return {i, j};
}
