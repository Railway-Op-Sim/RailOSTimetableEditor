import PySimpleGUI as psg
import sys
import time
import railos_ttbeditor.gui as railos_gui
import railos_ttbeditor.session as railosttb_session

from railos_ttbeditor.addresses import GUIKey


def railosttb_editor() -> None:
    _session = railosttb_session.RailOSTTBSession()

    while True:
        _event, _values = _session.window.read()
        if _event in [psg.WIN_CLOSED, "Exit"]:
            _session.window.close()
            sys.exit(0)
        if _event == GUIKey.MANUAL_TERMINATION.name:
            _session.manual_termination_checkbox(**_values)
        elif _event == GUIKey.RAILOS_LOC_BROWSER.name:
            _railos_location = _session.find_railos()
        elif _event == GUIKey.ROUTE_BROWSER.name:
            _session.choose_route()
        elif _event == "Timetable":
            _session.choose_timetable()
        elif _event == GUIKey.TIMETABLE_DISPLAY.name:
            _session.choose_service(_values[_event])


if __name__ in "__main__":
    railosttb_editor()
