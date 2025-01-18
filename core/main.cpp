#include <string>
#include "FlowManager.h"

int main() {
    std::string file_path = R"(../work/map_input.txt)"; // Map input file location
    FlowManager manager;

    // Read input from file
    manager.read_map_input(file_path);

    // transform polygons to convex form
    manager.triangulate_polygons();
    //manager.transform_polygons_to_non_convex_form();
    manager.output_convex_polygons_to_txt();

    // Set valid neighbors for each point in the map
    manager.create_visibility_graph_brute_force();
    manager.output_visibility_graph_to_txt();

    // Find shortest path from start point to end point
    manager.find_shortest_path();
    manager.output_shortest_path_to_txt();

    return 0;
}