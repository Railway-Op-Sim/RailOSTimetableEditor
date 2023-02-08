import PySimpleGUI as psg
import sys
import time
import railos_ttbeditor.gui as railos_gui
import railos_ttbeditor.session as railosttb_session


def railosttb_editor() -> None:
    _window: psg.Window = railos_gui.build_interface()
    _session = railosttb_session.RailOSTTBSession(_window)

    while True:
        _event, _values = _window.read()
        if _event in [psg.WIN_CLOSED, "Exit"]:
            _window.close()
            sys.exit(0)
        if _event == "MANUAL_TERMINATION":
            _session.manual_termination_checkbox(_window, **_values)
        elif _event == "RAILOS_LOC_BROWSER":
            _railos_location = _session.find_railos()
        elif _event == "ROUTE_BROWSER":
            _session.choose_route(_window)
        elif _event == "Timetable":
            _window.close()
            _window = railos_gui.build_interface(timetable_entries=_session.choose_timetable())


if __name__ in "__main__":
    railosttb_editor()
