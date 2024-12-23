#include <string>
#include "FlowManager.h"

int main() {
    std::string file_path = R"(../work/map_input.txt)"; // Map input file location
    FlowManager manager;

    // Get map data and make convexes
    manager.read_map_input(file_path);
    manager.output_convex_polygons_to_txt();

    // Set valid neighbors for each point in the map
    manager.create_visibility_graph();
    manager.output_visibility_graph_to_txt();

    // Find shortest path from start point to end point
    manager.shortest_path_a_star();
    manager.output_shortest_path_to_txt();

    return 0;
}