//-------------------------------------------------------------------------//
//         ROS Timetable Editor File Parser Class Definition               //
//                                                                         //
// This file provides part of the source code towards the standalone       //
// timetable editor constructed using the Qt v5.14 framework.              //
// for Railway Operation Simulator, a railway signalling simulation        //
// by Albert Ball, Copyright (c) 2010 [original development].              //
//                                                                         //
// Copyright (C) 2020 Kristian Zarebski                                    //
//                                                                         //
// This program is free software: you can redistribute it and/or modify    //
// it under the terms of the GNU General Public License as published by    //
// the Free Software Foundation, either version 3 of the License, or       //
// (at your option) any later version.                                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           //
// GNU General Public License for more details.                            //
//                                                                         //
// You should have received a copy of the GNU General Public License       //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.   //
//                                                                         //
// Efforts to improve on this application are welcomed, with the request   //
// that any improvements you deem notable be offered back to the           //
// original project.                                                       //
//                                                                         //
//-------------------------------------------------------------------------//

#include "rosttbgen.hxx"

QString ROSTTBGen::parse_file(const QFileDialog* file, const QDir* directory)
{
    _cached_text = "";
    QString in_file = file->getOpenFileName(_parent, "Open Timetable",directory->absolutePath(), "ROS Timetable Files (*.ttb)");
    QFile open_file(in_file);
    if (!open_file.open(QIODevice::ReadOnly | QFile::Text))
    {
        if(in_file.isEmpty() || in_file.isNull()) return "NULL";
        QMessageBox::critical(_parent, "File Open Failure", "Failed to open file '"+in_file+"'");
        return QString();
    }

    QTextStream in(&open_file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        bool _record_ttb = false;

        QStringList _parts = line.split(QChar::Null);

        bool _start_time_found = false;

        for(auto part : _parts)
        {
            if(part.contains(";"))
            {
                _record_ttb = _check_isID(part.split(";")[0]);

                if(_record_ttb)
                {
                    _input_data += part.split(",");
                }
            }
            else
            {
                if(!_start_time_found)
                {
                    for(int i{0}; i < part.size(); ++i)
                    {
                        if(_check_isTime(part.mid(i, i+5)))
                        {
                            _input_data.append(part.mid(i, i+5));
                            _start_time_found = true;
                            break;
                        }
                    }
                }
                if(part.size() > 0) _comments.append(part);
            }
        }
    }

    open_file.close();

    _process_data();

    QString _file_name = open_file.fileName();

    _current_timetable->setFileName(_file_name);

    return _file_name;
}

QString ROSTTBGen::parse_rly(const QString railways_dir)
{
    QFileDialog* _route_diag = new QFileDialog;
    QString _current_route = _route_diag->getOpenFileName(_parent, "Open Route", railways_dir,
                                   "ROS Railway Files (*.rly)");
    QStringList _stations = {};
    QFile open_file(_current_route);
    if (!open_file.open(QIODevice::ReadOnly | QFile::Text))
        return "NULL";

    QTextStream in(&open_file);
    QString previous = "";
    int counter = 0;
    while (!in.atEnd()) {

        QString line = in.readLine();
        if(previous[0].isLetter() && line[0] == "*")
        {
            _stations.push_back(previous.split(QChar::Null)[0]);
        }
        if(counter > 0)
        {
            previous = line;
        }
        ++counter;
    }
    _stations_list = QSet<QString>(_stations.begin(), _stations.end());

    open_file.close();

    return _current_route;
}

bool ROSTTBGen::_check_isTime(QString string)
{
    if(string.size() != 5) return false;
    return string[0].isNumber() && string[1].isNumber() && string[2] == ":" && string[3].isNumber() && string[4].isNumber();
}

bool ROSTTBGen::_check_isID(QString string)
{
    if(string.size() != 4 || _check_isTime("0"+string)) return false;

    return true;
}

bool ROSTTBGen::_check_Integer(QString string)
{
    for(auto n : string)
    {
        if(!n.isNumber()) return false;
    }

    return true;
}

bool ROSTTBGen::_checkPas(QStringList str_list)
{
    if(str_list.contains("pas")) return true;

    return false;
}

bool ROSTTBGen::_checkCDT(QStringList str_list)
{
    if(str_list.contains("cdt")) return true;

    return false;
}

bool ROSTTBGen::_checkIfStart(QStringList str_list)
{
    QStringList _types = _start_types.values();
    for(auto type : _types)
    {
        if(str_list.contains(type)) return true;
    }

    return false;
}

