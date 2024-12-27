#pragma once 
#include "FlowManager.h"
#include <filesystem>
#include <algorithm>


int FlowManager::read_map_input(std::string& path)
{	
	std::ifstream input_file(path);

	if (!input_file.is_open()) {
		return 1;
	}

	// set dry map data from file
	read_map_data(input_file);	

	std::string line;

	// set each polygon coordinates and serial number
	for (int i = 0; i < this->polygon_amount; i++) {
		int coords_amount;
		int poly_id;

		getline(input_file, line);
		poly_id = stoi(line);
		getline(input_file, line);
		coords_amount = stoi(line);

		// get the coordinates and
		std::vector<Point> coords = read_poly_data(input_file, coords_amount);
		Polygon temp_polygon(coords);
		temp_polygon.setPolyId(poly_id);

		if (temp_polygon.getRawPoints().size() < 3) {
			return 1;		// not a valid convex created
		}

		this->polygons.push_back(temp_polygon);

	}

	// get the algorithm type
	std::getline(input_file, line);
	this->algorithm_type = std::stoi(line);
	return 0;
}


std::vector<Point> FlowManager::read_poly_data(std::ifstream& input_file, int coords_amount) {
	float xVal, yVal;
	std::vector<Point> coords;
	std::string line;

	// read the points from file into a vector
	for (int i = 0; i < coords_amount; i++) {
		getline(input_file, line);
		std::istringstream word(line);
		word >> xVal >> yVal;
		Point point(xVal, yVal);
		coords.push_back(point);
	}

	return coords;
}


void FlowManager::read_map_data(std::ifstream& input_file) {
	std::string line;
	float xVal, yVal;
	int num_of_poly;

	// set map range\size values
	std::getline(input_file, line);
	this->x_axis = std::stoi(line);
	std::getline(input_file, line);
	this->y_axis = std::stoi(line);

	// set start and end points
	std::getline(input_file, line);
	std::istringstream word1(line);
	word1 >> xVal >> yVal;
	this->start_point.setPoint(xVal, yVal);

	std::getline(input_file, line);
	std::istringstream word2(line);
	word2 >> xVal >> yVal;
	this->end_point.setPoint(xVal, yVal);

	// set number of polygons in the map
	std::getline(input_file, line);
	num_of_poly = std::stoi(line);
	this->polygon_amount = num_of_poly;
}


int FlowManager::output_convex_polygons_to_txt() {

	std::ofstream output_file("../work/polygons_hull.txt");

	if (!output_file.is_open()) {
		return 1;
	}

	// write the convexes points to the file
	for (int i = 0; i < this->polygons.size(); i++) {
		std::vector<Node>& coords = this->polygons[i].getCoords();
		for (int j = 0; j < coords.size(); j++) {
			output_file << coords[j].getX() << ' ' << coords[j].getY() << std::endl;
		}
		output_file << std::endl;
	}

	return 0;
}


int FlowManager::output_visibility_graph_to_txt() {

	std::ofstream output_file("../work/neighbors.txt");
	Node& start = this->start_point;

	if (!output_file.is_open()) {
		return 1;
	}

	// write start point neighbors data
	output_file << start.getX() << ' ' << start.getY() << std::endl;
	std::vector<Node*> start_neighbors = start.getNeighbors();
	for (int k = 0; k < start_neighbors.size(); k++) {
		output_file << start_neighbors[k]->getX() << ' ' << start_neighbors[k]->getY() << std::endl;
	}
	output_file << std::endl;

	// write neighbors data for rest of the points
	for (int i = 0; i < this->polygons.size(); i++) {
		std::vector<Node>& coords = this->polygons[i].getCoords();
		for (int j = 0; j < coords.size(); j++) {
			output_file << coords[j].getX() << ' ' << coords[j].getY() << std::endl;
			std::vector<Node*> neighbors = coords[j].getNeighbors();
			for(int i = 0; i < neighbors.size(); i++)
				output_file << neighbors[i]->getX() << ' ' << neighbors[i]->getY() << std::endl;
			output_file << std::endl;
		}
	}

	return 0;
}


int FlowManager::output_shortest_path_to_txt() {
	std::ofstream output_file("../work/path.txt");

	if (!output_file.is_open()) {
		return 1;
	} 

	// avoid accessing empty vector
	else if (path.empty()) {
		return 1;
	}

	// write path points to the file
	for (int i = 0; i < this->path.size(); i++) {
		output_file << path[i].getX() << ' ' << path[i].getY() << std::endl;
	}
}


