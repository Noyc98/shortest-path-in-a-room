import numpy as np
import random
import time


class Polygon:
    def __init__(self):
        self.base = 0
        self.radius = 0
        self.coords = 0

    # Func to set the base coord of the poly
    # args - (limit - map range, polygons - polygons array of the map)
    def set_poly_base(self, limit, polygons):
        # True = polygons overlapping is possible
        start_point = np.array([0, 0])
        end_point = np.array([limit, limit])
        overlap = True
        start_time = time.time()

        # generate random data for polygon until no overlap possible (values decided by percentage of the map size)
        while overlap:
            x = random.uniform(0, limit)
            y = random.uniform(0, limit)
            point = np.array([x, y])

            # polygon point cant be the start or the end point
            if not (np.array_equal(point, start_point)) or (np.array_equal(point, end_point)):
                self.base = np.array([x, y])
                self.radius = random.uniform(0.07*limit, 0.15*limit)     # adjust radius size here
                overlap = self.polygons_overlap(polygons)
                if (time.time() - start_time) > 1.7:
                    break
        return overlap

    # Boolean func to check overlapping possibility between polygons
    # args - (polygons - polygons array of the map)
    def polygons_overlap(self, polygons):
        # for each polygon in the map - check if their radius range is overlapping with the new one(self)
        for poly in polygons:
            if self.circle_overlap(poly.radius, poly.base):
                return True
        return False

    # Func to check overlapping possibility between polygons
    # args - (limit - limit of the map range, polygons - polygons array of the map)
    def gen_polygon(self, limit, polygons):
        # randomly generate values
        overlap = self.set_poly_base(limit, polygons)
        polygon_coords = np.array([self.base])
        num_of_poly_coords = random.randint(20, 50)

        # create the amount of coords for the plygon
        for i in range(num_of_poly_coords):
            while True:
                # choose random degree in circle and create polar coordinate values
                theta = random.uniform(0, 2 * np.pi)
                r = random.uniform(0, self.radius)

                # convert polar values to cartesian values
                x = self.base[0] + r * np.cos(theta)
                y = self.base[1] + r * np.sin(theta)
                if not ((y < 0 or y > limit) or (x < 0 or x > limit)):
                    break

            if not overlap:
                rand_coord = np.array([x, y])
                polygon_coords = np.append(polygon_coords, [rand_coord], axis=0)

        self.coords = polygon_coords
        return overlap

    # Func to check if there is a polygons overlap possibility with self polygon circle
    # args - (poly_radius - given polygon radiuse, poly_base - given polygons center base)
    def circle_overlap(self, poly_radius, poly_base):
        center_dist = np.linalg.norm(np.array(self.base) - np.array(poly_base))  # distance from circles centers
        r_sum = self.radius + poly_radius
        return center_dist < r_sum  # True if there is an overlap chance
