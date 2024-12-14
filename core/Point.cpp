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