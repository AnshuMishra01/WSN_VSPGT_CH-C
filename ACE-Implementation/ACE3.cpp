#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>

using namespace std;

struct Node {
    int id;
    double energy;
    int num_neighbors;
    double distance_to_sink;
    int cluster_id;

    Node(int id, double energy, int num_neighbors, double distance_to_sink, int cluster_id = -1) 
        : id(id), energy(energy), num_neighbors(num_neighbors), distance_to_sink(distance_to_sink), cluster_id(cluster_id) {}
};

bool comparePotential(const Node &a, const Node &b) {
    if (a.energy != b.energy)
        return a.energy > b.energy;
    if (a.num_neighbors != b.num_neighbors)
        return a.num_neighbors > b.num_neighbors;
    return a.distance_to_sink < b.distance_to_sink;
}

vector<Node> selectClusterHeads(vector<Node> &nodes) {
    sort(nodes.begin(), nodes.end(), comparePotential);

    vector<Node> clusterHeads;
    for (int i = 0; i < nodes.size() / 20; ++i) {
        nodes[i].cluster_id = i;
        clusterHeads.push_back(nodes[i]);
    }
    return clusterHeads;
}

vector<Node> findFollowers(const vector<Node> &nodes, int cluster_id) {
    vector<Node> followers;
    for (const auto &node : nodes) {
        if (node.cluster_id == -1 || node.cluster_id == cluster_id) {
            followers.push_back(node);
        }
    }
    return followers;
}

void printClusters(const vector<Node> &clusterHeads, const vector<Node> &nodes, ofstream &outfile) {
    outfile << "Clusters:" << endl;
    for (const auto &clusterHead : clusterHeads) {
        outfile << "ClusterHead " << clusterHead.cluster_id << " (Node " << clusterHead.id << "):" << endl;
        outfile << "  Followers:" << endl;
        for (const auto &follower : findFollowers(nodes, clusterHead.cluster_id)) {
            outfile << "    Node " << follower.id << endl;
        }
    }
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

    ofstream outfile("clusters.csv");
    if (!outfile) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    auto start = chrono::high_resolution_clock::now();
    int iterations = 7; // Total iterations to run

    while (chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start).count() < 70) {
        cout << "New iteration:" << endl;
        vector<Node> clusterHeads = selectClusterHeads(nodes);
        printClusters(clusterHeads, nodes, outfile);

        // Simulate 10 seconds delay
        auto iteration_start = chrono::high_resolution_clock::now();
        while (chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - iteration_start).count() < 10) {
            // Busy wait for 10 seconds
        }
    }

    outfile.close();
    return 0;
}
