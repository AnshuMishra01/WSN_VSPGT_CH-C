#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
#include <cmath>
#include <map>
#include <set>

using namespace std;

const double k1 = 0.5;
const double k2 = 0.5;
const double desiredIterations = 3;
const double expectedIterationLength = 10; // seconds

struct Node {
    int id;
    double energy;
    int num_neighbors;
    double distance_to_sink;
    int cluster_id;
    string state;
    double time;
    int numLoyalFollowers;
    int bestFollowerCount;
    int bestLeader;

    Node(int id, double energy, int num_neighbors, double distance_to_sink) 
        : id(id), energy(energy), num_neighbors(num_neighbors), distance_to_sink(distance_to_sink), cluster_id(-1), state("Unclustered"), time(0), numLoyalFollowers(0), bestFollowerCount(0), bestLeader(id) {}
};

bool comparePotential(const Node &a, const Node &b) {
    if (a.energy != b.energy)
        return a.energy > b.energy;
    if (a.num_neighbors != b.num_neighbors)
        return a.num_neighbors > b.num_neighbors;
    return a.distance_to_sink < b.distance_to_sink;
}

vector<Node> selectClusterHeads(vector<Node> &nodes, double currentTime, int maxClusterHeads) {
    vector<Node> clusterHeads;
    for (auto &node : nodes) {
        if (node.state == "Unclustered") {
            node.numLoyalFollowers = count_if(nodes.begin(), nodes.end(), [&node](const Node &n) {
                return (n.state == "Unclustered" && n.id != node.id);
            });

            double fmin = (exp(-k1 * (currentTime / (desiredIterations * expectedIterationLength)) - k2)) * node.num_neighbors;
            if (node.numLoyalFollowers >= fmin) {
                node.cluster_id = node.id; // Use node's ID as cluster ID
                node.state = "ClusterHead";
                clusterHeads.push_back(node);
                if (clusterHeads.size() >= maxClusterHeads) {
                    break;
                }
            }
        }
    }
    return clusterHeads;
}

void updateFollowers(vector<Node> &nodes) {
    for (auto &node : nodes) {
        if (node.state == "Unclustered") {
            Node *bestCH = nullptr;
            for (auto &ch : nodes) {
                if (ch.state == "ClusterHead" && (bestCH == nullptr || ch.distance_to_sink < bestCH->distance_to_sink)) {
                    bestCH = &ch;
                }
            }
            if (bestCH) {
                node.cluster_id = bestCH->cluster_id;
                node.state = "Follower";
            }
        }
    }
}

void printClusters(const vector<Node> &nodes, ofstream &outfile) {
    map<int, set<int>> clusters;

    for (const auto &node : nodes) {
        if (node.state == "Follower" || node.state == "ClusterHead") {
            clusters[node.cluster_id].insert(node.id);
        }
    }

    for (const auto &cluster : clusters) {
        outfile << "ClusterHead," << cluster.first << endl;
        for (const auto &node_id : cluster.second) {
            if (node_id != cluster.first) {
                outfile << "Follower," << node_id << "," << cluster.first << endl;
            }
        }
    }
    outfile << endl;
}

int main() {
    srand(time(0)); // Seed for random number generation

    vector<Node> nodes;
    for (int i = 1; i <= 100; ++i) {
        double energy = (rand() % 100) + 1; // Random energy between 1 and 100
        int num_neighbors = (rand() % 20) + 1; // Random number of neighbors between 1 and 20
        double distance_to_sink = (rand() % 200) + 1; // Random distance to sink between 1 and 200
        nodes.emplace_back(i, energy, num_neighbors, distance_to_sink);
    }

    int maxClusterHeads = nodes.size() / 20; // Desired number of cluster heads

    ofstream outfile("clusters.csv");
    if (!outfile) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    auto start = chrono::high_resolution_clock::now();
    int iterations = 0;

    while (chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start).count() < 70) {
        iterations++;
        cout << "Iteration " << iterations << ":" << endl;

        // Time elapsed since the start of the algorithm
        auto currentTime = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start).count();

        vector<Node> clusterHeads = selectClusterHeads(nodes, currentTime, maxClusterHeads);

        // Update followers
        updateFollowers(nodes);

        printClusters(nodes, outfile);

        // Simulate 10 seconds delay
        auto iteration_start = chrono::high_resolution_clock::now();
        while (chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - iteration_start).count() < 10) {
            // Busy wait for 10 seconds
        }
    }

    outfile.close();
    return 0;
}