void FlowManager::transform_polygons_to_convex_form() {
	for (Polygon& polygon : this->polygons) {
		polygon.transform_to_convex_polygon();
	}
}


void FlowManager::triangulate_polygons() {
	std::vector<Polygon> triangulated_polygons;

	for (Polygon& polygon : this->polygons) {
		const std::vector<Point>& points = polygon.getRawPoints();

		if (points.size() < 3) {
			continue; // Skip invalid or degenerate polygons
		}
		else if (points.size() == 3) {
			Polygon triangle(points);
			triangle.transformPointsToCoords();
			triangulated_polygons.emplace_back(triangle);
			continue;
		}

		// Perform ear-clipping triangulation
		std::vector<Point> remaining_points = points;
		while (remaining_points.size() > 3) {
			size_t n = remaining_points.size();
			bool ear_found = false;

			for (size_t i = 0; i < n; ++i) {
				size_t prev = (i + n - 1) % n;
				size_t next = (i + 1) % n;

				Point& a = remaining_points[prev];
				Point& b = remaining_points[i];
				Point& c = remaining_points[next];

				// Check if the current triangle is an ear
				if (Polygon::isEar(a, b, c, remaining_points)) {
					// Create a new triangle
					Polygon triangle({ a, b, c });
					triangle.transformPointsToCoords();
					triangulated_polygons.emplace_back(triangle);

					// Remove the ear from the polygon
					remaining_points.erase(remaining_points.begin() + i);
					ear_found = true;
					break;
				}
			}

			// If no ear is found, break to avoid infinite loop
			if (!ear_found) {
				break;
			}
		}

		// Add the remaining triangle
		if (remaining_points.size() == 3) {
			Polygon triangle(remaining_points);
			triangle.transformPointsToCoords();
			triangulated_polygons.emplace_back(triangle);
		}
	}

	// Override the polygons vector with the triangulated polygons
	this->polygons = std::move(triangulated_polygons);
	this->polygon_amount = this->polygons.size();
}

bool FlowManager::border_neighbors(Node& node1, Node& node2) {
	Point node1_point = node1.getPoint();
	Point node2_point = node2.getPoint();

	if (node1_point.getX() == 0 || node1.getY() == 0)
	{
		if (node1_point.getX() == node2_point.getX() || node1_point.getY() == node2_point.getY())
			return true;	// same border neighbors
	}
	else if (node1_point.getX() == x_axis || node1_point.getY() == y_axis)
	{
		if (node1_point.getX() == node2_point.getX() || node1_point.getY() == node2_point.getY())
			return true;	// same border neighbors
	}
	return false;
}


bool FlowManager::check_neighbors(Node& pivot, Node& potential_node) {
	bool intersection = false;	// intersection condition
	Point potential_point = potential_node.getPoint();
	Point pivot_point = pivot.getPoint();

	// checking intersection between the pivot and potential_point line and other lines (go to both loops only if intersection = false)
	for (int inter_poly = 0; inter_poly < this->polygon_amount && !intersection; inter_poly++) {
		std::vector<Node>& temp_nodes = this->polygons[inter_poly].getCoords();
		size_t inter_poly_size = temp_nodes.size();

		for (int inter_node = 0; inter_node < inter_poly_size && !intersection; inter_node++) {
			if (!Point::equal(pivot_point, temp_nodes[inter_node].getPoint()) && !Point::equal(potential_point, temp_nodes[inter_node].getPoint()))
				if (!Point::equal(pivot_point, temp_nodes[(inter_node + 1) % inter_poly_size].getPoint()) && !Point::equal(potential_point, temp_nodes[(inter_node + 1) % inter_poly_size].getPoint()))
					intersection = Point::intersect(pivot_point, potential_point, temp_nodes[inter_node].getPoint(), temp_nodes[(inter_node + 1) % inter_poly_size].getPoint());
		}
	}

	return intersection;
}


