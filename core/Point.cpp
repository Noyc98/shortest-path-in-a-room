#pragma once
#include "Point.h"


Point::Point(float x, float y) {
	this->x = x;
	this->y = y;
}


void Point::setPoint(float x, float y) {
	this->x = x;
	this->y = y;
}


float Point::getX() const {
	return this->x;
}


float Point::getY() const {
	return this->y;
}


bool Point::equal(const Point& p1, const Point& p2) {
	if (p1.x == p2.x && p1.y == p2.y)	// comapare values
		return true;
	return false;
}


float Point::dist(const Point& p1, const Point& p2) {
	return static_cast<float>(std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2)));
}


Direction Point::orientation(const Point& a, const Point& b, const Point& c) {
	float val = ((b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y));
	if (val == 0) return collinear;		// same angle (collinear)
	return val < 0 ? counter_clockwise : clockwise;
}


bool Point::intersect(const Point& p1, const Point& p2, const Point& p3, const Point& p4) {
	// set the angles directions between the lines
	Direction o1 = orientation(p1, p2, p3);
	Direction o2 = orientation(p1, p2, p4);
	Direction o3 = orientation(p3, p4, p1);
	Direction o4 = orientation(p3, p4, p2);

	// classic intersection (cross between lines)
	if (o1 != o2 && o3 != o4)
		return true;

	return false;
}

void Point::normalize() {
	float magnitude = std::sqrt(x * x + y * y);
	if (magnitude > 0.0f) {
		x /= magnitude;
		y /= magnitude;
	}
}

// implement dot product
float Point::dot(const Point& other) const {
	return x * other.x + y * other.y;
}



bool Point::isPointInsideTriangle(const Point& a, const Point& b, const Point& c, const Point& p) {
	// Compute vectors        
	Point v0 = c - a;
	Point v1 = b - a;
	Point v2 = p - a;

	// Compute dot products
	float dot00 = v0.dot(v0);
	float dot01 = v0.dot(v1);
	float dot02 = v0.dot(v2);
	float dot11 = v1.dot(v1);
	float dot12 = v1.dot(v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}


Point Point::operator-(const Point& other) const {
	return Point(x - other.x, y - other.y);
}

Point Point::operator*(float scalar) const {
	return Point(this->x * scalar, this->y * scalar);
}

Point Point::operator+(const Point& other) const {
	return Point(x + other.x, y + other.y);
}

bool Point::operator==(const Point& other) const {
	return (this->x == other.x && this->y == other.y);
}