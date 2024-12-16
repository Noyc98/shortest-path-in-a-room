#pragma once
#include "Point.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <limits>
#include <string>


/// @brief Class to represent a graph node object
/// @param point - The point that the Node represent (location in map)
/// @param g_score - The Node distance from start point (in the path) (for A* algorithm use)
/// @param f_score - The g_score value + dist(point, end_point) (for A* algorithm use)
/// @param neighbors - Array of pointers to Node objects in the map (neighbors of each node)
class Node {
public:

	Node() = default;

	Node(const Point p) {
		this->point = p;
	}

	// get value functions
	float getX() const { return this->point.x; }
	float getY() const { return this->point.y; }
	Point getPoint() const { return this->point; }
	float get_gScore() const { return this->g_score; }
	float get_fScore() const { return this->f_score; }
	std::vector<Node*> getNeighbors() const { return this->neighbors; }

	// set value functions
	void setPoint(float x, float y) {
		this->point.x = x;
		this->point.y = y;
	}
	void set_gScore(float val) { this->g_score = val; }
	void set_fScore(float val) { this->f_score = val; }
	void setNeighbors(const std::vector<Node*> nodes) { this->neighbors = nodes; }

	/// @brief Comparing the f scores between points and saves the minimum f score
	/// @param Points vector of points
	/// @return The vector index of the minimum f score point
	static int min_f_score(const std::vector<Node*>& nodes) {
		float minf = std::numeric_limits<float>::infinity();
		int min_f_index = 0;

		// search the min value index and saves it
		for (int i = 0; i < nodes.size(); i++) {
			if (nodes[i]->get_fScore() < minf) {
				minf = nodes[i]->get_fScore();
				min_f_index = i;
			}
		}
		return min_f_index;
	}

private:
	Point point = {};
	float g_score = std::numeric_limits<float>::infinity();
	float f_score = std::numeric_limits<float>::infinity();
	std::vector<Node*> neighbors{};
};