void FlowManager::create_visibility_graph()
{
	int polygons_amount = this->polygon_amount;
	std::vector<Node*> start_point_neighbors;

	// Precompute all polygon points
	std::vector<Node*> all_points;
	for (int curr_poly = 0; curr_poly < polygons_amount; curr_poly++) {
		auto& coords = this->polygons[curr_poly].getCoords();
		for (auto& node : coords) {
			all_points.push_back(&node);
		}
	}

	// Helper function to compute the angle between two points
	auto compute_angle = [](const Node& from, const Node& to) -> double {
		return std::atan2(to.getY() - from.getY(), to.getX() - from.getX());
		};

	// Iterate through each polygon's points
	for (int curr_poly = 0; curr_poly < polygons_amount; curr_poly++) {
		auto& coords = this->polygons[curr_poly].getCoords();
		int curr_poly_size = coords.size();

		for (int curr_node_num = 0; curr_node_num < curr_poly_size; curr_node_num++) {
			Node& curr_node = coords[curr_node_num];
			std::vector<Node*> neighbors;

			// Sort all points based on the angle they make with the current node
			std::sort(all_points.begin(), all_points.end(),
				[&curr_node, &compute_angle](Node* a, Node* b) {
					return compute_angle(curr_node, *a) < compute_angle(curr_node, *b);
				});

			// Check visibility of each sorted point
			for (Node* potential_node : all_points) {
				if (potential_node == &curr_node) continue;

				// Exclude lines within the same polygon that are not adjacent
				bool is_same_polygon = false;
				if (potential_node >= &coords[0] && potential_node <= &coords[curr_poly_size - 1])
				{
					is_same_polygon = true;

					// Check if potential_node is adjacent to curr_node
					int potential_index = potential_node - &coords[0];
					if ((potential_index == (curr_node_num + 1) % curr_poly_size) ||
						(potential_index == (curr_node_num - 1 + curr_poly_size) % curr_poly_size)) {
						is_same_polygon = false; // Adjacent nodes are allowed
					}
				}

				if (is_same_polygon) continue;

				// Check if current node and potential node are border neighbors - if not check interceptions
				if (!border_neighbors(curr_node, *potential_node)) {
					// Check if the line between the current node and the potential node intersects any obstacle
					if (!check_neighbors(curr_node, *potential_node)) {
						neighbors.push_back(potential_node);
					}
				}
			}

			// Check visibility of start and end points
			if (!check_neighbors(this->start_point, curr_node)) {
				start_point_neighbors.push_back(&curr_node);
			}
			if (!check_neighbors(curr_node, this->end_point)) {
				neighbors.push_back(&(this->end_point));
			}

			curr_node.setNeighbors(neighbors);
		}
	}

	// Check if the start point is directly visible to the end point
	if (!check_neighbors(this->start_point, this->end_point)) {
		start_point_neighbors.push_back(&(this->end_point));
	}

	this->start_point.setNeighbors(start_point_neighbors);
}


std::vector<Node> FlowManager::reconstruct_path(std::unordered_map<Node*, Node*>& cameFrom) {
	std::vector<Node> total_path;
	Node* curr = &(this->end_point);
	total_path.push_back(*curr);

	// backtracking path till you get to the start point (started from end point)
	while (!Point::equal(curr->getPoint(), this->start_point.getPoint())) {
		curr = cameFrom[curr];
		total_path.push_back(*curr);
	}

	std::reverse(total_path.begin(), total_path.end());		// reverse path to start from start_point
	return total_path;
}


void FlowManager::shortest_path_a_star() 
{
	std::unordered_map<Node*, Node*> cameFrom;
	std::vector<Node*> openSet;

	// initializing first values
	this->start_point.set_gScore(0);
	this->start_point.set_fScore(Point::dist(this->start_point.getPoint(), this->end_point.getPoint()));
	openSet.push_back(&(this->start_point));

	// go trough each point in openSet (potential point to check shorter path from)
	while (!openSet.empty()) 
	{
		int min_f_index = Node::min_f_score(openSet);		// get the min f score index point from openSet
		Node* current = openSet[min_f_index];		// the min f score point in openSet

		if (Point::equal(current->getPoint(), this->end_point.getPoint())) 
		{		// if we got to the goal
			this->path = reconstruct_path(cameFrom);
			return;
		}

		openSet.erase(openSet.begin() + min_f_index);
		std::vector<Node*> neighbors = current->getNeighbors();
		size_t neighbor_amount = neighbors.size();

		//loop through current point neighbors
		for (int i = 0; i < neighbor_amount; i++) 
		{
			Node* neighbor = neighbors[i];

			// check if we found a cheaper path to the neighbor
			float temp_gScore = current->get_gScore() + Point::dist(current->getPoint(), neighbor->getPoint());
			if (temp_gScore < neighbor->get_gScore()) 
			{
				cameFrom[neighbor] = current;
				neighbor->set_gScore(temp_gScore);
				neighbor->set_fScore(temp_gScore + Point::dist(neighbor->getPoint(), (this->end_point.getPoint())));

				// if nieghbor is not in openSet - add it (via lambda function that uses the equal points function from Point class)
				auto it = std::find_if(openSet.begin(), openSet.end(), [&](Node* p) 
					{return Point::equal(p->getPoint(), neighbor->getPoint()); });
				if (it == openSet.end())
					openSet.push_back(neighbor);
			}
		}
	}
}


