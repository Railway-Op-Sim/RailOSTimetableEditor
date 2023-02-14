import typing
import os.path
from railos_ttbeditor.addresses import GUIKey
import railostools.rly as railos_rly
import railos_ttbeditor.gui as railos_gui
import railostools.ttb.parsing as railos_ttb
from railostools.ttb.components import TimetabledService
from railostools.ttb.components.actions import Location, cdt, jbo, fsp, rsp, pas
from railostools.ttb.components.start import Sfs, Snt, Sns, Sns_fsh, Snt_sh, Sns_sh
import railostools.exceptions as railos_exc
import PySimpleGUI as psg
import pandas


class RailOSTTBSession:
    def __init__(self) -> None:
        self._railos_location: typing.Optional[str] = None
        self._current_rly: typing.Optional[str] = None
        self._current_ttb: typing.Optional[str] = None
        self.service_list: typing.List[str] = [["", "", "", ""]]
        self.service_entry: typing.List[str] = [["", "", "", ""]]
        self._railway_parser = railos_rly.RlyParser()
        self._timetable_parser = railos_ttb.TTBParser()
        self.current_service: TimetabledService = None
        self.window = railos_gui.build_interface(
            timetable_entries=self.service_list,
            service_entries=self.service_entry
        )

    def manual_termination_checkbox(self, MANUAL_TERMINATION: bool) -> None:
        self.window[GUIKey.MANUAL_TERMINATION_TIME.name].update(disabled=MANUAL_TERMINATION)

    def _load_railway(self, railway_file: str) -> None:
        railos_rly.RlyParser().parse(railway_file)

    def _populate_services(self) -> typing.List[typing.List[str]]:
        if not (services := self._timetable_parser._data.services):
            return

        _table: typing.List[typing.List[str]] = [
            [identifier, service.start_type.time, service.header.description]
            for identifier, service in services.items()
        ]
        return _table

    def choose_service(
        self, TIMETABLE_DISPLAY: typing.List[typing.List[typing.Any]]
    ) -> typing.Dict:
        if not (
            _data_selected := [
                list(self._timetable_parser._data.services.values())[row]
                for row in TIMETABLE_DISPLAY
            ]
        ):
            return [["", "", "", ""]]
        _service: TimetabledService = _data_selected[0]
        self.service_list = []

        for action in _service.actions.values():
            if isinstance(action, Location):
                self.service_list.append(
                    [action.time, action.end_time or "", action.name, ""]
                )
            elif isinstance(action, cdt):
                self.service_list.append([action.time, "", "", "↩"])
            elif isinstance(action, jbo):
                self.service_list.append(
                    [action.time, "", "", f"← {action.joining_service_ref}"]
                )
            elif isinstance(action, fsp):
                self.service_list.append(
                    [
                        action.time,
                        "",
                        "",
                        f"{_service.header.reference} ↔ {action.new_service_ref}",
                    ]
                )
            elif isinstance(action, rsp):
                self.service_list.append(
                    [
                        action.time,
                        "",
                        "",
                        f"{action.new_service_ref} ↔ {_service.header.reference}",
                    ]
                )
            elif isinstance(action, pas):
                self.service_list.append([action.time, "", f"{action.location}", "↓"])
        self.window[GUIKey.SERVICE_DISPLAY.name].update(values=self.service_list)
        self.current_service = _service

    def find_railos(self) -> str:
        _exe = psg.popup_get_file(
            "",
            multiple_files=False,
            no_window=True,
            file_types=(("Railway Operation Simulator", "railway.exe"),),
        )
        if not _exe:
            return
        self._railos_location = _exe
        self.window[GUIKey.RAILOS_LOC.name].update(
            _exe, text_color="green" if os.path.exists(_exe) else "red"
        )

    def choose_route(self) -> None:
        _rly_file = psg.popup_get_file(
            "",
            multiple_files=False,
            no_window=True,
            file_types=(("Railway Files", "*.rly"),),
        )
        if not _rly_file:
            return
        self._current_rly = _rly_file
        self._railway_parser.parse(_rly_file)
        self.window[GUIKey.ROUTE_CHOICE.name].update(
            _rly_file, text_color="green" if os.path.exists(_rly_file) else "red"
        )

    def choose_timetable(self) -> typing.List[typing.List[typing.Any]]:
        _ttb_file = psg.popup_get_file(
            "",
            multiple_files=False,
            no_window=True,
            file_types=(("Timetable Files", "*.ttb"),),
        )
        if not _ttb_file:
            return
        self._current_ttb = _ttb_file

        try:
            self._timetable_parser.parse(_ttb_file)
        except Exception:
            psg.popup(
                f"Error: Failed to parse file '{_ttb_file}'. "
                "Note legacy timetables are not supported.",
                title="Parsing Error",
            )
            self._current_ttb = None
            return
        self.service_list = self._populate_services()
        self.window[GUIKey.TIMETABLE_DISPLAY.name].update(values=self.service_list)