ROSService::ServiceType ROSTTBGen::_parseType(QStringList str_list)
{
    QStringList _str_types = _start_types.values();
    QList<ROSService::ServiceType> _types = _start_types.keys();
    for(int i{0}; i < _start_types.size(); ++i)
    {
        if(str_list.contains(_str_types[i])) return _types[i];
    }

    return ROSService::ServiceType::Service;
}

ROSService::FinishState ROSTTBGen::_parseExit(QStringList str_list)
{
    QStringList _str_types = _exit_types.values();
    QList<ROSService::FinishState> _types = _exit_types.keys();
    for(int i{0}; i < _exit_types.size(); ++i)
    {
        if(str_list.contains(_str_types[i]))
        {
            return _types[i];
        }
    }

    return ROSService::FinishState::FinishExit;
}

bool ROSTTBGen::_checkIfEnd(QStringList str_list)
{
    QStringList _types = _exit_types.values();
    for(auto type : _types)
    {
        if(str_list.contains(type)) return true;
    }

    return false;
}

bool ROSTTBGen::_checkStationCall(QStringList str_list)
{
    return _check_isTime(str_list[0]) && !_checkCDT(str_list) && !_checkPas(str_list) && !_checkIfStart(str_list) && !_checkIfEnd(str_list);
}

bool ROSTTBGen::_checkIsCoordinate(QString string)
{
    int n_chars = 0, n_nums = 0;

    for(int i{0}; i < string.size(); ++i)
    {
        if(string[i].isLetter()) n_chars++;
        else n_nums++;
    }

    return string.contains("-") && string[string.indexOf("-")-1].isNumber() && n_chars < 2 && n_nums > 1;
}

bool ROSTTBGen::_isRepeat(QStringList str_list)
{
    if(str_list.size() != 4) return false;

    return str_list[0] == "R" && _check_Integer(str_list[1]) && _check_Integer(str_list[2]) && _check_Integer(str_list[3]);
}

bool ROSTTBGen::_isCallingPoint(QStringList str_list)
{
    if(str_list.size() != 3) return false;
    return _check_isTime(str_list[0]) && _check_isTime(str_list[1]) && str_list[2][0].isLetter();
}

bool ROSTTBGen::_isStartStopPoint(QStringList str_list)
{
    if(str_list.size() != 2) return false;
    return !_check_isID(str_list[0]) && _check_isTime(str_list[0]) && str_list[1][0].isLetter() && !_checkCDT(str_list) && !_checkPas(str_list);
}

bool ROSTTBGen::_isServiceDefinition(QStringList str_list)
{
    if(str_list.size() != 7) return false;
    return _check_isID(str_list[0]) && _check_Integer(str_list[2]) && _check_Integer(str_list[3]) && _check_Integer(str_list[4]) && _check_Integer(str_list[5]) && _check_Integer(str_list[6]);
}

bool ROSTTBGen::_isContinuedServiceDefinition(QStringList str_list)
{
    if(str_list.size() != 2) return false;

    return _check_isID(str_list[0]);
}

bool ROSTTBGen::_isDirChange(QStringList str_list)
{
    return str_list.size() == 2 && str_list[1] == "cdt";
}

bool ROSTTBGen::_isSplit(QStringList str_list)
{
    return _check_isTime(str_list[0]) && (str_list[1] == "fsp" || str_list[1] == "rsp");
}

