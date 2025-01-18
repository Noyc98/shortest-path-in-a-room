import numpy as np
import matplotlib.pyplot as plt
from Polygon import Polygon
import plotly.graph_objects as go
from xyz_to_polygons import xyz_to_polygons

class Map:
    def __init__(self):
        self.Polygons: list[Polygon] = []
        self.output_polygons: list[Polygon] = []
        self.map_size: int = 0
        self.num_of_polygons: int = 0
        self.start_point: list[int] = []
        self.end_point: list[int] = []
        self.shortest_path = np.array([], dtype=float).reshape((0, 2))

    # Func to create the map
    def generate_map(self, map_size, num_of_polygons):
        # generate polygon from point cloud
        polygon, map_size = xyz_to_polygons('../work/floor_1_amir/point_cloud.xyz')
        self.map_size = map_size

        # set start and end points in the map
        self.start_point = np.array([100, 200])
        self.end_point = np.array([330, 150])

        polygon1 = Polygon()
        polygon1.coords = np.array(polygon[::6])
        self.Polygons.append(polygon1)

        self.num_of_polygons = len(self.Polygons)

    # Func to plot visual map (each argument represent what included in the map output - True = include in output map)
    def plot_map(self, with_dash, include_poly_dots=False, include_polygons=False, include_path=False):
        if not with_dash:   # plot with plt
            # plot each section of the map
            if include_poly_dots or include_polygons:
                self.plot_polygons(include_poly_dots, include_polygons)
            if include_path:
                plt.plot(self.shortest_path[:, 0], self.shortest_path[:, 1], color='green')
            self.plotting_map()
        else:   # plot map with dash gui
            return self.plot_map_plotly_scatter(include_path)

    def plot_map_plotly_scatter(self, include_path):
        scatter_list = []

        start_point = go.Scatter(x=[self.start_point[0]], y=[self.start_point[1]], mode='markers+text',
                                 marker=dict(size=16, color='green'),
                                 text=["Eskimosi"],
                                 textposition='top center',
                                 textfont=dict(size=10, color='black'),
                                 hoverinfo='x+y'
                                 )
        end_point = go.Scatter(x=[self.end_point[0]], y=[self.end_point[1]], mode='markers+text',
                               marker=dict(size=16, color='purple'),
                               text=["Eskimosit"],
                               textposition='top center',
                               textfont=dict(size=10, color='black'),
                               hoverinfo='x+y'
                               )
        scatter_list.append(start_point)
        scatter_list.append(end_point)

        for i in range(len(self.Polygons)):
            poly = self.Polygons[i]
            x = poly.coords[:, 0]
            y = poly.coords[:, 1]

            # close each polygon start and end point (for the plot only)
            x = list(x) + [x[0]]
            y = list(y) + [y[0]]

            scatter_dots = go.Scatter(x=x, y=y, mode='markers', marker=dict(size=8, color='orange'), hoverinfo='x+y')
            scatter_line = go.Scatter(x=x, y=y, mode='lines', fill='toself', name=f"Polygon {i}",
                                      line=dict(color='#abd5eb'), fillcolor='#abd5eb',
                                      hoverinfo='text',
                                      text=[f"coordinates amount"])

            scatter_list.append(scatter_line)
            scatter_list.append(scatter_dots)

        if include_path:
            x = self.shortest_path[:, 0]
            y = self.shortest_path[:, 1]
            path_scatter = go.Scatter(x=x, y=y, mode='lines', line=dict(color='green'))
            scatter_list.append(path_scatter)

        return scatter_list

    # Func to plot the coordinates radius of each polygon by circles
    # args - (polygons array of the map)
    def plot_circles(self):
        try:
            # plot the circles of each polygon
            for poly in self.Polygons:
                center = poly.base
                radius = poly.radius

                # generating angles for the circle outline coordinates
                thetas = np.linspace(0, 2 * np.pi, 100)

                # converting from polar to cartesian
                x = center[0] + radius * (np.cos(thetas))
                y = center[1] + radius * (np.sin(thetas))
                plt.plot(x, y, color='grey')  # ploting grey circles in map

        except Exception as error:
            print(f"Circles plot error: {error}")

    # Func to plot the polygons based on the preferances given
    # args - (polygons - the map polygons, other args are booleans values to what we want to plot in map)
    def plot_polygons(self, include_dots, include_poly):
        try:
            # draw each polygon coordinates in the map
            for poly in self.Polygons:
                if include_dots:
                    plt.scatter(poly.coords[:, 0], poly.coords[:, 1])
                if include_poly:
                    random_color = 'grey' # np.random.rand(3,)
                    plt.plot(poly.coords[:, 0], poly.coords[:, 1], color=random_color)
                    plt.plot([poly.coords[-1, 0], poly.coords[0, 0]], [poly.coords[-1, 1], poly.coords[0, 1]],
                             color='grey')
                    plt.fill(poly.coords[:, 0], poly.coords[:, 1], color=random_color)

            # for the first plot - to check if random polygons generated are not overlapping
            # include_poly = false because there are no convexes yet (before C++ run)
            if include_dots and not include_poly:
                self.plot_circles()

        except Exception as error:
            print(f"Polygon plot error: {error}")


    def plotting_map(self):
        try:
            # plot path points
            plt.scatter(self.start_point[0], self.start_point[1], color='green', s=80)
            plt.text(self.start_point[0], self.start_point[1], '', ha='left', va='bottom')
            plt.scatter(self.end_point[0], self.end_point[1], color='purple', s=80)
            plt.text(self.end_point[0], self.end_point[1], '', ha='left', va='bottom')

            # Draw the boundary manually using lines
            plt.plot([0, self.map_size], [0, 0], color='gray', linewidth=6)  # Bottom border
            plt.plot([self.map_size, self.map_size], [0, self.map_size], color='gray', linewidth=6)  # Right border
            plt.plot([self.map_size, 0], [self.map_size, self.map_size], color='gray', linewidth=6)  # Top border
            plt.plot([0, 0], [self.map_size, 0], color='gray', linewidth=6)  # Left border

            # define axis size of the map
            plt.figure(1)
            plt.xlim(0, self.map_size)
            plt.ylim(0, self.map_size)

            # map labels\titles
            plt.xlabel('X-Axis')
            plt.ylabel('Y-Axis')
            plt.title('Plygons Map')

            plt.grid()  # draw lines in map
            plt.show()

        except Exception as error:
            print(f"Map plot error: {error}")

