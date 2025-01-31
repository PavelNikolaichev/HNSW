#include "Graph.h"
#include <cmath>
#include <random>
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_set>

static float distance(const std::vector<float>& a, const std::vector<float>& b);

Graph::Graph(int M, int efConstruction, double mL)
    : M(M), efConstruction(efConstruction), mL(mL),
      entryPoint(nullptr), currentMaxLevel(0) {}

void Graph::insert(const std::vector<float>& point) {
    const int level = getRandomLevel();
    Node* newNode = new Node(nodes.size(), point, level);
    nodes.emplace_back(newNode);

    if (level > currentMaxLevel) {
        currentMaxLevel = level;
        entryPoint = newNode;
    }

    if (!entryPoint) {
        entryPoint = newNode;
        return;
    }

    Node* curr = entryPoint;
    for (int l = currentMaxLevel; l > level; --l) {
        auto candidates = searchLayer(curr, point, 1, l);
        if (!candidates.empty()) curr = candidates[0];
    }

    for (int l = std::min(level, currentMaxLevel); l >= 0; --l) {
        auto candidates = searchLayer(curr, point, efConstruction, l);

        // Maintain that the number of connections is at most M
        createConnections(newNode, candidates, l);
        pruneConnections(newNode, l);
    }
}

std::vector<Node *> Graph::searchLayer(Node *start, const std::vector<float> &point,
                                       int ef, int layer) {
    using HeapElement = std::pair<float, Node*>;
    auto cmp = [](const HeapElement& a, const HeapElement& b) {
        return a.first > b.first;
    };

    std::priority_queue<HeapElement, std::vector<HeapElement>, decltype(cmp)> candidates(cmp);
    std::vector<HeapElement> nearest;
    std::unordered_set<Node*> visited;

    float startDist = distance(start->data, point);
    candidates.emplace(startDist, start);
    visited.insert(start);

    while (!candidates.empty()) {
        auto [dist, node] = candidates.top();
        candidates.pop();

        nearest.emplace_back(dist, node);

        for (Node* neighbor : node->neighbors[layer]) {
            if (visited.count(neighbor)) {
                continue;
            }

            visited.insert(neighbor);

            float d = distance(neighbor->data, point);
            if (candidates.size() < ef) {
                candidates.emplace(d, neighbor);
            } else {
                if (d < candidates.top().first) {
                    candidates.pop();
                    candidates.emplace(d, neighbor);
                }
            }
        }
    }

    std::sort(nearest.begin(), nearest.end(),
              [](const HeapElement& a, const HeapElement& b) {
                  return a.first < b.first;
              });

    std::vector<Node*> results;
    for (size_t i = 0; i < (size_t)ef && i < nearest.size(); ++i) {
        results.push_back(nearest[i].second);
    }

    return results;
}

std::vector<Node*> Graph::search(const std::vector<float>& query, int k, int efSearch) {
    std::vector<Node*> results;
    if (!entryPoint) {
        return results;
    }

    Node* curr = entryPoint;
    for (int l = currentMaxLevel; l > 0; --l) {
        auto candidates = searchLayer(curr, query, 1, l);
        if (!candidates.empty()) {
            curr = candidates[0];
        }
    }

    auto candidates = searchLayer(curr, query, efSearch, 0);
    std::sort(candidates.begin(), candidates.end(),
             [&](Node* a, Node* b) {
                 return distance(a->data, query) < distance(b->data, query);
             });

    if (k > candidates.size()) {
        k = candidates.size();
    }

    results.assign(candidates.begin(), candidates.begin() + k);

    return results;
}

void Graph::createConnections(Node* node, const std::vector<Node*>& targets, int level) {
    for (Node* target : targets) {
        node->neighbors[level].push_back(target);
        target->neighbors[level].push_back(node);
    }
}

int Graph::getRandomLevel() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<> dis(0.0, 1.0);

    return static_cast<int>(-log(dis(gen)) * mL);
}

void Graph::createConnections(Node* node, Node* target, int level) {
    node->neighbors[level].push_back(target);
    target->neighbors[level].push_back(node);
}

void Graph::pruneConnections(Node* node, int level) {
    auto& neighbors = node->neighbors[level];
    if (neighbors.size() <= M) {
        return;
    }

    std::sort(neighbors.begin(), neighbors.end(),
             [&](Node* a, Node* b) {
                 return distance(a->data, node->data) <
                        distance(b->data, node->data);
             });

    neighbors.resize(M);
}

float distance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}