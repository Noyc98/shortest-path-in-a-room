#pragma once 
#include "FlowManager.h"
#include <filesystem>


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

		if (temp_polygon.getCoords().size() < 3) {
			return 1;		// not a valid convex created
		}

		this->polygons.push_back(temp_polygon);
	}
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

int FlowManager::create_convex_output() {

	std::ofstream output_file("polygons_hull.txt");

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

int FlowManager::create_neighbors_output() {

	std::ofstream output_file("neighbors.txt");
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


int FlowManager::create_path_output() {
	std::ofstream output_file("path.txt");

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

void FlowManager::find_neighbors() 
{
	int polygons_amount = this->polygon_amount;
	std::vector<Node*> start_point_neighbors;

	// loop for polygons
	for (int curr_poly = 0; curr_poly < polygons_amount; curr_poly++) {
		std::vector<Node>& coords = this->polygons[curr_poly].getCoords();
		int curr_poly_size = coords.size();

		// loop for points of the current polygon
		for (int curr_node_num = 0; curr_node_num < curr_poly_size; curr_node_num++) {
			Node& curr_node = coords[curr_node_num];
			std::vector<Node*> neighbors;
			bool start_intersect; // intersection condition for end_point
			bool end_intersect;   // intersection condition for end_point

			// check if there is an intersection between the curr_point - potential_point line to other lines (looping through potential polygons)
			for (int potential_poly_num = 0; potential_poly_num < polygons_amount; potential_poly_num++) {
				std::vector<Node>& petential_coords = this->polygons[potential_poly_num].getCoords();
				int pot_poly_size = petential_coords.size();
				
				// set prev and next points of curr_point to his neighbors
				if (curr_poly == potential_poly_num) {
					neighbors.push_back(&(coords[(curr_node_num + 1) % pot_poly_size]));
					int previous_index = ((((curr_node_num - 1) % pot_poly_size) + pot_poly_size) % pot_poly_size);
					neighbors.push_back(&(coords[previous_index]));  
				}
				else {
					// check if the potential point is a valid neighbor of curr_point (looping through the potential polygon points)
					for (int potential_node_num = 0; potential_node_num < pot_poly_size; potential_node_num++) {
						Node& potential_node = petential_coords[potential_node_num];
						bool intersection;	// intersection condition

						// check if there is an intersection between the curr_point - potential_point line to other lines
						intersection = check_neighbors(curr_node, potential_node);

						if (!intersection)
							neighbors.push_back(&potential_node);
					}
				}
			}

			// check if there is an intersection between the start_point - curr_point line to other lines
			start_intersect = check_neighbors(this->start_point, curr_node);
			if (!start_intersect)	// if not curr_point is a valid neighbor of start_point
				start_point_neighbors.push_back(&curr_node);

			// check if there is an intersection between the curr_point - end_point line to other lines
			end_intersect = check_neighbors(curr_node, this->end_point);
			if(!end_intersect)		// if not end_point is a valid neighbor of curr_point
				neighbors.push_back(&(this->end_point));

			coords[curr_node_num].setNeighbors(neighbors);
		}
	}
	//check if end point is a neighbor of start point (best route possible)
	if(!check_neighbors(this->start_point, this->end_point))
		start_point_neighbors.push_back(&(this->end_point));

	this->start_point.setNeighbors(start_point_neighbors);	// set start_point neighbors
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

void FlowManager::find_path() 
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