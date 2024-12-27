#pragma once 
#include "Polygon.h"
#include <vector>
#include <algorithm>


void Polygon::transform_to_convex_polygon() {
	std::vector<Point> convex_points = grahamScan(this->raw_points);	// create a convex
	this->coords = convert_to_nodes(convex_points);		// converting the convex points to nodes
}


bool Polygon::compare_by_angle(const Point& pivot, const Point& p1, const Point& p2) {
	Direction direction = Point::orientation(pivot, p1, p2);
	if (direction == counter_clockwise) {
		return true;
	}
	if (direction == clockwise) {
		return false;
	}
	if (direction == collinear) {
		(Point::dist(pivot, p2) >= Point::dist(pivot, p1)) ? true : false;
	}
}

bool Polygon::isEar(const Point& a, const Point& b, const Point& c, const std::vector<Point>& points) {
	// Check if the triangle is convex
	if (Point::orientation(a, b, c) != 2) {
		return false;
	}

	// Check if any other point in the polygon lies inside the triangle
	for (const Point& p : points) {
		if (p == a || p == b || p == c) {
			continue; // Skip vertices of the triangle
		}
		if (Point::isPointInsideTriangle(a, b, c, p)) {
			return false; // Point inside triangle, not an ear
		}
	}

	// No points inside and triangle is convex
	return true;
}


std::vector<Point> Polygon::grahamScan(std::vector<Point> pointsVector) {
	std::vector<Point> convex_points;
	size_t n = pointsVector.size();
	float minY = pointsVector[0].y;
	int min_index = 0;
	convex_points.reserve(n);	// max of n points possible for each convex (points amount in polygon)

	// search the min y coordinate (or min y and x if there are few min y's)
	for (int i = 1; i < n; i++) {
		float newY = pointsVector[i].y;

		if ((newY < minY) || (newY == minY) && (pointsVector[i].x < pointsVector[min_index].x)) {
			minY = newY;
			min_index = i;
		}
	}

	//sorting the points by their angel according to the pivot point
	std::swap(pointsVector[0], pointsVector[min_index]);
	Point pivot = pointsVector[0];
	std::sort(std::next(pointsVector.begin(), 1), pointsVector.end(), [pivot](const Point& p1, const Point& p2) {
		return compare_by_angle(pivot, p1, p2); });

	int arrSize = 1;
	// remove points with the same angle (duplication)
	for (int i = 0; i < n; i++) {
		// when the angle of the (i)th and (i+1)th elements are the same, remove points
		while (i < n - 1 && Point::orientation(pivot, pointsVector[i], pointsVector[i + 1]) == 0)
			i++;
		pointsVector[arrSize] = pointsVector[i];
		arrSize++;
	}

	convex_points.push_back(pointsVector[0]);
	convex_points.push_back(pointsVector[1]);
	convex_points.push_back(pointsVector[2]);

	//check clock turns for remaining points - when last, before last and point[i] making left turn, remove point
	for (size_t i = 3; i < arrSize; i++) {
		while (convex_points.size() > 1 && Point::orientation(convex_points[convex_points.size() - 2], convex_points[convex_points.size() - 1], pointsVector[i]) != 2)
			convex_points.pop_back();
		convex_points.push_back(pointsVector[i]);
	}

	return convex_points;
}


std::vector<Node> Polygon::convert_to_nodes(const std::vector<Point>& pointsVector) {
	std::vector<Node> nodesVector = {};

	for (int i = 0; i < pointsVector.size(); i++) {
		Point point = pointsVector[i];
		Node node(point);
		nodesVector.push_back(node);
	}

	return nodesVector;
}


bool Polygon::containsPoint(const Point& p) const {
	int n = this->coords.size();
	if (n < 3) {
		return false; // A polygon needs at least 3 vertices
	}

	// Ray-casting algorithm: Count how many times a ray from the point crosses polygon edges
	int count = 0;
	Point rayEnd(std::numeric_limits<float>::max(), p.y); // Ray extending infinitely to the right

	for (int i = 0; i < n; ++i) {
		Point v1 = coords[i].getPoint();
		Point v2 = coords[(i + 1) % n].getPoint(); // Next vertex (loop back at the end)

		// Check if the ray intersects with the edge (v1, v2)
		if (Point::intersect(p, rayEnd, v1, v2)) {
			// Check if the point lies exactly on an edge
			if (Point::orientation(v1, p, v2) == collinear &&
				p.x >= std::min(v1.x, v2.x) && p.x <= std::max(v1.x, v2.x) &&
				p.y >= std::min(v1.y, v2.y) && p.y <= std::max(v1.y, v2.y)) {
				return true; // Point is on the edge
			}
			count++;
		}
	}

	// Point is inside if the ray intersects an odd number of edges
	return (count % 2 == 1);
}
