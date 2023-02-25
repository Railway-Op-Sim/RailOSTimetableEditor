import PySimpleGUI as psg
import sys
import time
import railos_ttbeditor.session as railosttb_session
import railos_ttbeditor.exceptions as railosttb_exc

from railos_ttbeditor.addresses import GUIKey


def railosttb_editor() -> None:
    _session = railosttb_session.RailOSTTBSession()

    while True:
        _event, _values = _session.window.read()
        if _event in [psg.WIN_CLOSED, "Exit"]:
            _session.window.close()
            sys.exit(0)
        try:
            if _event == GUIKey.RAILOS_LOC_BROWSER.name:
                _session.find_railos()
            elif _event == GUIKey.CHOOSE_BY_LOCATION.name:
                _session.window[GUIKey.LOCATIONS_COLUMN.name].update(visible=_values[GUIKey.CHOOSE_BY_LOCATION.name])
                _session.window[GUIKey.TRACK_IDS_COLUMN.name].update(visible=not _values[GUIKey.CHOOSE_BY_LOCATION.name])
            elif _event == GUIKey.LOCATION_SELECT.name:
                _session.populate_track_ids(_values[GUIKey.LOCATION_SELECT.name])
            elif _event == GUIKey.TRACK_IDS.name:
                _session.select_track_spawn(_values[GUIKey.TRACK_IDS.name])
            elif _event == GUIKey.MANUAL_TERMINATION.name:
                _session.manual_termination_checkbox(**_values)
            elif _event == GUIKey.ROUTE_BROWSER.name:
                _session.choose_route()
            elif _event == "Timetable":
                _session.choose_timetable()
            elif _event == GUIKey.TIMETABLE_DISPLAY.name:
                _session.choose_service(_values[_event])
            if _values.get(GUIKey.UNIT_SELECT_KPH.name):
                _session.set_to_kph()
            elif _values.get(GUIKey.UNIT_SELECT_MPH.name):
                _session.set_to_mph()
            if _values.get(GUIKey.UNIT_SELECT_KW.name):
                _session.set_to_kw()
            elif _values.get(GUIKey.UNIT_SELECT_HP.name):
                _session.set_to_hp()
        except railosttb_exc.RailOSLocationUnset as e:
            psg.popup(e.message, title=e.title, button_color=None, keep_on_top=True)


if __name__ in "__main__":
    railosttb_editor()
