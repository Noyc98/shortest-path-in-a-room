///
///@file Polygon.h
/// Polygon.cpp header file
///
#pragma once
#include "Point.h"
#include "Node.h"
#include <vector>
//#include <gmock/gmock.h>

/// @brief Class to represent a polygon object 
/// @param poly_number - Serial number of the polygon
/// @param coords - The coordinates array of the polygon
class Polygon {
public:

	// default constructor
	Polygon() = default;

	Polygon(const std::vector<Point>& pointsVector) {
		std::vector<Point> convex_points = grahamScan(pointsVector);	// create a convex
		this->coords = convert_to_nodes(convex_points);		// converting the convex points to nodes
	}

	// set values functions
	void setPolyId(const int num) {this->poly_id = num; }

	// get values funcions
	const int getPolyId() const {return this->poly_id; }
	std::vector<Node>& const getCoords() {return this->coords; }

	/// @brief Converting polygon shape to a convex shape
	/// @return The coordinates vector of the new convex
	std::vector<Point> grahamScan(std::vector<Point> pointsVector);
	
	/// @brief Converting Point object vector to Node object vector
	/// @param convex_points - The Points vector
	/// @return Nodes vector of the Points vector given
	std::vector<Node> convert_to_nodes(const std::vector<Point>& pointsVector);

	// string for polyogn printing
	std::string LOG_print() {
		std::string poly_str;

		poly_str = "convex points:\n";
		for (int j = 0; j < coords.size(); j++) {
			Point p = coords[j].getPoint();
			poly_str += p.LOG_print() + "\n";
		}
		return poly_str;
	}

private:
	/// @brief An angle compare func for sorting the points by their angles for graham scan use (using orientation func)
	/// @param pivot The convex pivot point
	/// @param p1 First point pointer of the line
	/// @param p2 Second point pointer of the line
	/// @return True if  p1 should be indexed in a lower index then p2 (counter_clockwise), false otherwise (clockwise)
	static bool compare_by_angle(const Point& pivot, const Point& p1, const Point& p2);

	// for accessing private func compare_by_angle in uni testing
	//FRIEND_TEST(MapTest, compare_by_angle);

	int poly_id = 0;
	std::vector<Node> coords{};
};