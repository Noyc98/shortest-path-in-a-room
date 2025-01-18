import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
from shapely.geometry import Polygon, MultiPolygon, box
from skimage import measure


def png_to_polygon(image_path, threshold=128):
    """
    Convert a PNG image to a non-convex polygon.

    Args:
        image_path (str): Path to the PNG image
        threshold (int): Threshold for binary image conversion (0-255)

    Returns:
        shapely.geometry.Polygon or shapely.geometry.MultiPolygon: The resulting polygon(s)
    """
    # Read image and convert to binary
    img = Image.open(image_path).convert('L')
    img_array = np.array(img)
    binary = img_array < threshold

    # Find contours
    contours = measure.find_contours(binary, 0.5)

    # Convert contours to polygons
    polygons = []
    for contour in contours:
        # Simplify contour if needed
        if len(contour) > 100:
            # Take every nth point to simplify
            contour = contour[::2]

        # Create polygon and fix orientation
        poly = Polygon(contour)
        if not poly.is_valid:
            poly = poly.buffer(0)
        polygons.append(poly)

    # Combine polygons if multiple are found
    if len(polygons) > 1:
        result = MultiPolygon(polygons)
    else:
        result = polygons[0]

    return result


def get_exterior_polygons(polygon: Polygon, bounds=(0, 0, 450, 600)):
    """
    Get the polygons representing the exterior regions around the main polygon.

    Args:
        polygon: shapely.geometry.Polygon
        bounds: tuple of (minx, miny, maxx, maxy) representing the plot boundaries

    Returns:
        list of shapely.geometry.Polygon: The exterior polygons
    """
    # height, width = img_shape
    minx, miny, maxx, maxy = bounds
    plot_bounds = box(minx, miny, maxx, maxy)

    # Buffer the polygon slightly to ensure proper difference operation
    polygon = polygon.buffer(0)

    # Get the difference between the plot bounds and the polygon
    exterior = plot_bounds.difference(polygon)

    # Ensure the result is valid
    if not exterior.is_valid:
        exterior = exterior.buffer(0)

    # Convert to list of polygons if result is MultiPolygon
    if isinstance(exterior, MultiPolygon):
        return [poly for poly in exterior.geoms if poly.area > 0]
    return [exterior] if exterior.area > 0 else []


def filter_and_save_xyz_file_as_png(input_file_path, z_threshold = 1.5):
    # Each line contains 6 values: x, y, z, r, g, b
    points = np.loadtxt(input_file_path, delimiter=' ', dtype=float)

    # Extract z values for filtering
    z = points[:, 2]

    # Filter points based on z-coordinate (e.g., to focus on the floor/ceiling)
    filtered_points = points[np.abs(z - np.min(z)) < z_threshold]

    # plot filtered points
    plt.plot(filtered_points[:, 0], filtered_points[:, 1], 'o', markersize=1)
    plt.savefig('../work/cloud_points.png')
    plt.show()


def png_to_polygons(file_path):
    # Load and process image
    main_polygon: MultiPolygon = png_to_polygon(file_path)

    # take the 3 polygon from the MultiPolygon
    interior_polygon: Polygon = Polygon()
    if isinstance(main_polygon, MultiPolygon):
        for i, poly in enumerate(main_polygon.geoms):
            if i == 2:
                interior_polygon = poly
                break

    # Get exterior polygons
    x_min, y_min, x_max, y_max = interior_polygon.bounds
    min_value = min(x_min, y_min) - 50
    max_value = max(x_max, y_max) + 50
    exterior_polygon_1 = get_exterior_polygons(interior_polygon, (0, 0, x_max-1, max_value))
    exterior_polygon_2 = get_exterior_polygons(interior_polygon, (x_max-1, 0, max_value, max_value))

    # convert to list of coordinates (list of 2)
    x, y = exterior_polygon_1[0].exterior.xy
    polygon_1 = [[cx, cy] for cx, cy in zip(x, y)]
    x, y = exterior_polygon_2[0].exterior.xy
    polygon_2 = [[cx, cy] for cx, cy in zip(x, y)]

    x, y = interior_polygon.exterior.xy
    polygon_3 = [[cx, cy] for cx, cy in zip(x, y)]


    return polygon_1, polygon_2, max_value, polygon_3


def xyz_to_polygons(input_file_path: str) -> tuple[list[list], int]:
    # Filter and save XYZ file as PNG
    filter_and_save_xyz_file_as_png(input_file_path)

    # Convert PNG to polygons
    exterior_polygons_1, exterior_polygons_2, max_value, interior_polygon = png_to_polygons('../work/cloud_points.png')

    return interior_polygon, max_value
