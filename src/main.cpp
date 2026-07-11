#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "../include/btree.hpp" 
#include "../include/bplustree.hpp"

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
    // go here 

    int queryType = 0; // 0 = Exact Lookup, 1 = Range Query
    int exactValue = 0;
    int rangeMin = 0;
    int rangeMax = 0;
    bool showResults = false;
    double bTreeTime = 0.0;
    double bPlusTreeTime = 0.0;

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
            // bTreeTime = benchmark;
            // bPlusTreeTime = benchmark;

            showResults = true;
        }

        if (showResults) {
            ImGui::Separator();
            ImGui::Text("Benchmarking Results:");
            
            ImGui::Columns(2, "BenchmarkColumns"); 
            ImGui::Text("B-Tree");
            ImGui::Text("Time: %.3f ms", bTreeTime);
            
            ImGui::NextColumn();
            ImGui::Text("B+ Tree");
            ImGui::Text("Time: %.3f ms", bPlusTreeTime);
            ImGui::Columns(1);
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