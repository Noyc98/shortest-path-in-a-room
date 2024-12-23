#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include "Polygon.h"


/// @brief Class for creating project managing object
/// @param polygons - Polygons array of the map
/// @param start_point - The starting point of the path
/// @param end_point - The ending point of the path
/// @param x_axis - The x axis range of the map
/// @param y_axis - The y axis range of the map
/// @param polygon_amount - The amount of polygons in the map
class FlowManager {
public:
	FlowManager() = default;

	FlowManager(int x_axis, int y_axis ,Node start, Node end, std::vector<Polygon> polygons) {
		this->x_axis = x_axis;
		this->y_axis = y_axis;
		this->start_point = start;
		this->end_point = end;
		this->polygons = polygons;
		this->polygon_amount = polygons.size();
	}

	/// @brief Reading the map data file created through python (including map dimensions, polygons coordinates etc...)
	/// @param input_file The map data file reference
	/// @return 1 if there was an error openning the file, 0 otherwise
	int read_map_input(std::string& path);

	/// @brief Creating an output file of the new convexes points for the python to read
	/// @return 1 if there was an error openning the file, 0 otherwise
	int output_convex_polygons_to_txt();

	/// @brief Creating an output file of the neighbors of each point for the python to read
	/// @return 1 if there was an error openning the file, 0 otherwise
	int output_visibility_graph_to_txt();

	/// @brief Creating an output file of the shortest path points for the python to read
	/// @return 1 if there was an error openning the file, 0 otherwise
	int output_shortest_path_to_txt();

	/// @brief Reading polygon points data from a file
	/// @param Input_file Reference to the data file
	/// @param Coords_amount How many points the polygon includes
	/// @return A vector of the polygon points
	std::vector<Point> read_poly_data(std::ifstream& input_file, int coords_amount);

	/// @brief Reading dry map details from the map input
	/// @param input_file Reference to the data file 
	void read_map_data(std::ifstream& input_file);

	/// @brief Setting the neighbors vector for each point
	void create_visibility_graph();

	/// @brief Checking intersection between a line and all other lines in map (checking relevant neighbors)
	/// @param pivot First point in line
	/// @param potentila_point Second point in line we are checking if a relevant neighbor
	/// @return True if there is an intersection with another line in map, false otherwise (potential point is a valid neighbor)
	bool check_neighbors(Node& pivot, Node& potentila_point);

	/// @brief Checking if both nodes are in the same map border
	/// @param node1 First node
	/// @param node2 Second node 
	/// @return True if both nodes are border neighbors
	bool border_neighbors(Node& node1, Node& node2);

	/// @brief Reconstructing the path trough the pointers from end to start
	/// @param cameFrom The unordered map that contains each Point and a pointer to the point she came from
	/// @return The path points vector
	std::vector<Node> reconstruct_path(std::unordered_map<Node*, Node*>& cameFrom);

	/// @brief A* algorithm to find the shortest path in the map from start to end
	void shortest_path_a_star();
	void shortest_path_rrt();
	void shortest_path_dijkstra();

private:
	std::vector<Polygon> polygons{};
	std::vector<Node> path{};
	Node start_point{};
	Node end_point{};
	int x_axis = 0;
	int y_axis = 0;
	int polygon_amount = 0;
};