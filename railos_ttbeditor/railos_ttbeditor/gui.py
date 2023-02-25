import PySimpleGUI as psg
import typing
import os.path
import pathlib
import toml
import datetime
import railostools.ttb.components as railos_ttb_comp
from railos_ttbeditor.addresses import GUIKey

RAILOSTTB_EDITOR_CACHE_FILE: str = os.path.join(pathlib.Path(__file__).parent, "railosttbedit_cache.toml")

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
    _preset_location: typing.Optional[str] = None
    if os.path.exists(RAILOSTTB_EDITOR_CACHE_FILE):
        _preset_location = toml.load(RAILOSTTB_EDITOR_CACHE_FILE).get("railos_location")
    return [
        [
            psg.Button(
                button_text="Set RailOS Location",
                tooltip="Locate Railway Operation Simulator installation",
                key=GUIKey.RAILOS_LOC_BROWSER.name,
            ),
            psg.Text(
                _preset_location
                or "Railway Operation Simulator Location Unset",
                text_color="green" if _preset_location else "red",
                key=GUIKey.RAILOS_LOC.name,
            ),
            psg.Button(
                button_text="Route",
                tooltip="Select route file",
                key=GUIKey.ROUTE_BROWSER.name,
            ),
            psg.Text(
                "Route Map Not Specified",
                text_color="red",
                key=GUIKey.ROUTE_CHOICE.name,
            ),
        ]
    ]


def _build_service_display_table(service_entries:  typing.List[typing.List[typing.Any]]) -> psg.Table:
    return psg.Table(
        values=service_entries,
        headings=["Arrive", "Depart", "Location", "Action"],
        justification="left",
        key=GUIKey.SERVICE_DISPLAY.name,
        auto_size_columns=False,
        enable_events=True,
        col_widths=[10,10,30],
        alternating_row_color="light grey"
    )


def _build_timetable_display_table(timetable_entries:  typing.List[typing.List[typing.Any]]) -> psg.Table:
    return psg.Table(
        values=timetable_entries,
        headings=["Headcode", "Time", "Description"],
        justification="left",
        key=GUIKey.TIMETABLE_DISPLAY.name,
        enable_events=True,
        auto_size_columns=False,
        col_widths=[10,10,30],
        alternating_row_color="light grey"
    )


def _build_start_time_spinbox() -> typing.Tuple[psg.Text, psg.Spin]:
    return (
        psg.Text("Start Time"),
        psg.Spin(
            values=list(_times_of_day()),
            tooltip="Service start time",
            key=GUIKey.SRV_START_TIME.name,
            size=(10, 1),
        ),
    )


def _build_unit_select() -> psg.Radio:
    return psg.Column(
        [
            [psg.Radio("kph", GUIKey.UNIT_SELECT_SPEED.name, key=GUIKey.UNIT_SELECT_KPH.name, enable_events=True, default=True)],
            [psg.Radio("mph", GUIKey.UNIT_SELECT_SPEED.name, key=GUIKey.UNIT_SELECT_MPH.name, enable_events=True)],
            [],
            [psg.Radio("kW", GUIKey.UNIT_SELECT_MASS.name, key=GUIKey.UNIT_SELECT_KW.name, enable_events=True, default=True)],
            [psg.Radio("hp", GUIKey.UNIT_SELECT_MASS.name, key=GUIKey.UNIT_SELECT_HP.name, enable_events=True)],
        ]
    )


def _build_spinbox_spec(
    specification: str, unit: str
) -> typing.Tuple[psg.Text, psg.Spin]:
    return (
        psg.Text(f"{specification.title()} ({unit})", key=f"SRV_{specification.replace(' ', '_').upper()}_LABEL"),
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
                        [psg.DropDown([], key=GUIKey.CONSIST_TEMPLATES.name, size=(25, 1))],
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
            [psg.Input("", key=GUIKey.SERVICE_REF.name, size=(20, 1))],
            [psg.Text("Description")],
            [psg.Input("", key=GUIKey.SERVICE_DESC.name, size=(40, 1))],
        ],
        vertical_alignment="top",
    )


def _build_service_type_select() -> psg.Column:
    return psg.Column(
        [
            [psg.Radio("Standard", GUIKey.RADIO_SRV_SELECT.name, key=GUIKey.RADIO_SRV_SNT.name, default=True)],
            [psg.Radio("Shuttle from Stop", GUIKey.RADIO_SRV_SELECT.name, key=GUIKey.RADIO_SRV_SNT_SH.name)],
            [
                psg.Radio(
                    "Shuttle from Feeder Service", GUIKey.RADIO_SRV_SELECT.name, key=GUIKey.RADIO_SRV_SNS_SH.name, disabled=True
                )
            ],
            [psg.Radio("Shuttle Finish Service", GUIKey.RADIO_SRV_SELECT.name, key=GUIKey.RADIO_SRV_SNS_FSH.name, disabled=True)],
            [
                psg.Radio(
                    "Service from Other Service", GUIKey.RADIO_SRV_SELECT.name, key=GUIKey.RADIO_SRV_SNS.name, disabled=True
                )
            ],
        ],
        vertical_alignment="top",
    )


