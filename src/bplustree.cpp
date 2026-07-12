using namespace std;
#include "../include/bplustree.hpp"

void BPlusTree::splitLeafNode(Node* leafNode, PlayerValuation& newKey, Node*& newLeaf) {
    newLeaf = new Node(true);
    splitCount++;
    for (int i = order / 2; i < leafNode->keys.size(); ++i) {
        newLeaf->keys.push_back(leafNode->keys[i]);
    }
    leafNode->keys.resize(order / 2);

    newLeaf->next = leafNode->next;
    leafNode->next = newLeaf;

    newKey = newLeaf->keys[0];
};

void BPlusTree::splitInternalNode(Node* internalNode, PlayerValuation& newKey, Node*& newInternal) {
    newInternal = new Node(false);
    splitCount++;
    for (int i = order / 2 + 1; i < internalNode->keys.size(); ++i) {
        newInternal->keys.push_back(internalNode->keys[i]);
    }
    for (int i = order / 2 + 1; i < internalNode->children.size(); ++i) {
        newInternal->children.push_back(internalNode->children[i]);
    }

    newKey = internalNode->keys[order / 2];
    internalNode->keys.resize(order / 2);
    internalNode->children.resize(order / 2 + 1);
};

void BPlusTree::deleteTree(Node* node) {
    if (!node) return;
    if (!node->isLeaf) {
        for (Node* child : node->children) {
            deleteTree(child);
        }
    }
    delete node;
}

    BPlusTree::BPlusTree(int neworder){
        this->order = neworder;
        root = nullptr;
        splitCount = 0;
    };

    BPlusTree::~BPlusTree() {
        deleteTree(root);
    };

    bool BPlusTree::search(double key){
        if (!root) return false;
        Node* curr = root;
        while (curr != nullptr) {
            int i = 0;
            while (i < curr->keys.size() && key > curr->keys[i].marketValue) {
                i++; // Look through all keys in the current node
            }

            if (i < curr->keys.size() && key == curr->keys[i].marketValue) {
                return true; // Key found
            }

            if (curr->isLeaf) {
                return false; // Key not found in leaf node
            } else {
                curr = curr->children[i]; // Move to the appropriate child
            }
        }
        return false;
    };

    void BPlusTree::insert(const PlayerValuation& key){
        if (!root) {
            root = new Node(true); // Create a new leaf node
            root->keys.push_back(key); // Insert the key
            return;
        }

        Node* curr = root;
        stack<Node*> path; // To keep track of the path for backtracking
        while (!curr->isLeaf) {
            int i = 0;
            while (i < curr->keys.size() && key > curr->keys[i]) {
                i++; // Look through all keys in the current node
            }

            path.push(curr);
            curr = curr->children[i]; // Move to the appropriate child
        }

        // Now curr is a leaf node, insert the key
        curr->keys.insert(lower_bound(curr->keys.begin(), curr->keys.end(), key), key);

        while (curr->keys.size() >= order) {
            PlayerValuation newKey;
            Node* newNode;

            if (curr->isLeaf) {
                splitLeafNode(curr, newKey, newNode);
            } else {
                splitInternalNode(curr, newKey, newNode);
            }

            if (path.empty()) {
                // If we are at the root, create a new root
                Node* newRoot = new Node(false);
                newRoot->keys.push_back(newKey);
                newRoot->children.push_back(curr); // old root, left side
                newRoot->children.push_back(newNode); // new right side
                root = newRoot;
                return;
            } else {
                // Backtrack to the parent node
                curr = path.top();
                path.pop();

                curr->keys.insert(lower_bound(curr->keys.begin(), curr->keys.end(), newKey), newKey);
                curr->children.insert(curr->children.begin() + (lower_bound(curr->keys.begin(), curr->keys.end(), newKey) - curr->keys.begin()) + 1, newNode);
            }
        }
    };

    void BPlusTree::remove(const PlayerValuation& key){
        if (!root) return; // Tree is empty

        Node* curr = root;
        stack<Node*> path; // To keep track of the path for backtracking
        while (!curr->isLeaf) {
            int i = 0;
            while (i < curr->keys.size() && key > curr->keys[i]) {
                i++; // Look through all keys in the current node
            }

            path.push(curr);
            curr = curr->children[i]; // Move to the appropriate child
        }

        // Now curr is a leaf node, delete the key
        auto it = lower_bound(curr->keys.begin(), curr->keys.end(), key);
        if (it != curr->keys.end() && *it == key) {
            curr->keys.erase(it); // Remove the key
        } else {
            return; // Key not found
        }
        int min_keys = (order - 1) / 2; // Minimum number of keys a node can have

        while (curr->keys.size() < min_keys && curr != root) {
            Node* parent = path.top();
            path.pop();

            int index = 0;
            while (index < parent->children.size() && parent->children[index] != curr) {
                index++;
            }

            Node* left_sibling = nullptr;
            Node* right_sibling = nullptr;
            if (index > 0) {
                left_sibling = parent->children[index - 1]; // Left sibling
            } else if (index < parent->children.size() - 1) {
                right_sibling = parent->children[index + 1]; // Right sibling
            }

            if (left_sibling && left_sibling->keys.size() > min_keys) {
                // Borrow from left sibling

                if (curr->isLeaf) {
                    curr->keys.insert(curr->keys.begin(), left_sibling->keys.back());
                    left_sibling->keys.pop_back();
                    parent->keys[index - 1] = curr->keys[0]; // Update parent key
                } else {
                    curr->keys.insert(curr->keys.begin(), parent->keys[index - 1]);
                    parent->keys[index - 1] = left_sibling->keys.back();
                    left_sibling->keys.pop_back();
                    curr->children.insert(curr->children.begin(), left_sibling->children.back());
                    left_sibling->children.pop_back();
                }

                curr = parent; // Move up to the parent for further checks
                
            } else if (right_sibling && right_sibling->keys.size() > min_keys) {
                // Borrow from right sibling

                if (curr->isLeaf) {
                    curr->keys.push_back(right_sibling->keys.front());
                    right_sibling->keys.erase(right_sibling->keys.begin());
                    parent->keys[index] = right_sibling->keys.front(); // Update parent key
                } else {
                    curr->keys.push_back(parent->keys[index]);
                    parent->keys[index] = right_sibling->keys.front();
                    right_sibling->keys.erase(right_sibling->keys.begin());
                    curr->children.push_back(right_sibling->children.front());
                    right_sibling->children.erase(right_sibling->children.begin());
                }

                curr = parent; // Move up to the parent for further checks
            } else {
                // Merge with a sibling

                if (left_sibling) {
                    // Merge with left sibling
                    if (curr->isLeaf) {
                        left_sibling->keys.insert(left_sibling->keys.end(), curr->keys.begin(), curr->keys.end());
                        left_sibling->next = curr->next; // Update next pointer
                    } else {
                        left_sibling->keys.push_back(parent->keys[index - 1]);
                        left_sibling->keys.insert(left_sibling->keys.end(), curr->keys.begin(), curr->keys.end());
                        left_sibling->children.insert(left_sibling->children.end(), curr->children.begin(), curr->children.end());
                    }
                    // Remove the key and child pointer from the parent
                    parent->keys.erase(parent->keys.begin() + index - 1);
                    parent->children.erase(parent->children.begin() + index);
                    delete curr;
                    curr = parent; // Move up to the parent for further checks
                } else if (right_sibling) {
                    // Merge with right sibling
                    if (curr->isLeaf) {
                        curr->keys.insert(curr->keys.end(), right_sibling->keys.begin(), right_sibling->keys.end());
                        curr->next = right_sibling->next; // Update next pointer
                    } else {
                        curr->keys.push_back(parent->keys[index]);
                        curr->keys.insert(curr->keys.end(), right_sibling->keys.begin(), right_sibling->keys.end());
                        curr->children.insert(curr->children.end(), right_sibling->children.begin(), right_sibling->children.end());
                    }
                    // Remove the key and child pointer from the parent
                    parent->keys.erase(parent->keys.begin() + index);
                    parent->children.erase(parent->children.begin() + index + 1);
                    delete right_sibling;
                    curr = parent; // Move up to the parent for further checks
                }
            }
        }

        // If the root node has no keys, make its first child the new root
        if (root->keys.empty() && !root->isLeaf) {
            Node* oldRoot = root;
            root = root->children[0];
            delete oldRoot;
        }

    };

    vector<PlayerValuation> BPlusTree::rangeSearch(double startKey, double endKey){
        if (!root) return {};

        lastQueryNodeVisits = 0;
        lastQueryComparisons = 0;
        vector<PlayerValuation> result;

        Node* curr = root;
        while (!curr->isLeaf) {
            lastQueryNodeVisits++;
            int i = 0;
            while (i < curr->keys.size() && startKey > curr->keys[i].marketValue) {
                lastQueryComparisons++;
                i++;
            }
            curr = curr->children[i];
        }

        lastQueryNodeVisits++;

        while (curr != nullptr) {
            for (const PlayerValuation& key : curr->keys) {
                lastQueryComparisons++;
                if (key.marketValue >= startKey && key.marketValue <= endKey) {
                    result.push_back(key);
                } else if (key.marketValue > endKey) {
                    return result;
                }
            }
            curr = curr->next;
        }
        return result;
    }

int BPlusTree::height(Node* node) const {
    if (node == nullptr) {
        return 0;
    }
    if (node->isLeaf) {
        return 1;
    }
    return 1 + height(node->children.front());
}

int BPlusTree::getHeight() const {
    return height(root);
}

int BPlusTree::getSplitCount() const {
    return splitCount;
}

int BPlusTree::getLastQueryNodeVisits() const {
    return lastQueryNodeVisits;
}

int BPlusTree::getLastQueryComparisons() const {
    return lastQueryComparisons;
}