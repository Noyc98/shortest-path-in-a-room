#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <limits>
#include <string>


/// @brief Enumeration for angle direction between 2 connected lines
enum Direction {
	collinear = 0,
	clockwise = 1,
	counter_clockwise = 2
};


/* @brief Struct to represent a coordinate object
	@param x - x value of coordinate
	@param y - y value of coordinate
*/
struct Point {
	float x = 0.0;
	float y = 0.0;

	/// @brief Default constructor
	Point() = default;

	/// @brief Constructor
	/// @param x x value of coordinate
	/// @param y y value of coordinate
	/// @return Point object
	
	Point(float x, float y);

	void setPoint(float x, float y);
	float getX() const;
	float getY() const;

	/// @brief Checking if there is a crossing intersection between 2 lines
	/// @param p1 First point in first line
	/// @param p2 Second point in first line
	/// @param p3 First point in second line
	/// @param p4 Second point in second line
	/// @return True if there is an intersection
	static bool intersect(const Point& p1, const Point& p2, const Point& p3, const Point& p4);

	/// @brief Calculate squere distance between 2 points
	/// @param p1 First point
	/// @param p2 Second point
	/// @return The squere distance between the points
	static float dist(const Point& p1, const Point& p2);

	/// @brief Checking if the x and y values of 2 points are equal
	/// @param p1 First point
	/// @param p2 Second point
	/// @return True if points are equal
	static bool equal(const Point& p1, const Point& p2);

	/// @brief Checking the direction of the angle between 2 connected lines
	/// @param a Fisrt point in first line
	/// @param b Second point in fist line, and first point in second line
	/// @param c Third point
	/// @return 0 If the lines are collinear, 2 if there is a counterclockwise turn, 1 if there is a counterclock turn
	static Direction orientation(const Point& a, const Point& b, const Point& c);

	/// @brief Normalizes the point (treating it as a vector)
	void normalize();

	// dot product function
	float dot(const Point& other) const;

	static bool isPointInsideTriangle(const Point& a, const Point& b, const Point& c, const Point& p);

	/// @brief Subtraction operator for vector subtraction
	/// @param other Another point
	/// @return Resulting point after subtraction
	Point operator-(const Point& other) const;

	// Overloaded * operator for scalar multiplication
	Point operator*(float scalar) const;

	// Overloaded + operator for 2 points
	Point operator+(const Point& other) const;

	// Overloaded == operator for 2 points
	bool operator==(const Point& other) const;

	// string for Point printing
	std::string LOG_print() {
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
};

