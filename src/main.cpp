#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "../include/btree.hpp" 
#include "../include/bplustree.hpp"
#include "../include/benchmark.hpp"
#include "../include/record.hpp"
#include "../include/rapid_csv.hpp" 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>

int main() {
    if (!glfwInit()) return 1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Group 109 - Football Market Value Analyzer", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Datasets for B-Tree and B+ Tree
    int treeOrder;
    std::cout << "Enter tree order: ";
    std::cin >> treeOrder;

    size_t maxRows;
    std::cout << "Enter number of rows to load: ";
    std::cin >> maxRows;

    Btree b_tree(treeOrder);
    BPlusTree b_plus_tree(treeOrder);

    std::vector<PlayerValuation> dataset;
    double bTreeBuildTime = 0.0;
    double bPlusTreeBuildTime = 0.0;

    std::unordered_map<int, std::string> playerNames;
    try {
        rapidcsv::Document playersDoc("../players.csv");
        size_t playerRows = playersDoc.GetRowCount();
        for (size_t i = 0; i < playerRows; ++i) {
            int id = playersDoc.GetCell<int>("player_id", i);
            playerNames[id] = playersDoc.GetCell<std::string>("name", i);
        }
    } catch (const std::exception& e) {
        std::cerr << "[DataLoader] Error reading players.csv: " << e.what() << "\n";
    }

    try {
        rapidcsv::Document doc("../player_valuations.csv");
        
        int loadedCount = 0;
        size_t totalRows = doc.GetRowCount();

        for (size_t i = 0; i < totalRows && (size_t)loadedCount < maxRows; ++i) {

            std::string valStr = doc.GetCell<std::string>("market_value_in_eur", i);
            if (valStr.empty()) continue;

            int playerId = doc.GetCell<int>("player_id", i);
            std::string date = doc.GetCell<std::string>("date", i);
            double marketValue = std::stod(valStr);

            PlayerValuation record;
            record.marketValue = marketValue;
            record.playerId = playerId;
            record.date = date;
            
            dataset.push_back(record);
            
            loadedCount++;
        }

        auto bTreeBuildStart = std::chrono::high_resolution_clock::now();
        for (const auto& record : dataset) {
            b_tree.insert(record);
        }
        auto bTreeBuildEnd = std::chrono::high_resolution_clock::now();

        auto bPlusTreeBuildStart = std::chrono::high_resolution_clock::now();
        for (const auto& record : dataset) {
            b_plus_tree.insert(record);
        }
        auto bPlusTreeBuildEnd = std::chrono::high_resolution_clock::now();

        bTreeBuildTime = std::chrono::duration<double, std::milli>(bTreeBuildEnd - bTreeBuildStart).count();
        bPlusTreeBuildTime = std::chrono::duration<double, std::milli>(bPlusTreeBuildEnd - bPlusTreeBuildStart).count();
        
        std::cout << "[DataLoader] Successfully loaded " << loadedCount << " valuations into the tree!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "[DataLoader] Error reading CSV: " << e.what() << "\n";
    }


    int queryType = 0; // 0 = Exact Lookup, 1 = Range Query
    int exactValue = 0;
    int rangeMin = 0;
    int rangeMax = 0;
    bool showResults = false;
    double bTreeTime = 0.0;
    double bPlusTreeTime = 0.0;
    int bTreeCount = 0;
    int bPlusTreeCount = 0;
    int bTreeNodeVisits = 0;
    int bPlusTreeNodeVisits = 0;
    int bTreeComparisons = 0;
    int bPlusTreeComparisons = 0;
    std::vector<PlayerValuation> bTreeResults;

    while (!glfwWindowShouldClose(window)) {
        // Start the ImGui frame
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("Football Market Value Analyzer", nullptr, flags);

        ImGui::Text("Select Query Type:");
        ImGui::RadioButton("Exact Lookup", &queryType, 0); ImGui::SameLine();
        ImGui::RadioButton("Range Query", &queryType, 1);

        if (queryType == 0) {
            ImGui::InputInt("Market Value (€)", &exactValue);
        } else {
            ImGui::InputInt("Min Value (€)", &rangeMin);
            ImGui::InputInt("Max Value (€)", &rangeMax);
        }

        if (ImGui::Button("Run Benchmark")) {
            if (queryType == 0) {
                // Perform exact lookup benchmark for B-Tree and B+ Tree
                bTreeTime = Benchmarker::benchmarkBTreeRange(b_tree, exactValue, exactValue, bTreeCount);
                bPlusTreeTime = Benchmarker::benchmarkBPlusTreeRange(b_plus_tree, exactValue, exactValue, bPlusTreeCount);
                bTreeResults = b_tree.range_search(exactValue, exactValue);
            } else {
                // Perform range query benchmark for B-Tree and B+ Tree
                bTreeTime = Benchmarker::benchmarkBTreeRange(b_tree, rangeMin, rangeMax, bTreeCount);
                bPlusTreeTime = Benchmarker::benchmarkBPlusTreeRange(b_plus_tree, rangeMin, rangeMax, bPlusTreeCount);
                bTreeResults = b_tree.range_search(rangeMin, rangeMax);
            }

            bTreeNodeVisits = b_tree.getLastQueryNodeVisits();
            bPlusTreeNodeVisits = b_plus_tree.getLastQueryNodeVisits();
            bTreeComparisons = b_tree.getLastQueryComparisons();
            bPlusTreeComparisons = b_plus_tree.getLastQueryComparisons();
            showResults = true;
        }

        if (showResults) {
            ImGui::Separator();
            ImGui::Text("Benchmarking Results (Order %d):", treeOrder);
            ImGui::Text("Build Time: B-Tree %.3f ms | B+ Tree %.3f ms", bTreeBuildTime, bPlusTreeBuildTime);
            
            ImGui::Columns(2, "BenchmarkColumns"); 
            ImGui::Text("B-Tree");
            ImGui::Text("Time: %.3f ms", bTreeTime);
            ImGui::Text("Found: %d records", bTreeCount);
            ImGui::Text("Node Visits: %d", bTreeNodeVisits);
            ImGui::Text("Comparisons: %d", bTreeComparisons);
            ImGui::Text("Height: %d", b_tree.getHeight());
            ImGui::Text("Splits: %d", b_tree.getSplitCount());
            
            ImGui::NextColumn();
            ImGui::Text("B+ Tree");
            ImGui::Text("Time: %.3f ms", bPlusTreeTime);
            ImGui::Text("Found: %d records", bPlusTreeCount);
            ImGui::Text("Node Visits: %d", bPlusTreeNodeVisits);
            ImGui::Text("Comparisons: %d", bPlusTreeComparisons);
            ImGui::Text("Height: %d", b_plus_tree.getHeight());
            ImGui::Text("Splits: %d", b_plus_tree.getSplitCount());
            ImGui::Columns(1);

            ImGui::Separator();
            ImGui::Text("Matching Players:");

            for (const auto& record : bTreeResults) {
                auto it = playerNames.find(record.playerId);
                std::string name = (it != playerNames.end()) ? it->second : ("Player " + std::to_string(record.playerId));
                ImGui::Text("%s | Market Value: %.0f | Date: %s",
                            name.c_str(),
                            record.marketValue,
                            record.date.c_str());
            }
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}