void ROSTTBGen::_process_service_candidate(int int_id, QStringList service)
{
    QString _id = "NULL", _description = "NULL";
    QTime _start_time = QTime();
    if(_isServiceDefinition(service[0].split(";")))
    {
        QStringList _components = service[0].split(";");
        _id = _components[0];
        _description = _components[1];
    }
    else if(_isContinuedServiceDefinition(service[0].split(";")))
    {
        QStringList _components = service[0].split(";");
        _id = _components[0];
        _description = _components[1];
    }

    if(_checkIfStart(service[1].split(";")))
    {
        QStringList _components = service[1].split(";");
        _start_time = QTime::fromString(_components[0], "HH:mm");
    }

    else
    {
        QMessageBox::critical(_parent, "Import Failure", "Failed to import service "+QString::number(int_id)+" '"+_id+"'");
        return;
    }

    ROSService* _service = new ROSService(int_id, _start_time, _id, _description);

    if(_isServiceDefinition(service[0].split(";")))
    {
        QStringList _components = service[0].split(";");
        _service->setStartSpeed(_components[2].toInt());
        _service->setMaxSpeed(_components[3].toInt());
        _service->setMass(_components[4].toInt());
        _service->setMaxBrake(_components[5].toInt());
        _service->setPower(_components[6].toInt());
    }

    if(_checkIfStart(service[1].split(";")))
    {
        QStringList _components = service[1].split(";");

        ROSService::ServiceType _type = _parseType(_components[1].split(";"));
        _service->setType(_type);
        QStringList _element_ids;
        switch(_type)
        {
            case ROSService::ServiceType::Service:
                _element_ids = _components[2].split(" ");
                _service->setEntryPoint(_element_ids);
                break;
            case ROSService::ServiceType::ShuttleFromStop:
                _element_ids = _components[2].split(" ");
                _service->setEntryPoint(_element_ids);
                _service->setParent(_components[3]);
                break;
            case ROSService::ServiceType::ShuttleFromFeeder:
                _service->setParent(_components[2]);
                _service->setDaughter(_components[3]);
                break;
            case ROSService::ServiceType::ServiceFromService:
                _service->setParent(_components[2]);
                break;
            case ROSService::ServiceType::ShuttleFinishService:
                _service->setParent(_components[2]);
                break;
            case ROSService::ServiceType::ServiceFromSplit:
                _service->setParent(_components[2]);
                break;
            default:
                break;

        }
    }

    int _final_index = 1;
    QStringList _components;

    if(_isRepeat(service[service.size()-1].split(";")))
    {
        _components = service[service.size()-1].split(";");
        _service->setNRepeats(_components[3].toInt());
        _service->setRepeatInterval(_components[1].toInt());
        _service->setIDIncrement(_components[2].toInt());
        _final_index = 2;
    }

    _components = service[service.size()-_final_index].split(";");
    ROSService::FinishState _fin_state = _parseExit(_components);
    _service->setFinishState(_fin_state);

    switch(_fin_state)
    {
        case ROSService::FinishState::FinishRemainHere:
            break;
        case ROSService::FinishState::FinishExit:
            _service->setExitPoint(_components[2]);
            break;
        case ROSService::FinishState::FinishFormNew:
            _service->setDaughter(_components[2]);
            break;
        case ROSService::FinishState::FinishJoinOther:
            _service->setDaughter(_components[2]);
            break;
        case ROSService::FinishState::FinishShuttleFormNew:
            _service->setParent(_components[2]);
            _service->setDaughter(_components[3]);
            break;
        case ROSService::FinishState::FinishShuttleRemainHere:
            _service->setParent(_components[2]);
            break;
        case ROSService::FinishState::FinishSingleShuttleFeeder:
            _service->setDaughter(_components[2]);
            break;
        default:
            break;
    }

    if(_fin_state != ROSService::FinishState::FinishRemainHere)
    {
        QTime _time = QTime::fromString(_components[0], "HH:mm");
        _service->setExitTime(_time);
    }

    for(int i{2}; i < service.size()-_final_index; ++i)
    {
        bool _pass_point = false;

        _service->setLabelledLocationStart(i == 2 && service[i].split(";").size() == 2);

        if(_service->getStations().size() > i-1 && _isDirChange(service[i].split(";")))
        {
            _service->setDirectionChangeAtStop(i-1, true, QTime::fromString(service[i].split(";")[0], "HH:mm"));
        }

        else if(_service->getStations().size() > i-1 && _isSplit(service[i].split(";")))
        {
            _service->setSplitAtStop(i-1, service[i].split(";")[1], service[i].split(";")[2], QTime::fromString(service[i].split(";")[0], "HH:mm"));
        }
        else if(_isCallingPoint(service[i].split(";")))
        {
            QStringList _components = service[i].split(";");
            _service->addStation({QTime::fromString(_components[0], "HH:mm"),
                                 QTime::fromString(_components[1], "HH:mm")},
                                 _components[2]);
        }

        else if(_isStartStopPoint(service[i].split(";")))
        {
            QStringList _components = service[i].split(";");
            _service->addStation({QTime::fromString(_components[0], "HH:mm"),
                                 QTime()},
                                 _components[1]);
        }

        else if(_checkPas(service[i].split(";")))
        {
            QStringList _components = service[i].split(";");
            _service->addStation({QTime::fromString(_components[0]), QTime()},
                                 _components[2]);
            _pass_point = true;

        }

        _service->setStopAsPassPoint(_service->getStations().size()-1, _pass_point);
        bool _cdt = (i < service.size()-2 && _checkCDT(service[i+1].split(";")));
        _service->setDirectionChangeAtStop(_service->getStations().size()-1, _cdt);

    }

    _current_timetable->addService(_service);
}

