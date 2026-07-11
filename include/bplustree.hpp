using namespace std;
#include <vector>
#include <stack>

struct Node {
        bool isLeaf; 
        vector<int> keys; 
        vector<Node*> children;
        Node* next; // Pointer to the next leaf node, only for leaf nodes

        Node(bool leaf) : isLeaf(leaf), next(nullptr) {}
    };

class BPlusTree {

    Node* root;
    int order; // Max number of keys in a node/order of the tree

    void splitLeafNode(Node* leafNode, int& newKey, Node*& newLeaf);

    void splitInternalNode(Node* internalNode, int& newKey, Node*& newInternal);

    void deleteTree(Node* node);

    public:
        BPlusTree(int neworder);

        ~BPlusTree();

        bool search(int key);

        void insert(int key);

        void remove(int key);

        vector<int> rangeSearch(int startKey, int endKey);
};