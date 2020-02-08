#ifndef ROSSERVICE_HXX
#define ROSSERVICE_HXX

#include <QString>
#include <QTime>
#include <QMap>

class ROSService
{
    enum class ServiceType
    {
        Service,
        ServiceFromSplit,
        ServiceFromService,
        SingleShuttleService,
        ShuttleFromStop,
        ShuttleFromFeeder
    };

    enum class FinishState
    {
        FinishRemainHere,
        FinishFormNew,
        FinishJoinOther,
        FinishExit,
    };

private:
    QTime _enter_map_time;
    ServiceType _service_type = ServiceType::Service;
    FinishState _finish_as = FinishState::FinishRemainHere;
    QString _enter_ids[2]= {"",""};
    QString _exit_id = "";
    QString _daughter_id = "";
    QString _service_id = "";
    QList<bool> _passing_stops = {};
    QList<QString> _stations = {};
    QString _parent_service = "";
    QString _shuttle_pos_ref_ids[2];
    QMap<int, QList<QTime>> _times = {};
    bool _change_dir_at_end = false;
    int _power = -1;
    int _mass= -1;
    int _max_speed = -1;
    int _max_brake = -1;
    int _start_speed = -1;
    QString _description = "";
    QString _start_new();
    QString _add_stops();
    QString _finalize();
    QString _join(QString join_symbol, QString a, QString b, QString c="NULL", QString d="NULL", QString e="NULL", QString f="NULL", QString g="NULL", QString h="NULL");
public:
    ROSService(QTime start_time, QString id, QList<QString> stations = {},
               int start_speed=-1, int max_speed=-1, int mass=-1, int max_brake_force=-1, int power=-1) :
        _enter_map_time(start_time), _service_id(id), _stations(stations), _power(power), _mass(mass),
        _max_speed(max_speed), _max_brake(max_brake_force), _start_speed(start_speed) {}
    void setMaxSpeed(const int& speed){_max_speed = speed;}
    void setMaxBrake(const int& force){_max_brake = force;}
    void setStartSpeed(const int& speed){_start_speed = speed;}
    void setMass(const int& mass){_mass = mass;}
    void setPower(const int& power) {_power = power;}
    void setDescription(const QString& desc) {_description = desc;}
    void orderService();
    int getMaxSpeed() const {return _max_speed;}
    int getMaxBrake() const {return _max_brake;}
    int getStartSpeed() const {return _start_speed;}
    int getMass() const {return _mass;}
    int getPower() {return _power;}
    bool checkService();
    void setType(ROSService::ServiceType type) {_service_type = type;}
    void setFinishState(ROSService::FinishState fin_state, QString exit_info="");
    void setParent(QString parent_service)
    {
        _parent_service = parent_service;
    }
    void addStation(QList<QTime> time, QString station)
    {
        _times[_times.size()] = time;
        _stations.push_back(station);
        _passing_stops.push_back(false);
    }
    void setStopAsPassPoint(int index, bool state) {_passing_stops[index] = state;}
    void setChangeDirAtTerminus() {_change_dir_at_end = true;}
    void setShuttleRefPosition(QString coordinates[2]);
    QString as_string();
    QString summarise();
    const QList<QString> getStations() const
    {
        return _stations;
    }
    const QMap<int, QList<QTime>> getTimes() const
    {
        return _times;
    }
    const QString getID() const {return _service_id; }
};

#endif // ROSSERVICE_HXX
