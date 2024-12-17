import random
from FlowManager import FlowManager


# random map data section:
MAP_SIZE = 5000
POLYGON_AMOUNT = random.randint(5, 7)
STOPPING_POINTS = [2000, 2500]


if __name__ == '__main__':
    manager = FlowManager()

    # create map data and plot it
    manager.create_map(MAP_SIZE, POLYGON_AMOUNT)
    manager.show_map(with_dash=False, include_poly_dots=False, include_polygons=True)

    # C++ execution
    manager.create_cpp_input()
    manager.find_path()
    manager.read_cpp_output()

    # Plot output (visibility graph + path)
    manager.read_and_plot_visibility_graph()
    manager.show_map(with_dash=False, include_poly_dots=False, include_polygons=True, include_path=True)
