cmake_minimum_required(VERSION 3.15)
project(FootballValuationTrees)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

##pull in desired version of catch through cmake automatically, make it available
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.2 # if you run into issues, try updating your editor or downgrading to a lower version of catch v3
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

add_executable(main 
    src/main.cpp
    src/btree.cpp
    src/bplustree.cpp
    src/csv_loader.cpp
    src/benchmark.cpp
    imgui/imgui.cpp 
    imgui/imgui_draw.cpp 
    imgui/imgui_tables.cpp 
    imgui/imgui_widgets.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
)

target_link_libraries(main glfw OpenGL::GL)
target_include_directories(main PRIVATE imgui imgui/backends)

include_directories(src)
        
# These tests can use the Catch2-provided main
add_executable(Tests
        tests/test_bplustree.cpp # your test file
        # add your own header files below - should be automatically added in CLion
        # example (can also separate with newlines):
        # src/AVL.h src/AVL.cpp
        )
        
target_link_libraries(Tests PRIVATE Catch2::Catch2WithMain) #link catch to test.cpp file
# the name here must match that of your testing executable (the one that has test.cpp)

# comment everything below out if you are using CLion
include(CTest)
include(Catch)
catch_discover_tests(Tests) # must be named the same as your test executable