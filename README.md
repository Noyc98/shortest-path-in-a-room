# Robotics Lab Project

## Overview
This project focuses on finding the shortest path between two points in a room using various algorithms. It supports both convex and non-convex polygonal maps and processes real-world point cloud data. The project combines Python for visualization and preprocessing with a C++ core module for computational efficiency.

## Features
- **Random Map Generation**: Generates random maps with obstacles and calculates the shortest path between start and end points.
- **Room Representation with Obstacles**: Simulates confined spaces with non-convex polygonal obstacles.
- **Point Cloud Processing**: Handles real-world environments by converting `.xyz` files into polygonal maps.

## Algorithms Used
- A*
- Dijkstra's Algorithm
- Rapidly-exploring Random Tree (RRT)
- Triangulation
- Visibility Graphs (Optimized using an O(n² log n) algorithm)

## Workflow
### Input
- Randomly generated maps or `.xyz` files representing real-world environments.

### Process
1. Map generation or point cloud conversion.
2. Data transfer between Python and C++ via `.txt` files.
3. Triangulation and visibility graph construction.
4. Pathfinding using the selected algorithm.

### Output
- The shortest path visualized using Python.

## Code Structure
### Python
- **`Polygon` Class**: Generates polygonal obstacles.
- **`Map` Class**: Visualizes maps, obstacles, and paths.
- **`FlowManager` Class**: Manages input, data flow, and visualization.

### C++
- **`Point` Class**: Handles geometric calculations.
- **`Node` Class**: Represents graph nodes and neighbors.
- **`Polygon` Class**: Manages room and obstacle polygons.
- **`FlowManager` Class**: Controls data flow and main logic.

## Examples
- Randomly generated maps with various obstacle configurations.
- Processed point clouds from real-world lab environments.

## References
1. *Environment Detection and Path Planning Using the E-puck Robot* - [ResearchGate](https://www.researchgate.net/publication/314299113_Environment_Detection_and_Path_Planning_Using_the_E-puck_Robot)
2. [Algorithm for Computing Visibility Graphs](https://www.science.smith.edu/~istreinu/Teaching/Courses/274/Spring98/Projects/Philip/fp/algVisibility.htm)
