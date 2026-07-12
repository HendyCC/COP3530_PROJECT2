# Football Player Valuation: B-Tree vs B+ Tree

This project uses the [Football Data from Transfermarkt](https://www.kaggle.com/datasets/davidcariboo/player-scores) dataset from Kaggle.
Data is not included in this repo (too large) — each team member needs to download it individually.

## 1. Problem
Football clubs and scouts need to query large player valuation databases quickly, finding players that fit into their budget. This may involve searching for a range of values or how a player's value has changed over time. We are implementing and comparing a B-Tree and a B+ Tree to determine which is better for the types of queries analysts may use.

## 2. Motivation
As datasets for player valuations grow in size to over 100k+ rows, it becomes even more important to have an efficient searching mechanism that doesn't hold analysts back. B-Trees and B+ Trees are widely used to organize and quickly retrieve data usually stored on disk. B-Trees and B+ Trees differ in that B+ Trees store all data in linked leaf nodes, while B-Trees store it across the tree itself. We expect this difference to matter for range-based queries, making B+ Trees better in this situation. If an analyst chose the wrong structure, it could force them to wait up to minutes longer between queries, drastically reducing their efficiency.

## 3. Features
- A query engine that loads player market valuation records and builds a B-Tree and B+ Tree for the data
- Exact lookup queries
- Range-based queries
- A benchmarking tool that tracks times based on structure and query type
- A menu to easily select query type, as well as a way to showcase benchmarks and results

## 4. Data
We are using the Football Data from Transfermarkt on Kaggle. It covers 500,000+ player valuations as well as lots of other football-related information that may be useful. Our primary table is `player_valuations.csv`, which contains one row per player valuation per date. We will use historical data, not just present data, since present data doesn't have enough players to hit the 100k+ rows requirement. Since player market valuations aren't distinct, we will use `player_id` as a secondary key to order same-primary-key data.

## 5. Tools
We are using C++ because we need high-performance code. We use Git/GitHub to keep our work organized and CLion/VS Code as our development environment.

## 6. Visuals
The program will have a menu interface where the user chooses the query type and enters the specifics. The program will then run the query for both structures, and the output page will show side-by-side benchmarking results as well as the query results.

## 7. Strategy
We will implement both structures using market value as the primary key and player_id as a secondary key. For the B-Tree, we will implement standard node-splitting logic for balance, with data stored in leaf and non-leaf nodes. The B+ Tree will use the same balancing logic, but with data only in the leaves, linked together for easier range traversal. We expect both structures to perform similarly on exact lookup queries at O(log n), but expect B+ Trees to be faster on range-based queries, with the speed difference related to the size of the query range. We will benchmark insertion speed, exact lookup speed, and range-based query speed across different ranges and sizes for both structures, with a max size of at least 100k+ rows.

## 8. Distribution of Responsibility and Roles
- **B-Tree implementation: Carl-Hendy Celestin
- **B+ Tree implementation: Patrick King
- **Benchmarking tool and performance measurement: Alejandro DelaTorre

## Building and Running

### Prerequisites
- C++17 compiler + CMake 3.15+
- OpenGL (system-provided)
- Internet access on first configure (GLFW and Catch2 are pulled via CMake FetchContent)

### Build
mkdir build && cd build

cmake ..

cmake --build .

### Run
Run the `main` executable from inside the `build/` directory. It loads `player_valuations.csv` and `players.csv` via a relative `../` path, so it will fail if run from anywhere else.

It's a console + GUI hybrid: it first prompts on stdin for `Enter tree order:` then `Enter number of rows to load:` before switching to the ImGui window. In the window, pick Exact Lookup or Range Query, enter a value, and click Run Benchmark to see side-by-side B-Tree/B+-Tree stats (time, node visits, etc.) and the matching players.
