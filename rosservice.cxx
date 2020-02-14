#include "rosservice.hxx"

QString join(QString join_symbol, QString a, QString b, QString c, QString d, QString e, QString f, QString g, QString h)
{
    if(a == "") return b;
    if(b == "") return "";
    QString _temp = a+join_symbol+b;

    if(c != "NULL") _temp += join_symbol+c;
    if(d != "NULL") _temp += join_symbol+d;
    if(e != "NULL") _temp += join_symbol+e;
    if(f != "NULL") _temp += join_symbol+f;
    if(g != "NULL") _temp += join_symbol+g;
    if(h != "NULL") _temp += join_symbol+h;

    return _temp;
}

QString join(QString join_symbol, QStringList list)
{
    QString _temp = list[0];

    for(int i{1}; i < list.size(); ++i) _temp += join_symbol+list[i];

    return _temp;
}


QString join(QChar::SpecialCharacter join_symbol, QStringList list)
{
    QString _temp = list[0];

    for(int i{1}; i < list.size(); ++i) _temp += join_symbol+list[i];

    return _temp;
}

void ROSService::shiftStationTimes(const int n_secs, QString &station)
{
    const int index = _stations.indexOf(station);
    if(index < 0) return;
    _times[index][0] = _times[index][0].addSecs(n_secs);
    _times[index][1] = _times[index][1].addSecs(n_secs);
}

void ROSService::shiftServiceTimes(const int n_secs)
{
    if(_parent_service != "") return;
    _enter_map_time = _enter_map_time.addSecs(n_secs);
    if(_exit_map_time != QTime()) _exit_map_time = _exit_map_time.addSecs(n_secs);
    for(auto s : _stations) shiftStationTimes(n_secs, s);
    for(int i{0}; i < _cdt_times.size(); ++i) if(_cdt_times[i] != QTime()) _cdt_times[i] = _cdt_times[i].addSecs(n_secs);
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

QString ROSService::_add_stops()
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
            out_string = join(";", _times[_times.size()-1][0].toString(), "Fer", _exit_id);
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
    _out = join(",", _out, _add_stops());
    _out = join(",", _out, _finalize());

    return _out;
}

void ROSService::setFinishState(ROSService::FinishState fin_state)
{
    _finish_as = fin_state;
}
void ROSService::setShuttleRefPosition(QString coordinates[2])
{
    _service_type = ServiceType::ShuttleFromStop;
    _enter_ids[0] = coordinates[0];
    _enter_ids[1] = coordinates[1];

}
