
#include "../include/record.hpp"
#include "../include/btree.hpp"
#include <vector>

int Btree::size() const {
    return count;
}

int Btree::height(Btree_node* node) const {
    if (node == nullptr) {
        return 0;
    }
    if (node->isleaf) {
        return 1;
    }
    return 1 + height(node->children.front());
}

int Btree::getHeight() const {
    return height(root);
}

int Btree::getSplitCount() const {
    return splitCount;
}

int Btree::getLastQueryNodeVisits() const {
    return lastQueryNodeVisits;
}

int Btree::getLastQueryComparisons() const {
    return lastQueryComparisons;
}


//True if record1 is before record2
bool order_check (const PlayerValuation& record1, const PlayerValuation& record2) {
    if(record1.marketValue != record2.marketValue) {
        return record1.marketValue < record2.marketValue;
    }
    return record1.playerId < record2.playerId;
}


//inserting element into tree
void Btree::insert(const PlayerValuation& record){

    if(root == nullptr) {
        root = new Btree_node(true);
        root->keys.push_back(record);
        count++;
        return;
    }

    if(root->keys.size() == Max_keys) {
        Btree_node* new_root = new Btree_node(false);
        new_root->children.push_back(root);
        split_children(new_root, 0);
        root = new_root;
    }

    Btree_node* current = root;

    while(!current->isleaf) {

        int child_index = 0;

        for( int i = 0; i < current->keys.size(); i++) {
            if(!order_check(record, current->keys[i])) {
                child_index = i + 1;
            }
        }

        if(current->children[child_index]->keys.size() == Max_keys) {
            split_children(current, child_index);
            if(!order_check(record, current->keys[child_index])) {
                child_index++;
            }
        }

        current = current->children[child_index];
    }

    current->keys.push_back(record);
    int i = current->keys.size() - 2;
    while(i >=0 && order_check(record, current->keys[i])) {
        current->keys[i+1] = current->keys[i];
        i--;
    }
    current->keys[i +1] = record;
    count++;

}

void Btree::split_children(Btree_node* parent, int child_index) {
    Btree_node* full_child = parent->children[child_index];
    int middle = full_child->keys.size() / 2;
    splitCount++;

    //middle key goes up
    PlayerValuation middle_key = full_child->keys[middle];

    //new node
    Btree_node* new_right = new Btree_node(full_child->isleaf);
    for (int i = middle +1; i < full_child->keys.size(); i++) {
        new_right->keys.push_back(full_child->keys[i]);
    }

    if(!full_child->isleaf) {
        for(int i = middle + 1; i < full_child->children.size(); i++) {
            new_right->children.push_back(full_child->children[i]);
        }
        full_child->children.resize(middle+1);
    }

    full_child->keys.resize(middle);
    parent->keys.insert(parent->keys.begin() + child_index, middle_key);
    parent->children.insert(parent->children.begin() + child_index + 1, new_right);
}


bool Btree::search(Btree_node* node, double market_value, int player_id, PlayerValuation& result) {
    if (node == nullptr) {
        return false;
    }
    lastQueryNodeVisits++;
    //checking node
    for(int i = 0; i < node->keys.size(); i++) {
        lastQueryComparisons++;
        if(node->keys[i].marketValue == market_value && node->keys[i].playerId == player_id) {
            result = node->keys[i];
            return true;
        }
    }
    if(node->isleaf) {
        return false;
    }
    int child_index = 0;
    while(child_index < node->keys.size()) {
        PlayerValuation current_key = node->keys[child_index];

        if(market_value < current_key.marketValue) {
            break;
        }

        if(market_value == current_key.marketValue && player_id <current_key.playerId) {
            break;
        }
        child_index++;
    }

    return search(node->children[child_index], market_value, player_id, result);
};

bool Btree::search(double market_value, int player_id, PlayerValuation& result) {
    return search(root, market_value, player_id, result);
}

vector<PlayerValuation> Btree::range_search(double min_value, double max_value) {
    vector<PlayerValuation> results;
    lastQueryNodeVisits = 0;
    lastQueryComparisons = 0;
    range_search(root, min_value, max_value, results, lastQueryNodeVisits, lastQueryComparisons);
    return results;
}
void Btree::range_search(Btree_node* node, double min_value, double max_value, vector<PlayerValuation>& results, int& nodeVisits, int& comparisons) {
    if (node == nullptr) {
        return;
    }

    nodeVisits++;

    int i = 0;
    while (i < node->keys.size()) {
        comparisons++;
        if (!node->isleaf && min_value <= node->keys[i].marketValue) {
            range_search(node->children[i], min_value, max_value, results, nodeVisits, comparisons);
        }

        comparisons++;
        if (node->keys[i].marketValue >= min_value && node->keys[i].marketValue <= max_value) {
            results.push_back(node->keys[i]);
        }

        i++;
    }

    if (!node->isleaf && (node->keys.empty() || max_value >= node->keys.back().marketValue)) {
        range_search(node->children[i], min_value, max_value, results, nodeVisits, comparisons);
    }
}
