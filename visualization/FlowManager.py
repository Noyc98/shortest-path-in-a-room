import numpy as np
import matplotlib.pyplot as plt
import os
import subprocess
from Polygon import Polygon
from Map import Map


# file names:
MAP_INPUT_FILE = 'map_input.txt'
OUTPUT_FILE = 'polygons_hull.txt'
NEIGHBORS_FILE = 'neighbors.txt'
PATH_FILE = 'path.txt'

# Set the working directory (adjust this path based on your structure)
WORK_DIR = os.path.join(os.path.dirname(__file__), '..', 'work')
os.makedirs(WORK_DIR, exist_ok=True)  # Create the directory if it doesn't exist
os.chdir(WORK_DIR)  # Change the current working directory to the specified folder

class FlowManager:
    def __init__(self):
        self.Map = Map()

    # Func to create map data
    def create_map(self, map_size, num_of_polygons):
        self.Map.generate_map(map_size, num_of_polygons)

    # Func to show the map on screen
    def show_map(self, with_dash=False, include_poly_dots=False, include_polygons=False, include_path=False):
        return self.Map.plot_map(with_dash, include_poly_dots, include_polygons, include_path)

    # Func to run C++ executable file
    def find_path(self):
        # Initialize C++ .exe file path
        cpp_execute_file_path = '../core/x64/Debug/core.exe'

        # Run the C++ execution and wait for it to complete before continuing or print error if failed
        try:
            subprocess.run([cpp_execute_file_path], check=True)
        except FileNotFoundError:
            print(f"Error: {cpp_execute_file_path} not found.")
        except subprocess.CalledProcessError as e:
            print(f"Sub process running error: {e}")
        except Exception as e:
            print(f"An unexpected error occurred: {e}")

        # Safety loop to wait for the C++ relevant files creation before continuing
        while not os.path.exists(OUTPUT_FILE) or not os.path.exists(PATH_FILE) or not os.path.exists(NEIGHBORS_FILE):
            continue

        self.Map.shortest_path = self.read_path()

    # Func to read the C++ path output file
    def read_path(self):
        # The output format - "x y\n" for each coordinate of the path
        file = open(PATH_FILE, 'r')
        path_coordinates = np.array([], dtype=float).reshape((0, 2))
        for line in file:
            if line.strip() != '':
                words = line.strip().split()
                word1, word2 = float(words[0]), float(words[1])
                point = np.array([word1, word2])
                path_coordinates = np.append(path_coordinates, [point], axis=0)
        return path_coordinates

    # Func to create map input file for C++ execution
    def create_cpp_input(self, algorithm_type='A*'):
        try:
            mode = 'w' if os.path.exists(MAP_INPUT_FILE) else 'a'  # check if file exists already
            file = open(MAP_INPUT_FILE, mode)
            # write the base data
            file.write(str(self.Map.map_size) + '\n' + str(self.Map.map_size) + '\n')
            file.write(' '.join(map(str, self.Map.start_point)) + '\n')
            file.write(' '.join(map(str, self.Map.end_point)) + '\n')
            file.write(str(len(self.Map.Polygons)) + '\n')

            index = 1
            # write polygons coordinates data
            for poly in self.Map.Polygons:
                file.write(str(index) + '\n')
                file.write(str(len(poly.coords)) + '\n')
                for coord in poly.coords:
                    file.write(' '.join(map(str, coord)) + '\n')
                index += 1

            if algorithm_type == 'A*':
                file.write('1\n')
            elif algorithm_type == 'rrt':
                file.write('2\n')
            else:
                file.write('3\n')

            print("C++ input file successfully created.")

        except Exception as error:
            print(f"File input creation error: {error}")

    # Func to read the C++ output files and store it in the relevant data members of the class
    def read_cpp_output(self):
        # The output format - "x y\n" for each coordinate and empty lines between polygons
        file = open(OUTPUT_FILE, 'r')       # open and read the file
        new_polygons = []
        new_poly = Polygon()
        poly_hull = np.array([], dtype=float).reshape((0, 2))

        # iterate each line in file and read it
        for line in file:
            if line.strip() != '':      # empty lines means new polygon
                words = line.strip().split()
                word1, word2 = float(words[0]), float(words[1])
                point = np.array([word1, word2])    # word1 = x val, word2 = y val
                poly_hull = np.append(poly_hull, [point], axis=0)
            else:
                new_poly.coords = poly_hull
                new_polygons.append(new_poly)
                poly_hull = np.array([], dtype=float).reshape((0, 2))
                new_poly = Polygon()
        self.Map.Polygons = new_polygons

    # Func to read C++ neighbors file and plotting it - FOR SELF USE ONLY! (not affecting the final output of the path)
    def read_and_plot_visibility_graph(self):
        # The output format - the first coord is the main coord
        # and "x y\n" for each coordinate (empty lines between each main coordinate)
        file = open(NEIGHBORS_FILE, 'r')
        poly_neighbors = np.array([], dtype=float).reshape((0, 2))
        first = True
        line = file.readline()
        curr = np.array([], dtype=float).reshape((0, 2))

        while line.strip() != '':
            words = line.strip().split()
            word1, word2 = float(words[0]), float(words[1])
            point = np.array([word1, word2])
            if first:
                plt.scatter(point[0], point[1], color='orange')
                curr = point
                first = False
            else:
                poly_neighbors = np.append(poly_neighbors, [point], axis=0)
            line = file.readline()

        for neighbor in poly_neighbors:
            plt.plot([curr[0], neighbor[0]], [curr[1], neighbor[1]], color='red')
        # self.show_map(include_polygons=True)
        poly_neighbors = np.array([], dtype=float).reshape((0, 2))
        first = True

        for line in file:
            if line.strip() != '':
                words = line.strip().split()
                word1, word2 = float(words[0]), float(words[1])
                point = np.array([word1, word2])
                if first:
                    plt.scatter(point[0], point[1], color='orange')
                    curr = point
                    first = False
                else:
                    poly_neighbors = np.append(poly_neighbors, [point], axis=0)
            else:
                for neighbor in poly_neighbors:
                    # plot it in front of everything color red
                    plt.plot([curr[0], neighbor[0]], [curr[1], neighbor[1]], color='red')
                poly_neighbors = np.array([], dtype=float).reshape((0, 2))
                first = True
        self.show_map(include_polygons=True)
