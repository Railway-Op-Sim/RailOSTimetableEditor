import PySimpleGUI as psg
import typing
import datetime
import pandas
import railostools.ttb.components as railos_ttb_comp


def _times_of_day() -> typing.Iterator[str]:
    for time in range(0, 431964, 60):
        yield datetime.datetime.fromtimestamp(time).strftime("%H:%M")


def _build_menu_bar() -> psg.MenuBar:
    _menu_bar_definition: typing.List = [
        [
            "&File",
            [
                "New",
                "&Save  Ctrl-S",
                "Save &As",
                "&Open",
                ["Timetable", "Git Repository"],
            ],
        ],
        ["&Help", ["About", "Documentation"]],
        ["&Edit", ["Add Consist"]],
    ]
    return psg.MenuBar(_menu_bar_definition)


def _build_setup_buttons() -> typing.List[typing.List[psg.Element]]:
    return [
        [
            psg.Button(
                button_text="Set RailOS Location",
                tooltip="Locate Railway Operation Simulator installation",
                key="RAILOS_LOC_BROWSER",
            ),
            psg.Text(
                "Railway Operation Simulator Location Unset",
                text_color="red",
                key="RAILOS_LOC",
            ),
            psg.Button(
                button_text="Route", tooltip="Select route file", key="ROUTE_BROWSER"
            ),
            psg.Text("Route Map Not Specified", text_color="red", key="ROUTE_CHOICE"),
        ]
    ]


def _build_service_display_table(service_entries:  typing.List[typing.List[typing.Any]]) -> psg.Table:
    return psg.Table(
        values=service_entries,
        headings=["Arrive", "Depart", "Location"],
        justification="left",
        key="SERVICE_DISPLAY",
        auto_size_columns=False,
        enable_events=True,
        col_widths=[10,10,30]
    )


def _build_timetable_display_table(timetable_entries:  typing.List[typing.List[typing.Any]]) -> psg.Table:
    return psg.Table(
        values=timetable_entries,
        headings=["Headcode", "Time", "Description"],
        justification="left",
        key="TIMETABLE_DISPLAY",
        enable_events=True,
        auto_size_columns=False,
        col_widths=[10,10,30]
    )


def _build_start_time_spinbox() -> typing.Tuple[psg.Text, psg.Spin]:
    return (
        psg.Text("Start Time"),
        psg.Spin(
            values=list(_times_of_day()),
            tooltip="Service start time",
            key="SRV_START_TIME",
            size=(10, 1),
        ),
    )


def _build_unit_select() -> psg.Radio:
    return psg.Column(
        [
            [psg.Radio("kph", "UNIT_SELECT", default=True)],
            [psg.Radio("mph", "UNIT_SELECT")],
        ]
    )


def _build_spinbox_spec(
    specification: str, unit: str
) -> typing.Tuple[psg.Text, psg.Spin]:
    return (
        psg.Text(f"{specification.title()} ({unit})"),
        psg.Spin(
            values=list(range(999)),
            disabled=True,
            tooltip=f"Set service {specification} value in {unit}",
            key=f"SRV_{specification.replace(' ', '_').upper()}",
            size=(5, 1),
        ),
    )


def _build_specs_layout() -> psg.Column:
    _spinboxes: typing.List[typing.Tuple[psg.Text, psg.Spin]] = [
        _build_spinbox_spec("Start Speed", "kph"),
        _build_spinbox_spec("Max Speed", "kph"),
        _build_spinbox_spec("Mass", "te"),
        _build_spinbox_spec("Max Brake Force", "te"),
        _build_spinbox_spec("Max Power", "kW"),
    ]

    return psg.Column(
        [
            [*_build_start_time_spinbox()],
            [
                psg.Column([[spinbox[0]] for spinbox in _spinboxes]),
                psg.Column([[spinbox[1]] for spinbox in _spinboxes]),
            ],
        ],
        justification="left",
    )


