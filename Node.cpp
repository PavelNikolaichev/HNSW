#include "Node.h"

Node::Node(size_t id, const std::vector<float> &data, int maxLevel) : id(id), data(data), neighbors(maxLevel + 1) {}
