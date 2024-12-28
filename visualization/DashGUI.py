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

app.layout = dmc.MantineProvider(
    theme={'colorScheme': 'light'},
    children=html.Div(
        style={'background-color': '#D1E5F4'},
        children=[
            html.H1("Shortest Path", id="welcome-msg", style={'textAlign': 'center', 'fontSize': '45px'}),
            html.P('Create your own map and find the shortest path!',
                   style={'textAlign': 'center', 'color': '#555555', 'marginTop': '-20px'}),
            dmc.Grid(children=[
                dmc.GridCol(span=2, children=[
                    dmc.Stack([
                        html.Label("Map size: ", style={'font-weight': 'bold'}),
                        dcc.Input(id='map-size-input', type='number', placeholder="Enter a number"),
                        html.Br(),
                        html.Label("Polygon amount: ", style={'font-weight': 'bold'}),
                        dcc.Input(id='Polygon-amount-input', type='number', placeholder="Enter a number"),
                        dmc.Grid(children=[
                            dmc.Button('Apply', id='apply-button', n_clicks=0, style={'background-color': '#172d57'}),
                            dmc.Button('Find Path', id='find-path-button', n_clicks=0,
                                       style={'background-color': 'green'})
                    ]),
                ]),
            ]),
            dmc.GridCol(span=10, children=[
                dcc.Graph(id='map', style={'height': '870px', 'width': '100%'})
            ]),
        ]),
        html.Div(id='error-msg', style={'color': '#e01220', 'margin-top': '10px', 'font-size': '17px'}),
        html.Div(id='hidden_div'),
    ])
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
    manager.Map = Map()
    execution(map_size, poly_amount)
    scatter_list = show_in_gui(include_path=False)
    return go.Figure(data=scatter_list).update_layout(showlegend=False)


@app.callback(
    Output('map', 'figure', allow_duplicate=True),
    Input('find-path-button', 'n_clicks'),
    Input('apply-button', 'n_clicks'),
    prevent_initial_call=True
)
def add_path(path_n_clicks, apply_n_clicks):
    if path_n_clicks == 0:
        raise PreventUpdate
    if path_n_clicks > 0 and apply_n_clicks == 0:
        display_error_msg(path_n_clicks, apply_n_clicks)
    scatter_list = show_in_gui(include_path=True)
    return go.Figure(data=scatter_list).update_layout(showlegend=False)

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