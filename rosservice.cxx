#include "rosservice.hxx"

QString ROSService::_join(QString join_symbol, QString a, QString b, QString c, QString d, QString e, QString f, QString g, QString h)
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

QString ROSService::_start_new()
{
    QString out_string = _join(";", _service_id,_description, QString(_start_speed), QString(_max_speed),
                        QString(_mass), QString(_max_brake), QString(_power));

    switch (_service_type)
    {
        case ROSService::ServiceType::ServiceFromSplit:
            out_string = _join(",", out_string, _join(";", _enter_map_time.toString("HH:mm"), "Sfs", _parent_service));
            break;
        case ROSService::ServiceType::ServiceFromService:
            out_string = _join(",", out_string, _join(";", _enter_map_time.toString("HH:mm"), "Sns", _parent_service));
            break;
        case ROSService::ServiceType::SingleShuttleService:
            out_string = _join(",", out_string, _join(";", _enter_map_time.toString("HH:mm"), "Sns-fsh", _parent_service));
            break;
        case ROSService::ServiceType::ShuttleFromStop:
            out_string = _join(",", out_string, _join(";", _enter_map_time.toString("HH:mm"), "Snt-sh", _shuttle_pos_ref_ids[0]));
            out_string = _join(" ", out_string, _join(";", _shuttle_pos_ref_ids[1], _parent_service));
            break;
        case ROSService::ServiceType::ShuttleFromFeeder:
            out_string = _join(",", out_string, _join(";", _enter_map_time.toString("HH:mm"), "Sns-sh", _parent_service));
            break;
        default:
            out_string = _join(",", out_string, _join(";", _enter_map_time.toString("HH:mm"), "Sns-fsh", _parent_service));
    }

    return out_string;
}

QString ROSService::_add_stops()
{

    if(_stations.size() < 2) return "";

    QString out_string = _join(";", _times[0][0].toString("HH:mm"), _stations[0]);

    for(int i{1}; i < _stations.size()-2; ++i)
    {
        out_string = _join(",", out_string, _join(";", (_times[i][1] != QTime()) ? _join(";", _times[i][0].toString("HH:mm"), _times[i][1].toString("HH:mm")) : _times[i][0].toString("HH:mm"), _stations[i]));
    }

    if(_finish_as == FinishState::FinishExit)
    {
        const int final = _stations.size()-2;
        out_string = _join(",", out_string, _join(";", _times[final][0].toString("HH:mm"),_times[final][1].toString("HH:mm"), _stations[final]));
    }

    else
    {
        const int final = _stations.size()-2;
        out_string = _join(",", out_string, _join(";", _times[final][0].toString("HH:mm"), _stations[final]));
    }

    return out_string;
}

QString ROSService::summarise()
{
    const QString _start = (_stations.size() < 1) ? "" : _stations[0],
                  _finish = (_stations.size() < 2) ? "" : _stations[_stations.size()-1];
    return _join("\t", _enter_map_time.toString("HH:mm"), _service_id, _description, _start, _finish);
}

QString ROSService::_finalize()
{
    QString out_string = "";

    switch(_finish_as)
    {
        case ROSService::FinishState::FinishExit:
            out_string = _join(";", _times[_times.size()-1][0].toString(), "Fer", _exit_id);
            break;
        case ROSService::FinishState::FinishFormNew:
            out_string = _join(";", _times[_times.size()-1][0].toString(), "Fns", _daughter_id);
            break;
        case ROSService::FinishState::FinishJoinOther:
            out_string = _join(";", _times[_times.size()-1][0].toString(), "Fjo", _daughter_id);
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
    _out = _join(",", _out, _add_stops());
    _out = _join(",", _out, _finalize());

    return _out;
}

void ROSService::setFinishState(ROSService::FinishState fin_state, QString exit_info)
{
    _finish_as = fin_state;

    switch (_finish_as)
    {
        case FinishState::FinishExit:
            _exit_id = exit_info;
            break;
        case FinishState::FinishFormNew:
            _daughter_id = exit_info;
            break;
        case FinishState::FinishJoinOther:
            _daughter_id = exit_info;
            break;
        default:
            break;
    }
}
void ROSService::setShuttleRefPosition(QString coordinates[2])
{
    _service_type = ServiceType::ShuttleFromStop;
    _shuttle_pos_ref_ids[0] = coordinates[0];
    _shuttle_pos_ref_ids[1] = coordinates[1];

}

void ROSService::orderService()
{
    QList<bool> _pass_temp = {};
    QList<QString> _stat_temp = {};
    QMap<int, QList<QTime>> _times_temp = {{}};

    QList<int> _ordered_index = {};

    for(int i{0}; i < _times_temp.size(); ++i)
    {
        if(i == 0)
        {
            _ordered_index.push_back(i);
        }

        else
        {
            for(auto j : _ordered_index)
            {
                if(_times[i][0] < _times[j][0])
                {
                    _ordered_index.insert(j, i);
                    break;
                }
            }
        }

    }

    for(int i{0}; i < _passing_stops.size(); ++i)
    {
        _pass_temp[i] = _passing_stops[_ordered_index[i]];
        _stat_temp[i] = _stations[_ordered_index[i]];
        _times_temp[i] = _times[_ordered_index[i]];
    }

    _passing_stops = _pass_temp;
    _stations = _stat_temp;
    _times = _times_temp;
}
