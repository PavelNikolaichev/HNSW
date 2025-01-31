#include <iostream>
#include "Graph.h"
#include <chrono>
#include <random>
#include <unordered_set>

void benchmarkInsertion(Graph& graph, const std::vector<std::vector<float>>& dataset) {
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& point : dataset) {
        graph.insert(point);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Insertion time for " << dataset.size() << " points: " << duration << " ms" << std::endl;
}

void benchmarkSearch(Graph& graph, const std::vector<std::vector<float>>& queries, int k, int efSearch) {
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& query : queries) {
        auto results = graph.search(query, k, efSearch);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Average search time per query: " << duration / queries.size() << " ms" << std::endl;
}

float calculateRecall(const std::vector<Node*>& hnswResults, const std::vector<std::vector<float>>& groundTruth) {
    int correct = 0;
    for (const auto& gtId : groundTruth) {
        // Assume groundTruth contains true nearest neighbors.
        for (const auto& result : hnswResults) {
            if (result->data == gtId) {
                correct++;
                break;
            }
        }
    }

    return static_cast<float>(correct) / groundTruth.size();
}


std::vector<std::vector<float>> generateSyntheticData(int numPoints, int dim) {
    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<float> dist(-100.0, 100.0);

    std::vector<std::vector<float>> data(numPoints);
    for (auto& vec : data) {
        vec.resize(dim);
        for (int i = 0; i < dim; i++) {
            vec[i] = dist(gen);
        }
    }
    return data;
}

float distance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

int main() {
    int dim = 10;
    int numPoints = 2000;

    std::cout << "Generating synthetic dataset..." << std::endl;
    auto dataset = generateSyntheticData(numPoints, dim);

    Graph graph(16, 200, 0.5);

    std::cout << "Benchmarking insertion..." << std::endl;
    std::cout << "Parameters: M=16, efConstruction=200, mL=0.5" << std::endl;

    benchmarkInsertion(graph, dataset);

    // Generate tests
    std::cout << "Generating synthetic queries..." << std::endl;
    auto queries = generateSyntheticData(100, dim);

    std::cout << "Benchmarking search..." << std::endl;
    benchmarkSearch(graph, queries, 10, 200);

    // find ground truth using brute force
    std::cout << "Finding ground truth using bruteforce..." << std::endl;
    std::vector<std::vector<std::vector<float>>> groundTruth;
    for (const auto& query : queries) {
        std::vector<std::pair<float, std::vector<float>>> distances;
        for (const auto& point : dataset) {
            distances.emplace_back(distance(query, point), point);
        }

        std::sort(distances.begin(), distances.end());
        std::vector<std::vector<float>> nearest;
        for (int i = 0; i < 10; i++) {
            nearest.push_back(distances[i].second);
        }
        groundTruth.push_back(nearest);
    }

    std::cout << "Calculating recall..." << std::endl;
    float recall = 0;
    for (size_t i = 0; i < queries.size(); i++) {
        auto results = graph.search(queries[i], 10, 4000);
        recall += calculateRecall(results, groundTruth[i]);
    }

    recall /= queries.size();

    std::cout << "Recall (avg): " << recall << std::endl;

    return 0;
}