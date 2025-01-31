#ifndef NODE_H
#define NODE_H

#include <vector>

class Node {
public:
    size_t id;
    std::vector<float> data;
    std::vector<std::vector<Node*>> neighbors;

    Node(size_t id, const std::vector<float>& data, int maxLevel);
};

#endif