void FlowManager::shortest_path_rrt() {
	std::vector<Node*> tree; // Tree containing nodes
	tree.push_back(&(this->start_point)); // Initialize with the start point

	std::unordered_map<Node*, Node*> cameFrom; // Tracks the parent of each node

	auto is_valid_point = [&](const Point& point) -> bool {
		for (const auto& polygon : this->polygons) {
			if (polygon.containsPoint(point)) { // Assuming Polygon class has containsPoint()
				return false; // Point is inside an obstacle
			}
		}
		return true;
		};

	auto nearest_node = [&](const Point& random_point) -> Node* {
		Node* nearest = nullptr;
		float min_distance = std::numeric_limits<float>::max();
		for (Node* node : tree) {
			float distance = Point::dist(node->getPoint(), random_point);
			if (distance < min_distance) {
				min_distance = distance;
				nearest = node;
			}
		}
		return nearest;
		};

	while (true) {
		// Generate a random point
		float rand_x = static_cast<float>(rand()) / RAND_MAX * this->x_axis;
		float rand_y = static_cast<float>(rand()) / RAND_MAX * this->y_axis;
		Point random_point(rand_x, rand_y);

		if (!is_valid_point(random_point)) {
			continue; // Skip invalid points
		}

		// Find the nearest node in the tree
		Node* nearest = nearest_node(random_point);

		// Create a new node towards the random point
		Point nearest_point = nearest->getPoint();
		Point direction = random_point - nearest_point; // Vector from nearest to random
		float step_size = 1.0; // Adjust step size as needed
		direction.normalize(); // Normalize to unit vector
		Point new_point = nearest_point + direction * step_size;

		if (!is_valid_point(new_point)) {
			continue; // Skip if the new point is invalid
		}

		// Add new node to the tree
		Node* new_node = new Node(new_point);
		tree.push_back(new_node);
		cameFrom[new_node] = nearest;

		// Check if the new node is close to the end point
		if (Point::dist(new_point, this->end_point.getPoint()) <= step_size) {
			cameFrom[&(this->end_point)] = new_node;
			break; // Path found
		}
	}

	// Reconstruct the path from the RRT tree
	this->path = reconstruct_path(cameFrom);
}


void FlowManager::shortest_path_dijkstra() {
	std::unordered_map<Node*, Node*> cameFrom; // To reconstruct the path
	std::unordered_map<Node*, float> distances; // To store the shortest distance to each node
	std::priority_queue<std::pair<float, Node*>, std::vector<std::pair<float, Node*>>, std::greater<>> pq; // Min-heap

	// Initialize distances with infinity and start point distance to 0
	for (Polygon& polygon : this->polygons) {
		for (Node& node : polygon.getCoords()) {
			distances[&node] = std::numeric_limits<float>::infinity();
		}
	}
	distances[&(this->start_point)] = 0;
	distances[&(this->end_point)] = std::numeric_limits<float>::infinity(); // End point is unreachable
	pq.emplace(0, &(this->start_point));

	// Dijkstra's Algorithm
	while (!pq.empty()) {
		float current_distance = pq.top().first;
		Node* current_node = pq.top().second;
		pq.pop();

		// If we reached the end point, reconstruct and return the path
		if (Point::equal(current_node->getPoint(), this->end_point.getPoint())) {
			this->path = reconstruct_path(cameFrom);
			return;
		}

		// Iterate over neighbors of the current node
		for (Node* neighbor : current_node->getNeighbors()) {
			float new_distance = current_distance + Point::dist(current_node->getPoint(), neighbor->getPoint());
			if (new_distance < distances[neighbor]) {
				distances[neighbor] = new_distance;
				cameFrom[neighbor] = current_node;
				pq.emplace(new_distance, neighbor);
			}
		}
	}

	// If we exhaust the queue without finding a path
	throw std::runtime_error("No path found using Dijkstra's algorithm.");
}


void FlowManager::find_shortest_path() {
	switch (this->algorithm_type) {
	case 1:
		shortest_path_a_star();
		break;
	case 2:
		shortest_path_rrt();
		break;
	case 3:
		shortest_path_dijkstra();
		break;
	default:
		throw std::runtime_error("Invalid algorithm type.");
	}
}
