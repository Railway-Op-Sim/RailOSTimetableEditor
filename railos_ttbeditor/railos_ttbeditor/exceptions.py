import dataclasses

@dataclasses.dataclass
class RailOSTTBEditorBaseException(Exception):
    message: str
    title: str

class RailOSLocationUnset(RailOSTTBEditorBaseException):
    def __init__(self) -> None:
        super().__init__(
            message="Railway Operation Simulator location must be set.",
            title="RailOS Location Unset"
        )

class NoTimetableLoaded(RailOSTTBEditorBaseException):
    def __init__(self) -> None:
        super().__init__(
            message="A timetable file (.ttb) must be loaded to use this feature.",
            title="No timetable file loaded"
        )