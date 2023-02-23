import typing
import os
import datetime
from railos_ttbeditor.addresses import GUIKey
import railostools.rly as railos_rly
import railos_ttbeditor.gui as railos_gui
import railostools.ttb.parsing as railos_ttb
from railostools.ttb.components import TimetabledService, StartType
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
        self._current_service: TimetabledService = None
        self._display_in_kph: bool = True
        self._display_in_kw: bool = True
        self.window = railos_gui.build_interface(
            timetable_entries=self.service_list,
            service_entries=self.service_entry
        )

    @property
    def parent_service(self) -> typing.Optional[TimetabledService]:
        if isinstance(self.current_service.start_type, (Sfs, Sns, Sns_fsh)):
            return self._timetable_parser._data.services[str(self.current_service.start_type.parent_service)]
        elif isinstance(self.current_service.start_type, Sns_sh):
            return self._timetable_parser._data.services[str(self.current_service.start_type.feeder_ref)]
        return None


    def _assemble_service_list(self) -> typing.List[str]:
        _service_list = []

        for action in self._current_service.actions.values():
            if isinstance(action, Location):
                _service_list.append(
                    [action.time, action.end_time or "", action.name, ""]
                )
            elif isinstance(action, cdt):
                _service_list.append([action.time, "", "", "↩"])
            elif isinstance(action, jbo):
                _service_list.append(
                    [action.time, "", "", f"← {action.joining_service_ref}"]
                )
            elif isinstance(action, fsp):
                _service_list.append(
                    [
                        action.time,
                        "",
                        "",
                        f"{self._current_service.header.reference} ↔ {action.new_service_ref}",
                    ]
                )
            elif isinstance(action, rsp):
                _service_list.append(
                    [
                        action.time,
                        "",
                        "",
                        f"{action.new_service_ref} ↔ {self._current_service.header.reference}",
                    ]
                )
            elif isinstance(action, pas):
                _service_list.append([action.time, "", f"{action.location}", "↓"])
        return _service_list

    @property
    def current_service(self) -> TimetabledService:
        return self._current_service

    @current_service.setter
    def current_service(self, service: TimetabledService) -> None:
        self._current_service = service
        self.service_list = self._assemble_service_list()
        self.window[GUIKey.SRV_MAX_SPEED.name].update(disabled=self.parent_service is not None)
        self.window[GUIKey.SRV_START_SPEED.name].update(disabled=self.parent_service is not None)
        self.window[GUIKey.SRV_MASS.name].update(
            getattr(self.parent_service or self._current_service, "header").mass or 0,
            disabled=self.parent_service is not None,
        )
        if self._current_service.header.power or self.parent_service:
            self.window[GUIKey.SRV_MAX_POWER.name].update(
                getattr(self.parent_service or self._current_service, "header").power or 0
                * (1 if self._display_in_kw else 1.34),
                disabled=self.parent_service is not None,
            )
        self.window[GUIKey.SRV_MAX_BRAKE_FORCE.name].update(
            getattr(self.parent_service or self._current_service, "header").brake_force or 0,
            disabled=self.parent_service is not None
        )
        self.window[GUIKey.SERVICE_DISPLAY.name].update(values=self.service_list)
        self.window[GUIKey.SRV_START_TIME.name].update(service.start_type.time)
        if self._current_service.header.start_speed or self.parent_service:
            self.window[GUIKey.SRV_START_SPEED.name].update(
                getattr(self.parent_service or self._current_service, "header").start_speed or 0
                * (1 if self._display_in_kph else 0.62137)
            )
        if self._current_service.header.max_speed or self.parent_service:
            self.window[GUIKey.SRV_MAX_SPEED.name].update(
                getattr(self.parent_service or self._current_service, "header").max_speed or 0
                * (1 if self._display_in_kph else 0.62137)
            )
        self.set_start_type(self._current_service.start_type)
        self.set_end_type(self._current_service.finish_type)

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
        self.current_service = _data_selected[0]

    def set_to_mph(self) -> None:
        try:
            _current_max_speed: float = self.current_service.header.max_speed
            _current_start_speed: float = self.current_service.header.start_speed
            self.window[GUIKey.SRV_MAX_SPEED.name].update(int(_current_max_speed * 0.62137))
            self.window[GUIKey.SRV_START_SPEED.name].update(int(_current_start_speed * 0.62137))
            self.window[GUIKey.SRV_START_SPEED_LABEL.name].update("Start Speed (mph)")
            self.window[GUIKey.SRV_MAX_SPEED_LABEL.name].update("Max Speed (mph)")
            self._display_in_kph = False
        except (AttributeError, TypeError):
            return

    def set_to_kph(self) -> None:
        try:
            _current_max_speed: float = self.current_service.header.max_speed
            _current_start_speed: float = self.current_service.header.start_speed
            self.window[GUIKey.SRV_MAX_SPEED.name].update(int(_current_max_speed / 0.62137))
            self.window[GUIKey.SRV_START_SPEED.name].update(int(_current_start_speed / 0.62137))
            self.window[GUIKey.SRV_START_SPEED_LABEL.name].update("Start Speed (kph)")
            self.window[GUIKey.SRV_MAX_SPEED_LABEL.name].update("Max Speed (kph)")
            self._display_in_kph = True
        except (AttributeError, TypeError) as e:
            return

    def set_to_kw(self) -> None:
        try:
            _current_power: float = self.current_service.header.power
            self.window[GUIKey.SRV_MAX_POWER.name].update(int(_current_power / 1.34))
            self.window[GUIKey.SRV_MAX_POWER_LABEL.name].update("Max Power (kW)")
            self._display_in_kw = True
        except (AttributeError, TypeError):
            return

    def set_to_hp(self) -> None:
        try:
            _current_power: float = self.current_service.header.power
            self.window[GUIKey.SRV_MAX_POWER.name].update(int(_current_power * 1.34))
            self.window[GUIKey.SRV_MAX_POWER_LABEL.name].update("Max Power (hp)")
            self._display_in_kw = False
        except (AttributeError, TypeError) as e:
            return

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
        except Exception as e:
            _log_file: str = os.path.join(os.getcwd(), datetime.datetime.now().strftime("parse_err_%H_%M_%S_%Y_%m_%d.log"))
            psg.popup(
                f"Error: Failed to parse file '{_ttb_file}'. "
                f"Note legacy timetables are not supported, see log file '{_log_file}'",
                title="Parsing Error",
            )
            with open(_log_file, "w") as out_f:
                out_f.write(e.args[0])
            self._current_ttb = None
            return
        self.service_list = self._populate_services()
        self.window[GUIKey.TIMETABLE_DISPLAY.name].update(values=self.service_list)

    def set_start_type(self, start_type: StartType) -> None:
        self.window[f"RADIO_SRV_{start_type.name.upper().replace('-', '_')}"].update(value=True)

    def set_end_type(self, end_type: StartType) -> None:
        self.window[f"RADIO_SRV_{end_type.name.upper().replace('-', '_')}"].update(value=True)