#pragma once
using namespace std;
#include <vector>
#include <stack>
#include "../include/record.hpp"

struct Node {
        bool isLeaf; 
        vector<PlayerValuation> keys; 
        vector<Node*> children;
        Node* next; // Pointer to the next leaf node, only for leaf nodes

        Node(bool leaf) : isLeaf(leaf), next(nullptr) {}
    };

class BPlusTree {

    Node* root;
    int order; // Max number of keys in a node/order of the tree
    int splitCount;
    int lastQueryNodeVisits;
    int lastQueryComparisons;

    void splitLeafNode(Node* leafNode, PlayerValuation& newKey, Node*& newLeaf);

    void splitInternalNode(Node* internalNode, PlayerValuation& newKey, Node*& newInternal);

    void deleteTree(Node* node);
    int height(Node* node) const;

    public:
        BPlusTree(int neworder);

        ~BPlusTree();

        bool search(double key);

        void insert(const PlayerValuation& key);

        void remove(const PlayerValuation& key);

        vector<PlayerValuation> rangeSearch(double startKey, double endKey);

        int getHeight() const;
        int getSplitCount() const;
        int getLastQueryNodeVisits() const;
        int getLastQueryComparisons() const;
};