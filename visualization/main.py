import random
from FlowManager import FlowManager


# random map data section:
MAP_SIZE = 5000
POLYGON_AMOUNT = random.randint(5, 7)
STOPPING_POINTS = [2000, 2500]


if __name__ == '__main__':
    manager = FlowManager()

    # create map data and pass it to the C++ as input file
    manager.create_map(MAP_SIZE, POLYGON_AMOUNT)
    manager.create_cpp_input()

    # run C++ execution
    manager.find_path()

    # read C++ solution data and plot convexes new map
    manager.read_cpp_output()
    manager.show_map(with_dash=False, include_poly_dots=False, include_polygons=True)

    # plot solution path
    manager.read_and_plot_neighbors()
    manager.show_map(with_dash=False, include_poly_dots=False, include_polygons=True, include_path=True)
