#include <string>
#include "FlowManager.h"

int main() {
	std::string file_path = R"(./map_input.txt)";		// map input file location
	FlowManager manager;

	// get map data and make convexes
	manager.read_map_input(file_path);
	manager.output_convex_polygons_to_txt();

	// set valid nieghbors for each point in map
	manager.create_visibility_graph();
	manager.output_visibility_graph_to_txt();

	// find shortest path from start point to end point
	manager.find_shortest_path();
	manager.output_shortest_path_to_txt();
}