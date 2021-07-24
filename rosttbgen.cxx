//-------------------------------------------------------------------------//
//         ROS Timetable Editor File Parser Class Definition               //
//                                                                         //
// This file provides part of the source code towards the standalone       //
// timetable editor constructed using the Qt v5.15 framework.              //
// for Railway Operation Simulator, a railway signalling simulation        //
// by Albert Ball, Copyright (c) 2010 [original development].              //
//                                                                         //
// Copyright (C) 2021 Kristian Zarebski                                    //
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
    QString in_file = file->getOpenFileName(_parent, QObject::tr("Open Timetable"), directory->absolutePath(), QObject::tr("ROS Timetable Files (*.ttb)"));
    if(in_file == QString()) return in_file; // In case user presses 'Cancel'
    QFile open_file(in_file);
    if (!open_file.open(QIODevice::ReadOnly | QFile::Text))
    {
        if(in_file.isEmpty() || in_file.isNull()) return "NULL";
        QMessageBox::critical(_parent, QObject::tr("File Open Failure"), QObject::tr("Failed to open file '")+in_file+"'");
        return QString();
    }

    QTextStream in(&open_file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        bool _record_ttb = false;

        QStringList _parts = line.split(QChar::Null);

        bool _start_time_found = false;

        for(auto& part : _parts)
        {
            if(part.contains(";"))
            {
                _record_ttb = _isID(part.split(";")[0]);

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
                        if(_isTime(part.mid(i, i+5)))
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
    QString _current_route = QFileDialog::getOpenFileName(_parent,  QObject::tr("Open Route"), railways_dir,
                                   QObject::tr("ROS Railway Files (*.rly)"));

    QFile open_file(_current_route);
    if (!open_file.open(QIODevice::ReadOnly | QFile::Text))
            return "NULL";

    open_file.close();

    _parse_rly_stations(_current_route);
    _parse_rly_coordinates(_current_route);


    return _current_route;
}

QSet<QString> ROSTTBGen::_parse_rly_stations(QString route_file)
{

    QStringList _stations = {};
    QFile open_file(route_file);
    if (!open_file.open(QIODevice::ReadOnly | QFile::Text))
        return {};

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
    _stations_list = _stations.toSet();

    open_file.close();

    return _stations_list;
}

QMap<QString, QList<QList<int>>> ROSTTBGen::_parse_rly_coordinates(const QString route_file)
{
    QFile open_file(route_file);

    if (!open_file.open(QIODevice::ReadOnly | QFile::Text))
        return {};

    QTextStream in(&open_file);

    QStringList _file_lines;

    while (!in.atEnd()) {
        QString line = in.readLine();
        _file_lines.push_back(line);
    }

    for(int i{0}; i < _file_lines.size()-1; ++i)
    {
        if(_stations_list.contains(_file_lines[i].replace(QChar::Null, "")) && _file_lines[i+1].contains("**"))
        {
            qDebug() << (_file_lines[i-1] != QString("\n")) << " " << (_file_lines[i-10] != QString("******")+QChar::Null+QString("\n")) << endl;

            if(!_file_lines[i-1].isEmpty() || _file_lines[i-10] != "******")
            {
                continue;
            }

            if(!_coordinates.contains(_file_lines[i].replace(QChar::Null, "")))
            {
                _coordinates[_file_lines[i].replace(QChar::Null, "")] = QList<QList<int>>();
            }

            try {
                _coordinates[_file_lines[i].replace(QChar::Null, "")].push_back(QList<int>({std::stoi(_file_lines[i-7].toStdString()), std::stoi(_file_lines[i-6].toStdString())}));
            }  catch (std::invalid_argument) {
                continue;
            }
        }
    }

    return _coordinates;
}

bool ROSTTBGen::_isTime(QString string)
{
    if(string.size() != 5) return false;
    return string[0].isNumber() && string[1].isNumber() && string[2] == ":" && string[3].isNumber() && string[4].isNumber();
}

bool ROSTTBGen::_isID(QString string)
{
    if(string.size() != 4 || _isTime("0"+string)) return false;

    return true;
}

bool ROSTTBGen::_isInteger(QString string)
{
    for(auto n : string)
    {
        if(!n.isNumber()) return false;
    }

    return true;
}

bool ROSTTBGen::_isPass(QStringList str_list)
{
    if(str_list.contains("pas")) return true;

    return false;
}

bool ROSTTBGen::_isStart(QStringList str_list)
{
    QStringList _types = _start_types.values();
    for(auto& type : _types)
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

bool ROSTTBGen::_isEnd(QStringList str_list)
{
    QStringList _types = _exit_types.values();
    for(auto& type : _types)
    {
        if(str_list.contains(type)) return true;
    }

    return false;
}

bool ROSTTBGen::_isCoordinate(QString string)
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

    return str_list[0] == "R" && _isInteger(str_list[1]) && _isInteger(str_list[2]) && _isInteger(str_list[3]);
}

bool ROSTTBGen::_isCallingPoint(QStringList str_list)
{
    if(str_list.size() != 3) return false;
    return _isTime(str_list[0]) && _isTime(str_list[1]) && str_list[2][0].isLetter();
}

bool ROSTTBGen::_isStartStopPoint(QStringList str_list)
{
    if(str_list.size() != 2) return false;
    return !_isID(str_list[0]) && _isTime(str_list[0]) && str_list[1][0].isLetter() && !_isDirChange(str_list) && !_isPass(str_list);
}

bool ROSTTBGen::_isServiceDefinition(QStringList str_list)
{
    if(str_list.size() != 7) return false;
    return _isID(str_list[0]) && _isInteger(str_list[2]) && _isInteger(str_list[3]) && _isInteger(str_list[4]) && _isInteger(str_list[5]) && _isInteger(str_list[6]);
}

bool ROSTTBGen::_isContinuedServiceDefinition(QStringList str_list)
{
    if(str_list.size() != 2) return false;

    return _isID(str_list[0]);
}

bool ROSTTBGen::_isDirChange(QStringList str_list)
{
    return str_list.size() == 2 && str_list[1] == "cdt";
}

bool ROSTTBGen::_isSplit(QStringList str_list)
{
    return _isTime(str_list[0]) && (str_list[1] == "fsp" || str_list[1] == "rsp");
}

bool ROSTTBGen::_isJoin(QStringList str_list)
{
    return _isTime(str_list[0]) && str_list[1] == "jbo";
}

bool ROSTTBGen::_process_service_candidate(int int_id, QStringList service)
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

    if(_isStart(service[1].split(";")))
    {
        QStringList _components = service[1].split(";");
        _start_time = QTime::fromString(_components[0], "HH:mm");
    }

    else
    {
        QMessageBox::critical(_parent, "Import Failure", "Failed to import service "+QString::number(int_id)+" '"+_id+"'");
        return false;
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

    if(_isStart(service[1].split(";")))
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

    // If the number of components does not match the expected number for the given
    // service termination type, do not attempt to retrieve termination info
    const QString _service_name = _service->getID();
    const QString _glob_msg = QString("Invalid termination statement for service ")+_service_name+". ";

    switch(_fin_state)
    {
        case ROSService::FinishState::FinishRemainHere:
            if(_components.size() != 1)
            {
                const QString _err_msg = _glob_msg + "Expected \"Frh\" for type \"Finish Remain Here\"";
                QMessageBox::critical(_parent, QObject::tr("Invalid service termination"), QObject::tr(_err_msg.toStdString().c_str()));
                break;
            }
            break;
        case ROSService::FinishState::FinishExit:
            if(_components.size() != 3)
            {
                const QString _err_msg = _glob_msg + "Expected \"HH:MM;Fer;Exit ID\" for type \"Finish Exit Railway\"";
                QMessageBox::critical(_parent, QObject::tr("Invalid service termination"), QObject::tr(_err_msg.toStdString().c_str()));
                break;
            }

            _service->setExitPoint(_components[2]);
            break;
        case ROSService::FinishState::FinishFormNew:
            if(_components.size() != 3)
            {
                const QString _err_msg = _glob_msg + "Expected \"HH:MM;Fns;Service ref\" for type \"Finish Form New\"";
                QMessageBox::critical(_parent, QObject::tr("Invalid service termination"), QObject::tr(_err_msg.toStdString().c_str()));
                break;
            }
            _service->setDaughter(_components[2]);
            break;
        case ROSService::FinishState::FinishJoinOther:
            if(_components.size() != 3)
            {
                const QString _err_msg = _glob_msg + "Expected \"HH:MM;Fjo;Service ref\" for type \"Finish Join Other\"";
                QMessageBox::critical(_parent, QObject::tr("Invalid service termination"), QObject::tr(_err_msg.toStdString().c_str()));
                break;
            }
            _service->setDaughter(_components[2]);
            break;
        case ROSService::FinishState::FinishShuttleFormNew:
            if(_components.size() != 4)
            {
                const QString _err_msg = _glob_msg + "Expected \"HH:MM;Fns-sh;Shuttle ref;Finish ref\" for type \"Finish Shuttle Form New\"";
                QMessageBox::critical(_parent, QObject::tr("Invalid service termination"), QObject::tr(_err_msg.toStdString().c_str()));
                break;
            }
            _service->setParent(_components[2]);
            _service->setDaughter(_components[3]);
            break;
        case ROSService::FinishState::FinishShuttleRemainHere:
            if(_components.size() != 3)
            {
                const QString _err_msg = _glob_msg + "Expected \"HH:MM;Frh-sh;Shuttle ref\" for type \"Finish Shuttle Remain Here\"";
                QMessageBox::critical(_parent, QObject::tr("Invalid service termination"), QObject::tr(_err_msg.toStdString().c_str()));
                break;
            }
            _service->setParent(_components[2]);
            break;
        case ROSService::FinishState::FinishSingleShuttleFeeder:
            if(_components.size() != 3)
            {
                const QString _err_msg = _glob_msg + "Expected \"HH:MM;F-nshs;Shuttle ref\" for type \"Finish Single Shuttle Feeder\"";
                QMessageBox::critical(_parent, QObject::tr("Invalid service termination"), QObject::tr(_err_msg.toStdString().c_str()));
                break;
            }
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

    // Parse Locations
    for(int i{2}; i < service.size()-_final_index; ++i)
    {
        bool _pass_point = false;

        _service->setLabelledLocationStart(i == 2 && service[i].split(";").size() == 2);

        if(_service->getStations().size() > i-1 && _isDirChange(service[i].split(";")))
        {
            _service->setDirectionChangeAtStop(i-1, true, QTime::fromString(service[i].split(";")[0], "HH:mm"));
        }

        else if(_isSplit(service[i].split(";")))
        {
            QStringList _components = service[i].split(";");
            const QStringList _stations = _service->getStations();
            _service->setSplitData(_components[1], _components[2], _stations[_service->getStations().size()-1], _components[0]);
        }
        else if(_isCallingPoint(service[i].split(";")))
        {
            QStringList _components = service[i].split(";");
            _service->addStation({QTime::fromString(_components[0].replace("W", ""), "HH:mm"),  // 'W' refers to legacy feature in older ROS
                                 QTime::fromString(_components[0].replace("W", ""), "HH:mm")},  // and referred to warning events, this is now deprecated
                                 _components[2]);                                               // with intro of action window. Added removal here for legacy timetable support.
        }

        else if(_isStartStopPoint(service[i].split(";")))
        {
            QStringList _components = service[i].split(";");
            _service->addStation({QTime::fromString(_components[0].replace("W", ""), "HH:mm"),
                                 QTime()},
                                 _components[1]);
        }

        else if(_isPass(service[i].split(";")))
        {
            QStringList _components = service[i].split(";");
            _service->addStation({QTime::fromString(_components[0].replace("W", ""), "HH:mm"), QTime()},
                                 _components[2]);
            _service->setStopAsPassPoint(_service->getStations().size()-1, _pass_point);

        }

        else if(_isJoin(service[i].split(";")))
        {
            QStringList _components = service[i].split(";");
            const QStringList _stations = _service->getStations();
            if(_stations.size() < 1) continue;
            _service->setJoinData(_components[2], _stations[_stations.size()-1], _components[0].replace("W", ""));
        }

        else if(_isDirChange(service[i].split(";")))
        {
            if(_service->getStations().size() == 0)
            {
                _service->appendDirectionChange(true, QTime::fromString(service[i].split(";")[0], "HH:mm"));
            }
            else
            {
                _service->setDirectionChangeAtStop(_service->getStations().size()-1, true, QTime::fromString(service[i].split(";")[0], "HH:mm"));
            }
        }
    }

    if(_isDirChange(service[service.size()-1].split(";")))
    {
        if(_service->getStations().size() == 0)
        {
            _service->appendDirectionChange(true, QTime::fromString(service[service.size()-1].split(";")[0], "HH:mm"));
        }
        else
        {
            _service->setDirectionChangeAtStop(_service->getStations().size()-1, true, QTime::fromString(service[service.size()-1].split(";")[0], "HH:mm"));
        }
    }

    _current_timetable->addService(_service);

    return true;
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

    QStringList _rcd_srv_ids;
    for(auto service : _services) _rcd_srv_ids.append(service[0].split(";")[0]);

    // Handle the Case Where Service IDs contain Duplicates in Read-In File
    // attempts to resolve this by changing the ID, else it ignores the duplicate

    QList<int> _ignored_indexes; // Indexes of services to ignore (unhandled duplicates)

    for(auto id : _rcd_srv_ids) // Only need to iterate through half the list for dupe check
    {
        if(_rcd_srv_ids.count(id) > 1)
        {
            QStringList _srv_1 = _services[_rcd_srv_ids.indexOf(id)][0].split(";");
            QStringList _srv_2 = _services[_rcd_srv_ids.indexOf(id, _rcd_srv_ids.indexOf(id)+1)][0].split(";");
            if(_srv_1[0] == _srv_2[0] && _srv_1[1] == _srv_1[1]) // Check if both same definition and same start conditions
            {
                // Complete Duplicate so ignore
                QMessageBox::critical(_parent, "Duplicate Service", "Service "+id+" is duplicated! Ignoring Duplicate");
                // Fetch Second Instance
                _ignored_indexes.append(_rcd_srv_ids.indexOf(id, _rcd_srv_ids.indexOf(id)+1));
                _services[_rcd_srv_ids.indexOf(id)][0].replace(id, id.mid(0,2)+"XX"); // Have to rename it so it is not processed twice
                break;
            }

            // Handle case of mistype (ID same but definition is different)
            QMessageBox::critical(_parent, "Duplicate Service", "Service ID "+id+" is duplicated! Attempting ID change");
            if(id[2].isNumber() && id[3].isNumber())
            {
                int _num = id.midRef(2,3).toInt();
                bool _new_id_found = false;
                while(!_new_id_found)
                {
                    _num++;
                    if(!_rcd_srv_ids.contains(id.mid(0,1)+QString::number(_num)))
                    {
                        QMessageBox::information(_parent, "Duplicate Service", "Service "+id+" now "+id.mid(0,2)+QString::number(_num));
                        _services[_rcd_srv_ids.indexOf(id)][0].replace(id, id.mid(0,2)+QString::number(_num));
                        _rcd_srv_ids[_rcd_srv_ids.indexOf(id)] = id.mid(0,2)+QString::number(_num);
                        break;
                    }
                    else if (_num == 99)
                    {
                        QMessageBox::critical(_parent, "Failed ID Update", "ID Change Failed");
                        _ignored_indexes.append(_rcd_srv_ids.indexOf(id));
                        break;
                    }
                }
            }
        }
    }

    for(int i{0}; i < _services.size(); ++i)
    {
        if(_ignored_indexes.contains(i)) continue;  // Ignore unprocessed Duplicates
        if(!_process_service_candidate(i, _services[i])) return;
    }

    if(_current_timetable->size() != _services.size()-_ignored_indexes.size())
    {
        QMessageBox::critical(_parent, "Failed Service Importing", "Number of services created ("+QString::number(_services.size())+") does not match number in input file ("+QString::number(_current_timetable->size())+")");
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

    const QStringList _start_point = service->getStartPoint();

    switch(service->getType())
    {
        case ROSService::ServiceType::Service:
            return join(";", _start_time, _start_type, _start_point[0]+" "+_start_point[1]);
        case ROSService::ServiceType::ShuttleFromStop:
            return join(";", _start_time, _start_type, _start_point[0], _start_point[1], service->getDaughter());
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

    const QList<QList<QTime>> _times =  service->getTimes();
    const QStringList _stations = service->getStations();
    const QList<bool> _passes = service->getPassList();
    const QMap<QString, QStringList> _joins = service->getJoinData();
    const QList<bool> _dirs = service->getDirectionChanges();
    const QList<QTime> _cdt_times = service->getCDTTimes();

    // Check if service is service starts from a location or if it is started from another service as this will mean
    // the first station timetable entry should only contain a single time
    if(service->labelledLocationStart() || service->getType() == ROSService::ServiceType::ShuttleFromFeeder || service->getType() == ROSService::ServiceType::ServiceFromService)
    {
        if(service->getTimes().size() == 0)
        {
            QMessageBox::critical(_parent, "BadAccessTimes", "Attempt to access service times array which is size zero by index");
            return {};
        }
        const QTime _depart = _times[0][1];
        _temp.append(join(";", _depart.toString("HH:mm"), _stations[0]));
    }

    for(int i{int_start}; i < int_final; ++i)
    {
        const QTime _arrive = _times[i][0],
              _depart = _times[i][1];
        const QString _name = _stations[i];
        const bool _is_pas = _passes[i],
        _is_cdt = _dirs[i],
        _is_join = _joins.size() > 0 && _name == _joins[_joins.keys()[0]][0];

        const QMap<QString, QStringList> _split = service->getSplitData();

        if(_is_pas)
        {
            _temp.append(join(";", _arrive.toString("HH:mm"), "pas", _name));
        }
        else
        {
            _temp.append(join(";", (_depart != QTime()) ? _arrive.toString("HH:mm")+";"+_depart.toString("HH:mm") : _arrive.toString("HH:mm"), _name));
        }

        if(_split.size() > 0 && _split[_split.keys()[0]][1] == _stations[i])
        {
            const QString _type = _split.keys()[0];
            const QStringList _data = _split[_type];
            _temp.append(join(";", _data[2], _type, _data[0]));
        }
        if(_is_join)
        {
            const QString _id = _joins.keys()[0], _time = _joins[_id][1];
            _temp.append(join(";", _time, "jbo", _id));
        }
        if(_is_cdt)
        {
            const QTime _cdt_time = _cdt_times[i];
            _temp.append(join(";", (_cdt_time != QTime()) ? _cdt_time.toString("HH:mm") : _arrive.toString("HH:mm"), "cdt"));
        }

    }

    if(service->getFinState() != ROSService::FinishState::FinishExit)
    {     
        const QTime _arrive = _times[int_final][0];
        _temp.append(join(";", _arrive.toString("HH:mm"), _stations[int_final]));


        const bool _is_cdt = _dirs[int_final];

        if(_is_cdt)
        {
            if(_times.size() == 0)
            {
                QMessageBox::critical(_parent, "BadAccessTimes", "Attempt to access service times array which is size zero by index");
                return {};
            }
            const QTime _arrive = _times[int_final][0],
                          _depart = _times[int_final][1];
            const QTime _cdt_time = _cdt_times[int_final];

            _temp.append(join(";", (_depart != QTime()) ? _cdt_time.toString("HH:mm") : _arrive.toString("HH:mm"), "cdt"));
            service->setExitTime(_cdt_time);
        }
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
        QMessageBox::critical(_parent, "Termination Failure", "Failed to Obtain a Termination Time for Service '"+service->getID()+"'.");
        return QString();
    }

    const QTime _exit_time = service->getExitTime();

    const QString _exit_type = _exit_types[service->getFinState()];
    const QString _other = (service->getJoinData().keys().size() > 0) ? _get_partner(service->getID()) : "";

    QString _new_serv, _prev_serv, _exit_id;

    switch(service->getFinState())
    {
        case ROSService::FinishState::FinishExit:
            _exit_id = service->getExitID();
            return join(";", _exit_time.toString("HH:mm"), _exit_type, _exit_id);
            break;
        case ROSService::FinishState::FinishFormNew:
             _new_serv = service->getDaughter();
             if(_new_serv == "")
             {
                 QMessageBox::critical(_parent, QObject::tr("Invalid child service"), QObject::tr("Could not retrieve daughter service"));
                 return "";
             }
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

QString ROSTTBGen::_make_repeat_line(ROSService *service)
{
    return join(";", "R", QString::number(service->getRepeatInterval()), QString::number(service->getIDIncrement()), QString::number(service->getNRepeats()));
}

QStringList ROSTTBGen::createTimetableStrings(ROSTimetable* timetable)
{
    QStringList _elements = {timetable->getStartTime().toString("HH:mm")};

    for(auto service : timetable->getServices())
    {
        if(service->getExitTime() == QTime())
        {
            QMessageBox::warning(_parent, "Incomplete Service", "No exit time specified for service '"+service->getID()+"', this service will be ignored during save. Please add at least two locations or specify a manual termination time.");
            continue;
        }
        QString _service = join(",", _make_service_definition(service), _make_type_line(service));
        const QStringList _call_points = _add_stations(service);
        if(service->getStations().size() > 0 && _call_points.size() > 0)_service = join(",", _service, join(",", _call_points));
        QString _term = _make_service_termination(service);
        if(_term == QString())
        {
            QMessageBox::warning(_parent, "Incomplete Service", "Service '"+service->getID()+"' is incomplete and will not be saved.");
            continue;
        }
        _service = join(",", _service, _term);
        if(service->getNRepeats() > 0) _service = join(",", _service, _make_repeat_line(service));

        _elements.push_back(_service);
    }

    return _elements;
}

