#ifndef GRAPH_H
#define GRAPH_H

#include "Node.h"
#include <vector>
#include <memory>

class Graph {
public:
    Graph(int M, int efConstruction, double mL = 0.62);

    void insert(const std::vector<float>& point);
    std::vector<Node*> search(const std::vector<float>& query, int k, int efSearch);

    void createConnections(Node *node, const std::vector<Node *> &targets, int level);

private:
    std::vector<Node *> searchLayer(Node *entryPoint, const std::vector<float> &target, int ef, int layer);
    int getRandomLevel();

    void createConnections(Node *node, Node *target, int level);

    void pruneConnections(Node *node, int level);

    // Hyperparameters
    int M;              // Max connections per layer
    int efConstruction; // Construction search scope
    double mL;          // Level generation multiplier (ln(1/M))

    // Graph
    std::vector<std::unique_ptr<Node>> nodes;
    Node* entryPoint;    // Top-layer entry node
    int currentMaxLevel; // Dynamic max level (not fixed to maintain M)
};

#endif //GRAPH_H