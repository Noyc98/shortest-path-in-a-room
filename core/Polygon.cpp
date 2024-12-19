#pragma once 
#include "Polygon.h"
#include <vector>
#include <algorithm>

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