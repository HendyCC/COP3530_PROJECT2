#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>
#include "../include/record.hpp"
#include "../include/btree.hpp"
#include "../include/bplustree.hpp" 

class Benchmarker {
public:
    static std::chrono::high_resolution_clock::time_point now() {
        return std::chrono::high_resolution_clock::now();
    }

   
    static double duration_ms(std::chrono::high_resolution_clock::time_point start, 
                               std::chrono::high_resolution_clock::time_point end) {
        return std::chrono::duration<double, std::milli>(end - start).count();
    }

// exact lookups
    static double duration_us(std::chrono::high_resolution_clock::time_point start, 
                               std::chrono::high_resolution_clock::time_point end) {
        return std::chrono::duration<double, std::micro>(end - start).count();
    }

    // B-TREE BENCHMARKS 
    
    static double benchmarkBTreeInsert(Btree& tree, const std::vector<PlayerValuation>& dataset) {
        auto start = now();
        for (const auto& record : dataset) {
            tree.insert(record);
        }
        auto end = now();
        return duration_ms(start, end);
    }

    static double benchmarkBTreeLookup(Btree& tree, double target_val, int target_id, int& out_found) {
        PlayerValuation resultPlaceholder;
        auto start = now();
        bool found = tree.search(target_val, target_id, resultPlaceholder);
        auto end = now();
        out_found = found ? 1 : 0;
        return duration_us(start, end);
    }

    static double benchmarkBTreeRange(Btree& tree, double min_val, double max_val, int& out_count) {
        auto start = now();
        std::vector<PlayerValuation> results = tree.range_search(min_val, max_val);
        auto end = now();
        out_count = results.size();
        return duration_ms(start, end);
    }

    // B+ TREE BENCHMARKS 
    
    static double benchmarkBPlusTreeInsert(BPlusTree& tree, const std::vector<PlayerValuation>& dataset) {
        auto start = now();
        for (const auto& record : dataset) {
            // Casts double marketValue to int to match your B+ Tree's sign signature
            tree.insert(record);
        }
        auto end = now();
        return duration_ms(start, end);
    }

    static double benchmarkBPlusTreeLookup(BPlusTree& tree, double target_val, int& out_found) {
        auto start = now();
        bool found = tree.search(target_val);
        auto end = now();
        out_found = found ? 1 : 0;
        return duration_us(start, end);
    }

    static double benchmarkBPlusTreeRange(BPlusTree& tree, double min_val, double max_val, int& out_count) {
        auto start = now();
        std::vector<PlayerValuation> results = tree.rangeSearch(min_val, max_val);
        auto end = now();
        out_count = results.size();
        return duration_ms(start, end);
    }
};

#endif // BENCHMARK_HPP
