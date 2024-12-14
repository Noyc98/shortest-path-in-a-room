import random
import threading
from FlowManager import FlowManager


# random map data section:
MAP_SIZE = 5000 # random.randint(1000, 10000)
POLY_AMOUNT = random.randint(5, 7)    # max amouont possible: 20-30 polygons of 7%-15% of map size
# (if you want more - adjust the polygons radius size in func set_poly_base in class Polygon to a smaller percent)

if __name__ == '__main__':
    manager = FlowManager()

    # create map data and pass it to the C++ as input file
    map_thread = threading.Thread(target=manager.create_map(MAP_SIZE, POLY_AMOUNT))
    manager.create_cpp_input()
    manager.show_map(with_dash=False, include_poly_dots=True)

    # run C++ execution
    cpp_thread = threading.Thread(target=manager.find_path())

    # read C++ solution data and plot convexes new map
    manager.read_cpp_output()
    manager.show_map(with_dash=False, include_poly_dots=True, include_polygons=True)

    # plot solution path
    manager.read_and_plot_neighbors()
    manager.show_map(with_dash=False, include_poly_dots=False, include_polygons=True, include_path=True)