def _build_finish_type_select() -> psg.Column:
    return psg.Column(
        [
            [psg.Radio("Finish and Form New Service", GUIKey.RADIO_SRV_FINISH_SELECT.name, key=GUIKey.RADIO_SRV_FNS.name)],
            [
                psg.Radio(
                    "Finish and Remain at Location",
                    GUIKey.RADIO_SRV_FINISH_SELECT.name,
                    key=GUIKey.RADIO_SRV_FRH.name,
                    default=True,
                )
            ],
            [psg.Radio("Finish Shuttle, Remain Here", GUIKey.RADIO_SRV_FINISH_SELECT.name, key=GUIKey.RADIO_SRV_FRH_SH.name)],
            [psg.Radio("Finish and Exit at Location", GUIKey.RADIO_SRV_FINISH_SELECT.name, key=GUIKey.RADIO_SRV_FER.name)],
            [psg.Radio("Finish and Join Other", GUIKey.RADIO_SRV_FINISH_SELECT.name, key=GUIKey.RADIO_SRV_FJO.name)],
            [psg.Radio("Finish Shuttle Feeder", GUIKey.RADIO_SRV_FINISH_SELECT.name, key=GUIKey.RADIO_SRV_F_NSHS.name)],
            [psg.Radio("Finish Shuttle, Form New", GUIKey.RADIO_SRV_FINISH_SELECT.name, key=GUIKey.RADIO_SRV_FNS_SH.name)],
        ],
        vertical_alignment="top",
    )


def _build_location_select(locations: typing.List[str], track_ids: typing.List[str]) -> psg.Column:
    return psg.Column(
        [
            [
                psg.Checkbox(
                    "Station/Siding Start", key=GUIKey.CHOOSE_BY_LOCATION.name, default=True, enable_events=True
                )
            ],
            [
                psg.Column(
                    [
                        [psg.Text("Locations"), psg.Text("", key=GUIKey.SPAWN_DIRECTION.name)],
                        [psg.DropDown(locations, key=GUIKey.LOCATION_SELECT.name, size=(25, 1), enable_events=True, readonly=True)],
                        [psg.Text("Track IDs")],
                        [psg.DropDown(track_ids, key=GUIKey.TRACK_IDS.name, size=(25, 1), enable_events=True, readonly=True)],
                    ],
                    vertical_alignment="top",
                    key=GUIKey.LOCATIONS_COLUMN.name
                ),
                psg.Column(
                    [
                        [psg.Text("Track ID Rear")],
                        [psg.Input("", key=GUIKey.TRACK_ID_REAR.name, size=(25, 1))],
                        [psg.Text("Track ID Front")],
                        [psg.Input("", key=GUIKey.TRACK_ID_FRONT.name, size=(25, 1))],
                    ],
                    visible=False,
                    vertical_alignment="top",
                    key=GUIKey.TRACK_IDS_COLUMN.name
                ),
            ],
        ]
    )


def _build_finish_service_ref() -> psg.Column:
    return psg.Column(
        [
            [psg.Text("Exit Location IDs\n(comma separated)")],
            [psg.Input("", key=GUIKey.EXIT_LOCATION_IDS.name, size=(25, 1))],
            [psg.Text("New Service Reference")],
            [psg.Input("", key=GUIKey.FINISH_SRV_REF.name, size=(25, 1))],
            [psg.Text("Manual Termination Time")],
            [
                psg.Checkbox("", default=False, key=GUIKey.MANUAL_TERMINATION.name),
                psg.Spin(
                    values=list(_times_of_day()),
                    key=GUIKey.MANUAL_TERMINATION_TIME.name,
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
                [psg.DropDown([], key=GUIKey.PARENT_SRV_REF.name, size=(25, 1))],
            ],
            vertical_alignment="top",
        ),
        psg.Column(
            [
                [psg.Text("Shuttle Reference")],
                [psg.Input("", key=GUIKey.SHUTTLE_REF.name, size=(25, 1))],
                [psg.Text("Shuttle Return Service Reference")],
                [psg.Input("", key=GUIKey.SHUTTLE_RETURN_REF.name, size=(25, 1))],
                [psg.Text("Feeder Reference")],
                [psg.DropDown([], key=GUIKey.FEEDER_SRV_REF.name, size=(25, 1))],
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
                [psg.Spin(values=list(range(99)), initial_value=0, key=GUIKey.REPEATS_N.name)],
                [psg.Spin(values=list(range(99)), initial_value=0, key=GUIKey.REPEATS_MINS.name)],
                [psg.Spin(values=list(range(99)), initial_value=0, key=GUIKey.REPEATS_INC.name)],
            ]
        ),
    ]


def _build_action_buttons() -> psg.Column:
    return psg.Column(
        [
            [
                psg.Button("Insert/Edit", key=GUIKey.INSERT_EDIT.name),
                psg.Button("Delete", key=GUIKey.DELETE.name),
                psg.Button("Clear", key=GUIKey.CLEAR.name),
                psg.Button("Clone", key=GUIKey.CLONE.name),
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


def _build_control_panel_2(locations: typing.List[str], track_ids: typing.List[str]) -> psg.Column:
    return psg.Column(
        [
            [_build_service_type_select(), _build_location_select(locations, track_ids)],
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


def build_interface(
    timetable_entries: typing.List[typing.List[typing.Any]],
    service_entries: typing.List[typing.List[typing.Any]],
    location_entries: typing.List[str],
    track_ids: typing.List[str],
    *args, **kwargs) -> psg.Window:
    psg.theme("Default1")

    if not timetable_entries:
        timetable_entries = []

    if not service_entries:
        service_entries = []

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
        [_build_control_panel_1(), _build_control_panel_2(location_entries, track_ids), _build_control_panel_3()],
        [_build_action_buttons()],
    ]

    return psg.Window("RailOS Timetable Editor", _layout, size=(1200, 768), *args, **kwargs)


if __name__ in "__main__":
    _window = build_interface()

    while True:
        _event, _ = _window.read()
        if _event in [psg.WIN_CLOSED, "Exit"]:
            _window.close()
            break
