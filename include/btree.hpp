#pragma once
#include <iostream>
#include <vector>
#include "record.hpp"

using namespace std;

struct Btree_node{
    vector<PlayerValuation> keys;
    vector<Btree_node*> children;
    bool isleaf;

    Btree_node(bool leaf) : isleaf{leaf} {}
};

class Btree{
private:
    Btree_node* root;
    int count = 0;
    int Max_keys = 3;
    int splitCount = 0;
    int lastQueryNodeVisits = 0;
    int lastQueryComparisons = 0;

    void split_children(Btree_node* parent, int child_index);
    void insert_child(Btree_node* node, const PlayerValuation& record);
    bool search(Btree_node* node, double market_value, int player_id, PlayerValuation& result);
    void range_search(Btree_node* node, double min_value, double max_value, vector<PlayerValuation>& results, int& nodeVisits, int& comparisons);
    int height(Btree_node* node) const;
public:
    Btree(int maximum): root(nullptr), Max_keys(maximum) {};
    void insert(const PlayerValuation& record);
    bool search(double market_value, int player_id, PlayerValuation& result);
    vector<PlayerValuation> range_search(double min_value, double max_value);
    int size() const;
    int getHeight() const;
    int getSplitCount() const;
    int getLastQueryNodeVisits() const;
    int getLastQueryComparisons() const;
};

bool order_check(const PlayerValuation& record1, const PlayerValuation& record2);
