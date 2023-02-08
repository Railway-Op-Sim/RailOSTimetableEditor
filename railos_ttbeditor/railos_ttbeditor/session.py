import typing
import os.path
import dataclasses
import railostools.rly as railos_rly
import railostools.ttb.parsing as railos_ttb
from railostools.ttb.components import TimetabledService
from railostools.ttb.components.actions import Location
import railostools.exceptions as railos_exc
import PySimpleGUI as psg
import pandas




@dataclasses.dataclass
class RailOSTTBSession:
    window: psg.Window

    def __post_init__(self) -> None:
        self._railos_location: typing.Optional[str] = None
        self._current_rly: typing.Optional[str] = None
        self._current_ttb: typing.Optional[str] = None
        self._railway_parser = railos_rly.RlyParser()
        self._timetable_parser = railos_ttb.TTBParser()

    def manual_termination_checkbox(self, MANUAL_TERMINATION: bool) -> None:
        self.window["MANUAL_TERMINATION_TIME"].update(disabled=MANUAL_TERMINATION)

    def _load_railway(self, railway_file: str) -> None:
        railos_rly.RlyParser().parse(railway_file)

    def _populate_services(self) -> None:
        if not (services := self._timetable_parser._data.services):
            return

        _table: typing.List[typing.List[str]] = [
            [identifier, service.start_type.time, service.header.description]
            for identifier, service in services.items()
        ]
        return _table

    def choose_service(self, TIMETABLE_DISPLAY: typing.List[typing.List[typing.Any]]) -> typing.List[typing.List[str]]:
        if not (
            _data_selected := [
                list(self._timetable_parser._data.services.values())[row]
                for row in TIMETABLE_DISPLAY
            ]
        ):
            return [["", "", ""]]
        _service: TimetabledService = _data_selected[0]
        _table_data: typing.List[typing.List[str]] = [
            [action.time, action.end_time or "", action.name]
            for action in _service.actions.values()
            if isinstance(action, Location)
        ]
        return _table_data

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
        self.window["RAILOS_LOC"].update(
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
        self.window["ROUTE_CHOICE"].update(
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
                title="Parsing Error"
            )
            self._current_ttb = None
            return
        return self._populate_services()