def _build_train_consist_panel() -> psg.Column:
    return psg.Column(
        [
            [
                psg.Column(
                    [
                        [psg.Text("From Train Template")],
                        [psg.DropDown([], key="CONSIST_TEMPLATES", size=(25, 1))],
                    ],
                    vertical_alignment="top",
                ),
                psg.Column(
                    [
                        [psg.Text("In Multiple")],
                        [
                            psg.Spin(
                                values=list(range(1, 6)), size=(8, 1), initial_value=1
                            )
                        ],
                    ],
                    vertical_alignment="top",
                ),
            ]
        ],
        vertical_alignment="top",
    )


def _build_ref_desc_boxes() -> psg.Column:
    return psg.Column(
        [
            [psg.Text("Service Reference")],
            [psg.Input("", key="SERVICE_REF", size=(20, 1))],
            [psg.Text("Description")],
            [psg.Input("", key="SERVICE_DESC", size=(40, 1))],
        ],
        vertical_alignment="top",
    )


def _build_service_type_select() -> psg.Column:
    return psg.Column(
        [
            [psg.Radio("Standard", "RADIO_SRV_SELECT", default=True)],
            [psg.Radio("Shuttle from Stop", "RADIO_SRV_SELECT")],
            [
                psg.Radio(
                    "Shuttle from Feeder Service", "RADIO_SRV_SELECT", disabled=True
                )
            ],
            [psg.Radio("Shuttle Finish Service", "RADIO_SRV_SELECT", disabled=True)],
            [
                psg.Radio(
                    "Shuttle from Other Service", "RADIO_SRV_SELECT", disabled=True
                )
            ],
        ],
        vertical_alignment="top",
    )


def _build_finish_type_select() -> psg.Column:
    return psg.Column(
        [
            [psg.Radio("Finish and Form New Service", "RADIO_SRV_FINISH_SELECT")],
            [
                psg.Radio(
                    "Finish and Remain at Location",
                    "RADIO_SRV_FINISH_SELECT",
                    default=True,
                )
            ],
            [psg.Radio("Finish and Exit at Location", "RADIO_SRV_FINISH_SELECT")],
            [psg.Radio("Finish and Join Other", "RADIO_SRV_FINISH_SELECT")],
            [psg.Radio("Finish and Shuttle Feeder", "RADIO_SRV_FINISH_SELECT")],
            [psg.Radio("Finish Shuttle, Form New", "RADIO_SRV_FINISH_SELECT")],
        ],
        vertical_alignment="top",
    )


def _build_location_select() -> psg.Column:
    return psg.Column(
        [
            [
                psg.Checkbox(
                    "Station/Siding Start", key="CHOOSE_BY_LOCATION", default=True
                )
            ],
            [
                psg.Column(
                    [
                        [psg.Text("Location")],
                        [psg.DropDown([], key="LOCATION_SELECT", size=(25, 1))],
                        [psg.Text("Track IDs")],
                        [psg.DropDown([], key="TRACK_IDS", size=(25, 1))],
                    ],
                    vertical_alignment="top",
                ),
                psg.Column(
                    [
                        [psg.Text("Track ID Rear")],
                        [psg.Input("", key="TRACK_ID_REAR", size=(25, 1))],
                        [psg.Text("Track ID Front")],
                        [psg.DropDown("", key="TRACK_ID_FRONT", size=(25, 1))],
                    ],
                    visible=False,
                    vertical_alignment="top",
                ),
            ],
        ]
    )


def _build_finish_service_ref() -> psg.Column:
    return psg.Column(
        [
            [psg.Text("Exit Location IDs\n(comma separated)")],
            [psg.Input("", key="EXIT_LOCATION_IDS", size=(25, 1))],
            [psg.Text("New Service Reference")],
            [psg.Input("", key="FINISH_SRV_REF", size=(25, 1))],
            [psg.Text("Manual Termination Time")],
            [
                psg.Checkbox("", default=False, key="MANUAL_TERMINATION"),
                psg.Spin(
                    values=list(_times_of_day()),
                    key="MANUAL_TERMINATION_TIME",
                    size=(10, 1),
                ),
            ],
        ]
    )


