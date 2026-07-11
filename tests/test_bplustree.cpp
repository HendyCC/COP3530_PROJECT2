#include <catch2/catch_test_macros.hpp>
using namespace std;
#include <vector>
#include <stack>

class BPlusTree {
    struct Node {
        bool isLeaf; 
        vector<int> keys; 
        vector<Node*> children;
        Node* next; // Pointer to the next leaf node, only for leaf nodes

        Node(bool leaf) : isLeaf(leaf), next(nullptr) {}
    };

    Node* root;
    int order; // Max number of keys in a node/order of the tree

    void splitLeafNode(Node* leafNode, int& newKey, Node*& newLeaf) {
        newLeaf = new Node(true);
        for (int i = order / 2; i < leafNode->keys.size(); ++i) {
            newLeaf->keys.push_back(leafNode->keys[i]);
        }
        leafNode->keys.resize(order / 2);

        newLeaf->next = leafNode->next;
        leafNode->next = newLeaf;

        newKey = newLeaf->keys[0];
    };

    void splitInternalNode(Node* internalNode, int& newKey, Node*& newInternal) {
        newInternal = new Node(false);
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

    void deleteTree(Node* node) {
        if (!node) return;
        if (!node->isLeaf) {
            for (Node* child : node->children) {
                deleteTree(child);
            }
        }
        delete node;
    }

    public:
        BPlusTree(int neworder){
            this->order = neworder;
            root = nullptr;
        };

        ~BPlusTree() {
            deleteTree(root);
        };

        bool search(int key){
            if (!root) return false;
            Node* curr = root;
            while (curr != nullptr) {
                int i = 0;
                while (i < curr->keys.size() && key > curr->keys[i]) {
                    i++; // Look through all keys in the current node
                }

                if (i < curr->keys.size() && key == curr->keys[i]) {
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

        void insert(int key){
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
                int newKey;
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

        void remove(int key){
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
        vector<int> rangeSearch(int startKey, int endKey){
            if (!root) return {}; // Tree is empty

            vector<int> result;

            Node* curr = root;
            while (!curr->isLeaf) {
                int i = 0;
                while (i < curr->keys.size() && startKey > curr->keys[i]) {
                    i++; // Look through all keys in the current node
                }
                curr = curr->children[i]; // Move to the appropriate child
            }

            // Now curr is a leaf node, collect keys in the range
            while (curr != nullptr) {
                for (int key : curr->keys) {
                    if (key >= startKey && key <= endKey) {
                        result.push_back(key);
                    } else if (key > endKey) {
                        return result; // No need to check further
                    }
                }
                curr = curr->next; // Move to the next leaf node
            }
            return result;
        };
};

TEST_CASE("B+ tree search and creation", "[bplustree]") {
    BPlusTree bpt(4);

    SECTION("Searching an empty tree returns false") {
        REQUIRE(bpt.search(10) == false);
    }
    
    SECTION("Searching for a non-existent key returns false") {
        bpt.insert(10);
        bpt.insert(20);
        REQUIRE(bpt.search(99) == false);
    }
}

TEST_CASE("B+ tree insertion", "[bplustree]") {
    BPlusTree bpt(4); // Order 4 means max 3 keys per node. 4th key triggers split.

    SECTION("Insert keys without splitting") {
        bpt.insert(10);
        bpt.insert(20);
        bpt.insert(5);

        REQUIRE(bpt.search(10) == true);
        REQUIRE(bpt.search(20) == true);
        REQUIRE(bpt.search(5) == true);
    }

    SECTION("Insert enough keys to trigger a leaf split") {
        bpt.insert(10);
        bpt.insert(20);
        bpt.insert(30);
        bpt.insert(40); // This should trigger the first split

        REQUIRE(bpt.search(10) == true);
        REQUIRE(bpt.search(20) == true);
        REQUIRE(bpt.search(30) == true);
        REQUIRE(bpt.search(40) == true);
    }

    SECTION("Insert enough keys to trigger an internal node split") {
        // Inserting 1..15 sequentially will heavily test your splitInternalNode logic
        for (int i = 1; i <= 15; i++) {
            bpt.insert(i);
        }

        for (int i = 1; i <= 15; i++) {
            REQUIRE(bpt.search(i) == true);
        }
    }
}

TEST_CASE("B+ tree range search", "[bplustree]") {
    BPlusTree bpt(4);
    
    // Insert out of order to make sure the tree sorts them correctly
    std::vector<int> data = {50, 10, 40, 20, 60, 30};
    for (int val : data) bpt.insert(val);

    SECTION("Standard range search") {
        std::vector<int> result = bpt.rangeSearch(15, 45);
        std::vector<int> expected = {20, 30, 40};
        
        REQUIRE(result.size() == expected.size());
        REQUIRE(result == expected);
    }

    SECTION("Range search extending past existing keys") {
        std::vector<int> result = bpt.rangeSearch(45, 100);
        std::vector<int> expected = {50, 60};
        REQUIRE(result == expected);
    }

    SECTION("Range search with no matches") {
        std::vector<int> result = bpt.rangeSearch(90, 100);
        REQUIRE(result.empty() == true);
    }
}

TEST_CASE("B+ tree removal", "[bplustree]") {
    BPlusTree bpt(4);
    
    // Build a tree big enough to test borrowing and merging
    for (int i = 10; i <= 80; i += 10) {
        bpt.insert(i);
    }
    // Tree contains: 10, 20, 30, 40, 50, 60, 70, 80

    SECTION("Delete a key that does not cause borrowing or merging") {
        bpt.remove(80);
        REQUIRE(bpt.search(80) == false);
        REQUIRE(bpt.search(70) == true); // Make sure rest of tree is safe
    }

    SECTION("Delete a key that triggers a borrow or merge") {
        bpt.remove(20);
        REQUIRE(bpt.search(20) == false);
        
        // Ensure the tree didn't break during the structural change
        REQUIRE(bpt.search(10) == true);
        REQUIRE(bpt.search(30) == true);
        
        // Range search is the ultimate test of a successful merge/borrow!
        std::vector<int> result = bpt.rangeSearch(0, 100);
        std::vector<int> expected = {10, 30, 40, 50, 60, 70, 80};
        REQUIRE(result == expected);
    }

    SECTION("Delete all keys to trigger root shrink and empty tree") {
        for (int i = 10; i <= 80; i += 10) {
            bpt.remove(i);
        }
        
        REQUIRE(bpt.search(50) == false);
        REQUIRE(bpt.rangeSearch(0, 100).empty() == true);
    }
}