void ROSTTBGen::_process_data()
{
    if(_input_data.size() < 1)
    {
        QMessageBox::critical(_parent, "Parsing Failed", "No data found in input file");
        return;
    }

    QList<QStringList> _services;

    QStringList _temp = {};

    QTime _time_temp = QTime::fromString(_input_data[0], "HH:mm");
    _current_timetable->setStartTime(_time_temp);

    for(int i{1}; i < _input_data.size(); ++i)
    {
        if((_isServiceDefinition(_input_data[i].split(";")) || _isContinuedServiceDefinition(_input_data[i].split(";"))) && i != 1)
        {
            _services.push_back(_temp);
            _temp = QStringList();
        }
        _temp.push_back(_input_data[i]);
    }

    _services.push_back(_temp);

    for(int i{0}; i < _services.size(); ++i)
    {
        _process_service_candidate(i, _services[i]);
    }

    if(_current_timetable->size() != _services.size())
    {
        QMessageBox::critical(_parent, "Failed Service Importing", "Number of services created does not match number in input file");
        return;
    }
}

QString ROSTTBGen::_make_service_definition(ROSService* service)
{
    const QString _id = service->getID(), _desc = service->getDescription(),
                  _start_speed = QString::number(service->getStartSpeed()),
                  _max_speed = QString::number(service->getMaxSpeed()),
                  _mass = QString::number(service->getMass()),
                  _brake = QString::number(service->getMaxBrake()),
                  _power = QString::number(service->getPower());

    QString _output;

    if(service->getType() == ROSService::ServiceType::Service || service->getType() == ROSService::ServiceType::ShuttleFromStop)
    {
        _output = join(";", _id, _desc, _start_speed, _max_speed, _mass, _brake, _power);
    }

    else
    {
        _output = join(";", _id, _desc);
    }

    return _output;
}

QString ROSTTBGen::_make_type_line(ROSService* service)
{
    const QString _start_time = service->getStartTime().toString("HH:mm"),
                  _start_type = _start_types[service->getType()];

    switch(service->getType())
    {
        case ROSService::ServiceType::Service:
            return join(";", _start_time, _start_type, service->getStartPoint()[0]+" "+service->getStartPoint()[1]);
        case ROSService::ServiceType::ShuttleFromStop:
            return join(";", _start_time, _start_type, service->getStartPoint()[0], service->getStartPoint()[1], service->getDaughter());
        case ROSService::ServiceType::ServiceFromSplit:
            return join(";",_start_time, _start_type, service->getParent());
        case ROSService::ServiceType::ShuttleFromFeeder:
            return join(";", _start_time, _start_type, service->getDaughter(), service->getParent());
        case ROSService::ServiceType::ServiceFromService:
            return join(";", _start_time, _start_type, service->getParent());
        case ROSService::ServiceType::ShuttleFinishService:
            return join(";", _start_time, _start_type, service->getParent());
        default:
                return "NULL";
    }
}

QStringList ROSTTBGen::_add_stations(ROSService* service)
{
    QStringList _temp = {};

    const int int_start = (service->labelledLocationStart()) ? 1 : 0;
    const int int_final = (service->getFinState() == ROSService::FinishState::FinishExit) ? service->getStations().size() : service->getStations().size()-1;

    // Check if service is service starts from a location or if it is started from another service as this will mean
    // the first station timetable entry should only contain a single time
    if(service->labelledLocationStart() || service->getType() == ROSService::ServiceType::ShuttleFromFeeder || service->getType() == ROSService::ServiceType::ServiceFromService)
    {
        const QTime _depart = service->getTimes()[0][1];
        _temp.append(join(";", _depart.toString("HH:mm"), service->getStations()[0]));
    }

    for(int i{int_start}; i < int_final; ++i)
    {
        const QTime _arrive = service->getTimes()[i][0],
              _depart = service->getTimes()[i][1];
        const QString _name = service->getStations()[i];
        const bool _is_pas = service->getPassList()[i],
             _is_cdt = service->getDirectionChanges()[i],
             _is_join = (service->getJoinData().size() > 0 && _name == service->getJoinData()[service->getJoinData().keys()[0]][0]);

        const QMap<QString, QStringList> _split = service->getSplitData();


        if(_is_pas)
        {
            _temp.append(join(";", _arrive.toString("HH:mm"), "pas", _name));
        }
        else
        {
            _temp.append(join(";", (_depart != QTime()) ? _arrive.toString("HH:mm")+";"+_depart.toString("HH:mm") : _arrive.toString("HH:mm"), _name));
        }

        if(_split.size() > 0 && _split[_split.keys()[0]][1] == service->getStations()[i])
        {
            const QString _type = _split.keys()[0];
            const QStringList _data = _split[_type];
            _temp.append(join(";", _data[2], (_type == "Front") ? "fsp" : "rsp", _data[0]));
        }
        if(_is_join)
        {
            const QMap<QString, QStringList> _join_dat = service->getJoinData();
            const QString _id = _join_dat.keys()[0], _time = _join_dat[_id][1];
            _temp.append(join(";", _time, "jbo", _id));
        }
        if(_is_cdt)
        {
            _temp.append(join(";", (_depart != QTime()) ? _depart.toString("HH:mm") : _arrive.toString("HH:mm"), "cdt"));
        }

    }

    if(service->getFinState() != ROSService::FinishState::FinishExit)
    {
        const QTime _arrive = service->getTimes()[int_final][0];
        _temp.append(join(";", _arrive.toString("HH:mm"), service->getStations()[int_final]));
    }

    return _temp;
}

