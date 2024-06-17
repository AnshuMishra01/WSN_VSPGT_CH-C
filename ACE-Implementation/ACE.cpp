#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

const int NUM_NODES = 100;
const double FIELD_SIZE = 100.0;
const double COMM_RANGE = 10.0;
const int MAX_ITERATIONS = 10;

struct Node {
    int id;
    double x, y;
    int clusterID;
    int bestLeader;
    std::string state;
    int loyalFollowers;

    Node(int id, double x, double y) : id(id), x(x), y(y), clusterID(-1), bestLeader(-1), state("Unclustered"), loyalFollowers(0) {}

    double distanceTo(const Node& other) const {
        return std::sqrt((x - other.x) * (x - other.y) * (x - other.x) + (y - other.y) * (y - other.y));
    }
};

double fmin(double t, int d, int I, int c, double k1 = 0.5, double k2 = 0.1) {
    return (std::exp(-k1 * (t / (double)c)) - k2) * d;
}

void initNodes(std::vector<Node>& nodes) {
    std::srand(std::time(0));
    for (int i = 0; i < NUM_NODES; ++i) {
        double x = (std::rand() / (double)RAND_MAX) * FIELD_SIZE;
        double y = (std::rand() / (double)RAND_MAX) * FIELD_SIZE;
        nodes.emplace_back(i, x, y);
    }
}

void calculateLoyalFollowers(Node& node, const std::vector<Node>& nodes) {
    int count = 0;
    for (const Node& other : nodes) {
        if (&node != &other && node.distanceTo(other) < COMM_RANGE) {
            count++;
        }
    }
    node.loyalFollowers = count;
}

void clusterFormation(std::vector<Node>& nodes) {
    for (Node& node : nodes) {
        calculateLoyalFollowers(node, nodes);
        double threshold = fmin(node.loyalFollowers, 5, 10, MAX_ITERATIONS);
        if (node.loyalFollowers >= threshold) {
            node.clusterID = node.id;
            node.state = "ClusterHead";
        }
    }
}

void migrateClusters(std::vector<Node>& nodes) {
    for (Node& node : nodes) {
        if (node.state == "ClusterHead") {
            for (Node& other : nodes) {
                if (other.state == "Unclustered" && node.distanceTo(other) < COMM_RANGE) {
                    other.state = "Follower";
                    other.clusterID = node.clusterID;
                }
            }
        }
    }
}

void printClusters(const std::vector<Node>& nodes) {
    for (const Node& node : nodes) {
        std::cout << "Node " << node.id << ": State = " << node.state << ", ClusterID = " << node.clusterID << "\n";
    }
}

int main() {
    std::vector<Node> nodes;
    initNodes(nodes);
    int iterations = 0;

    while (iterations < MAX_ITERATIONS) {
        clusterFormation(nodes);
        migrateClusters(nodes);
        iterations++;
    }

    printClusters(nodes);
    return 0;
}