def _build_others_ref_panel() -> typing.List[psg.Element]:
    return [
        psg.Column(
            [
                [psg.Text("Parent Service Reference")],
                [psg.DropDown([], key="PARENT_SRV_REF", size=(25, 1))],
            ],
            vertical_alignment="top",
        ),
        psg.Column(
            [
                [psg.Text("Shuttle Reference")],
                [psg.Input("", key="SHUTTLE_REF", size=(25, 1))],
                [psg.Text("Shuttle Return Service Reference")],
                [psg.Input("", key="SHUTTLE_RETURN_REF", size=(25, 1))],
                [psg.Text("Feeder Reference")],
                [psg.DropDown([], key="FEEDER_SRV_REF", size=(25, 1))],
            ],
            vertical_alignment="top",
        ),
    ]


def _build_repeats_panel() -> typing.List[psg.Column]:
    return [
        psg.Column(
            [
                [psg.Text("Repeats")],
                [psg.Text("Time Interval (mins)")],
                [psg.Text("Reference Increment")],
            ]
        ),
        psg.Column(
            [
                [psg.Spin(values=list(range(99)), initial_value=0, key="REPEATS_N")],
                [psg.Spin(values=list(range(99)), initial_value=0, key="REPEATS_MINS")],
                [psg.Spin(values=list(range(99)), initial_value=0, key="REPEATS_INC")],
            ]
        ),
    ]


def _build_action_buttons() -> psg.Column:
    return psg.Column(
        [
            [
                psg.Button("Insert/Edit", key="INSERT_EDIT"),
                psg.Button("Delete", key="DELETE"),
                psg.Button("Clear", key="CLEAR"),
                psg.Button("Clone", key="CLONE"),
            ]
        ],
        justification="r",
    )


def _build_control_panel_1() -> psg.Column:
    return psg.Column(
        [
            [_build_specs_layout(), _build_unit_select()],
            [_build_train_consist_panel()],
            [_build_ref_desc_boxes()],
        ],
        vertical_alignment="top",
        expand_x=True,
        expand_y=True,
    )


def _build_control_panel_2() -> psg.Column:
    return psg.Column(
        [
            [_build_service_type_select(), _build_location_select()],
            _build_others_ref_panel(),
        ],
        vertical_alignment="top",
        expand_x=True,
        expand_y=True,
    )


def _build_control_panel_3() -> psg.Column:
    return psg.Column(
        [
            [_build_finish_type_select(), _build_finish_service_ref()],
            _build_repeats_panel(),
        ],
        expand_x=True,
        expand_y=True,
    )


def build_interface(timetable_entries: typing.List[typing.List[typing.Any]]=None, service_entries: typing.List[typing.List[typing.Any]]=None) -> psg.Window:
    psg.theme("Default1")

    if not timetable_entries:
        timetable_entries = [["", "", ""]]

    if not service_entries:
        service_entries = [["", "", ""]]

    _menu: psg.MenuBar = _build_menu_bar()
    _setup_buttons: typing.List[typing.List[psg.Element]] = _build_setup_buttons()
    _ttb_table: psg.Table = _build_timetable_display_table(timetable_entries)
    _srv_table: psg.Table = _build_service_display_table(service_entries)

    _layout = [
        [_menu],
        [
            psg.Column(
                [*_setup_buttons], expand_x=True, expand_y=True, justification="c"
            )
        ],
        [
            psg.Column(
                [[_srv_table, _ttb_table]],
                expand_x=True,
                expand_y=True,
                justification="c",
            )
        ],
        [_build_control_panel_1(), _build_control_panel_2(), _build_control_panel_3()],
        [_build_action_buttons()],
    ]

    return psg.Window("RailOS Timetable Editor", _layout, size=(1200, 768))


if __name__ in "__main__":
    _window = build_interface()

    while True:
        _event, _ = _window.read()
        if _event in [psg.WIN_CLOSED, "Exit"]:
            _window.close()
            break