QString ROSTTBGen::_get_partner(const QString &id)
{
    for(auto s : _current_timetable->getServices())
    {
        const QMap<QString, QStringList> _join_dat = s->getJoinData();
        if(_join_dat[_join_dat.keys()[0]][0] == id)
        {
            return s->getID();
        }
    }

    return "NULL";
}

QString ROSTTBGen::_make_service_termination(ROSService* service)
{
    if(service->getFinState() == ROSService::FinishState::FinishRemainHere) return "Frh";

    if(service->getExitTime() == QTime())
    {
        throw std::runtime_error("Failed to retrieve Exit time");
    }

    const QTime _exit_time = service->getExitTime();

    const QString _exit_type = _exit_types[service->getFinState()];
    const QString _other = _get_partner(service->getID());

    QString _new_serv, _prev_serv, _exit_id;

    switch(service->getFinState())
    {
        case ROSService::FinishState::FinishExit:
            _exit_id = service->getExitID();
            return join(";", _exit_time.toString("HH:mm"), _exit_type, _exit_id);
            break;
        case ROSService::FinishState::FinishFormNew:
             _new_serv = service->getDaughter();
             if(_new_serv == "") throw std::runtime_error("Could not retrieve daughter service");
             qDebug() << join(";", _exit_time.toString("HH:mm"), _exit_type, _new_serv);
            return join(";", _exit_time.toString("HH:mm"), _exit_type, _new_serv);
            break;
        case ROSService::FinishState::FinishJoinOther:
            return join(";", _exit_time.toString("HH:mm"), _exit_type, _other);
            break;
        case ROSService::FinishState::FinishShuttleFormNew:
            _prev_serv = service->getParent();
            _new_serv = service->getDaughter();
            return join(";", _exit_time.toString("HH:mm"), _prev_serv, _new_serv);
            break;
        case ROSService::FinishState::FinishShuttleRemainHere:
            _prev_serv = service->getParent();
            return join(";", _exit_time.toString("HH:mm"), _exit_type, _new_serv);
            break;
        case ROSService::FinishState::FinishSingleShuttleFeeder:
            _prev_serv = service->getParent();
            return join(";", _exit_time.toString("HH:mm"), _exit_type, _new_serv);
            break;
        default:
            return "NULL";
            break;
     }
}

int ROSTTBGen::_calculate_repeat_time_interval(ROSService* service)
{
    return service->getStartTime().msecsTo(service->getExitTime())/60*1000.;
}

QString ROSTTBGen::_make_repeat_line(ROSService *service)
{
    return join(";", "R", QString::number(service->getRepeatInterval()), QString::number(service->getIDIncrement()), QString::number(service->getNRepeats()));
}

QStringList ROSTTBGen::createTimetableStrings(ROSTimetable* timetable)
{
    QStringList _elements = {timetable->getStartTime().toString("HH:mm")};

    for(auto service : timetable->getServices())
    {
        QString _service = join(",", _make_service_definition(service), _make_type_line(service));
        _service = join(",", _service, join(",", _add_stations(service)));
        _service = join(",", _service, _make_service_termination(service));

        if(service->getNRepeats() > 0)
        {
            _service = join(",", _service, _make_repeat_line(service));
        }

        _elements.push_back(_service);
    }

    return _elements;
}

