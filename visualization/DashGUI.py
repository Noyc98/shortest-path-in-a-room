import sys
import dash
import threading
import subprocess
from dash import dcc
from dash import html
import dash_mantine_components as dmc
import dash_bootstrap_components as dbc
from dash.dependencies import Input, Output, State
from dash.exceptions import PreventUpdate
from FlowManager import FlowManager
import plotly.express as px
import plotly.graph_objects as go
from Map import Map
import pandas as pd

manager = FlowManager()


def execution(map_size, poly_amount):

    # create map data and pass it to the C++ as input file
    map_thread = threading.Thread(target=manager.create_map(map_size, poly_amount))
    manager.create_cpp_input()

    # run C++ execution
    cpp_thread = threading.Thread(target=manager.find_path())

    # read C++ solution data and plot convexes new map
    manager.read_cpp_output()


def show_in_gui(include_path=False):
    fig = manager.show_map(with_dash=True, include_path=include_path)
    return fig


# Create Dash application
app = dash.Dash("EskimosiDash")

app.layout = html.Div(
    style={'background-color': '#9badbf'},

    children=[
        html.H1("Eskimosi Map GUI",
                id="welcome-msg",
                style={
                    'textAlign': 'center',
                    'fontSize': '45px'
                }),
        html.P('Create your own map and help the Eskimosi get to his wife!',
               style={
                    'textAlign': 'center',
                    'color': '#555555',
                    'marginTop': '-20px'
               }),
        dmc.Grid(children=[
            dmc.Col(span=2, children=[
                dmc.Stack([
                    html.Label("Map size: ", style={'font-weight': 'bold'}),
                    dcc.Input(id='map-size-input', type='number', placeholder="Enter a number"),
                    html.Br(),
                    html.Label("Polygon amount: ", style={'font-weight': 'bold'}),
                    dcc.Input(id='Polygon-amount-input', type='number', placeholder="Enter a number"),
                    dmc.Grid(children=[
                        dmc.Button('Apply', id='apply-button', n_clicks=0,
                                   style={
                                       'background-color': '#172d57'
                                   }),
                        dmc.Button('Find Path', id='find-path-button', n_clicks=0,
                                   style={
                                       'background-color': 'green'
                                   })
                    ]),
                ]),
            ]),
            dmc.Col(span=10, children=[
                dcc.Graph(
                    id='map',
                    style={'height': '870px', 'width': '100%'},
                    )
            ]),
        ]),
        html.Div(id='error-msg', style={'color': '#e01220', 'margin-top': '10px', 'font-size': '17px'}),
        html.Div(id='hidden_div'),
    ]
)


@app.callback(
    Output('map', 'figure', allow_duplicate=True),
    Input('apply-button', 'n_clicks'),
    State('map-size-input', 'value'),
    State('Polygon-amount-input', 'value'),
    prevent_initial_call=True
)
def get_map_data(apply_n_clicks, map_size, poly_amount):
    if apply_n_clicks == 0:
        raise PreventUpdate

    manager.Map = Map()  # clear old map data if exist
    execution(map_size, poly_amount)
    scatter_list = show_in_gui(include_path=False)
    fig = go.Figure(data=scatter_list)
    fig.update_layout(showlegend=False)
    return fig


@app.callback(
    Output('map', 'figure', allow_duplicate=True),
    Input('find-path-button', 'n_clicks'),
    Input('apply-button', 'n_clicks'),
    prevent_initial_call=True
)
def add_path(path_n_clicks, apply_n_clicks):
    if path_n_clicks == 0:
        raise PreventUpdate
    else:
        if path_n_clicks > 0 and apply_n_clicks == 0:
            display_error_msg(path_n_clicks, apply_n_clicks)
        else:
            scatter_list = show_in_gui(include_path=True)
            fig = go.Figure(data=scatter_list)
            fig.update_layout(showlegend=False)
            return fig


@app.callback(
    Output('error-msg', 'children'),
    Input('find-path-button', 'n_clicks'),
    Input('apply-button', 'n_clicks'),
)
def display_error_msg(path_n_clicks, apply_n_clicks):
    if path_n_clicks == 0 and apply_n_clicks == 0:
        raise PreventUpdate

    if path_n_clicks > 0 and apply_n_clicks == 0:
        return "Error: Apply a map before finding path!"


if __name__ == '__main__':
    app.run_server(debug=True)