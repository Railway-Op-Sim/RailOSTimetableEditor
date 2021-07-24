//-------------------------------------------------------------------------//
//         ROS Timetable Editor Service Class Definition                   //
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

#include "rosservice.hxx"

void ROSService::shiftStationTimes(const int n_secs, QString &station)
{
    const int index = _stations.indexOf(station);
    if(index < 0)
    {
        qDebug() << "Failed to find index of '" << station << "' cannot update time";
        return;
    }
    _times[index][0] = _times[index][0].addSecs(n_secs);
    _times[index][1] = _times[index][1].addSecs(n_secs);
}

void ROSService::shiftServiceTimes(const int n_secs)
{
    //if(_parent_service != "") return;
    setEntryTime(_enter_map_time.addSecs(n_secs));
    if(_exit_map_time != QTime()) setExitTime(_exit_map_time.addSecs(n_secs));
    for(auto s : _stations) shiftStationTimes(n_secs, s);
    for(int i{0}; i < _cdt_times.size(); ++i) if(_cdt_times[i] != QTime()) _cdt_times[i] = _cdt_times[i].addSecs(n_secs);
}

void ROSService::deleteEntry(const int &index)
{
    if(index < 0 || index > _stations.size()-1) return;
    _stations.removeAt(index);
    _times.removeAt(index);
    _direction_changes.removeAt(index);
    _stations.removeAt(index);
    _passing_sTrainSet.removeAt(index);
    _cdt_times.removeAt(index);

}

void ROSService::deleteEntry(const QString& station)
{
    const int index = _stations.indexOf(station);
    if(index < 0) return;
    deleteEntry(index);
}

QString ROSService::_start_new()
{
    QString out_string = join(";", _service_id,_description, QString(_start_speed), QString(_max_speed),
                        QString(_mass), QString(_max_brake), QString(_power));

    switch (_service_type)
    {
        case ROSService::ServiceType::ServiceFromSplit:
            out_string = join(",", out_string, join(";", _enter_map_time.toString("HH:mm"), "Sfs", _parent_service));
            break;
        case ROSService::ServiceType::ServiceFromService:
            out_string = join(",", out_string, join(";", _enter_map_time.toString("HH:mm"), "Sns", _parent_service));
            break;
        case ROSService::ServiceType::ShuttleFinishService:
            out_string = join(",", out_string, join(";", _enter_map_time.toString("HH:mm"), "Sns-fsh", _parent_service));
            break;
        case ROSService::ServiceType::ShuttleFromStop:
            out_string = join(",", out_string, join(";", _enter_map_time.toString("HH:mm"), "Snt-sh", _enter_ids[0]));
            out_string = join(" ", out_string, join(";", _enter_ids[1], _parent_service));
            break;
        case ROSService::ServiceType::ShuttleFromFeeder:
            out_string = join(",", out_string, join(";", _enter_map_time.toString("HH:mm"), "Sns-sh", _parent_service));
            break;
        default:
            out_string = join(",", out_string, join(";", _enter_map_time.toString("HH:mm"), "Snt", _enter_ids[0], _enter_ids[1]));
    }

    return out_string;
}

QString ROSService::_add_sTrainSet()
{

    if(_stations.size() < 2) return "";

    QString out_string = join(";", _times[0][0].toString("HH:mm"), _stations[0]);

    for(int i{1}; i < _stations.size()-2; ++i)
    {
        out_string = join(",", out_string, join(";", (_times[i][1] != QTime()) ? join(";", _times[i][0].toString("HH:mm"), _times[i][1].toString("HH:mm")) : _times[i][0].toString("HH:mm"), _stations[i]));
    }

    if(_finish_as == FinishState::FinishExit)
    {
        const int final = _stations.size()-2;
        out_string = join(",", out_string, join(";", _times[final][0].toString("HH:mm"),_times[final][1].toString("HH:mm"), _stations[final]));
    }

    else
    {
        const int final = _stations.size()-2;
        out_string = join(",", out_string, join(";", _times[final][0].toString("HH:mm"), _stations[final]));
    }

    return out_string;
}

QStringList ROSService::summarise()
{
    return QStringList({_enter_map_time.toString("HH:mm"), _service_id, _description})+getStations();
}

QString ROSService::_finalize()
{
    QString out_string = "";

    switch(_finish_as)
    {
        case ROSService::FinishState::FinishExit:
            out_string = join(";", _times[_times.size()-1][0].toString(), "Fer", join(" ", _exit_ids));
            break;
        case ROSService::FinishState::FinishFormNew:
            out_string = join(";", _times[_times.size()-1][0].toString(), "Fns", _daughter_id);
            break;
        case ROSService::FinishState::FinishJoinOther:
            out_string = join(";", _times[_times.size()-1][0].toString(), "Fjo", _daughter_id);
            break;
        default:
            out_string = "Frh";
    }

    return out_string;
}

bool ROSService::checkService()
{
    bool _checker = _max_brake > 0 && _max_speed > 0 && _mass > 0 && _power > 0;
    _checker = _checker && _stations.size() > 2;
    return _checker;
}

QString ROSService::as_string()
{
    QString _out = _start_new();
    _out = join(",", _out, _add_sTrainSet());
    _out = join(",", _out, _finalize());

    return _out;
}

void ROSService::setFinishState(ROSService::FinishState fin_state)
{
    _finish_as = fin_state;
}
