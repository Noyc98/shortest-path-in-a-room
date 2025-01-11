#pragma once
#include "Point.h"
#include "Node.h"
#include <vector>


/// @brief Class to represent a polygon object 
/// @param poly_number - Serial number of the polygon
/// @param coords - The coordinates array of the polygon
class Polygon {
public:

	// default constructor
	Polygon() = default;

	Polygon(const std::vector<Point>& pointsVector) {
		this->raw_points = pointsVector;	// set the raw points
	}

	void transform_to_convex_polygon();
	void transform_to_non_convex_polygon();

	// set values functions
	void setPolyId(const int num) {this->poly_id = num; }

	// get values funcions
	const int getPolyId() const {return this->poly_id; }
	std::vector<Node>& const getCoords() {return this->coords; }
	std::vector<Point>& const getRawPoints() {return this->raw_points; }
	void transformPointsToCoords(){ this->coords = convert_to_nodes(this->raw_points);  }
	static bool isEar(const Point& a, const Point& b, const Point& c, const std::vector<Point>& points);

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

	/// @brief Checks if a point is inside the polygon
	/// @param p The point to check
	/// @return True if the point is inside the polygon, false otherwise
	bool containsPoint(const Point& p) const;

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
	std::vector<Point> raw_points{};
};