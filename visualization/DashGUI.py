import dash
import threading
from dash import dcc, html
import dash_mantine_components as dmc
from dash.dependencies import Input, Output, State
from dash.exceptions import PreventUpdate
from FlowManager import FlowManager
import plotly.graph_objects as go
from Map import Map
from dash_mantine_components import MantineProvider
import os
os.environ['REACT_VERSION'] = '18.2.0'

manager = FlowManager()

def execution(map_size, poly_amount):
    threading.Thread(target=manager.create_map(map_size, poly_amount))
    manager.create_cpp_input()
    threading.Thread(target=manager.find_path())
    manager.read_cpp_output()

def show_in_gui(include_path=False):
    return manager.show_map(with_dash=True, include_path=include_path)


app = dash.Dash("ShortestPathDash")

if __name__ == '__main__':
    app.run_server(debug=True)