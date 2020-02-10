#ifndef ROSSERVICE_HXX
#define ROSSERVICE_HXX

#include <QString>
#include <QTime>
#include <QMap>
#include <QDebug>

class ROSService
{
private:
    int _integer_id = 0;
    QTime _enter_map_time = QTime();
    QTime _exit_map_time = QTime();
    QStringList _enter_ids = {"", ""};
    QString _exit_id = "";
    QString _daughter_id = "";
    QString _service_id = "";
    QList<bool> _passing_stops = {};
    QList<bool> _direction_changes = {};
    QList<QString> _stations = {};
    QString _parent_service = "";
    QList<QList<QTime>> _times = {};
    int _power = -1;
    int _mass= -1;
    int _max_speed = -1;
    int _max_brake = -1;
    int _start_speed = -1;
    int _n_repeats = 0;
    int _nsecs_wait_turnaround = 0;
    int _id_increment = 1;
    QString _description = "";
    QString _start_new();
    QString _add_stops();
    QString _finalize();
    QString _join(QString join_symbol, QString a, QString b, QString c="NULL", QString d="NULL", QString e="NULL", QString f="NULL", QString g="NULL", QString h="NULL");
public:
    enum class ServiceType
    {
        Service,
        ServiceFromSplit,
        ServiceFromService,
        SingleShuttleService,
        ShuttleFromStop,
        ShuttleFromFeeder,
    };

    enum class FinishState
    {
        FinishRemainHere,
        FinishFormNew,
        FinishJoinOther,
        FinishExit,
        FinishShuttleRemainHere,
        FinishShuttleFormNew,
        FinishSingleShuttleFeeder,
    };
    ROSService(int int_id, QTime start_time, QString id, QString description, QList<QString> stations = {}, QList<QList<QTime>> times = {},
               int start_speed=-1, int max_speed=-1, int mass=-1, int max_brake_force=-1, int power=-1) :
        _integer_id(int_id), _enter_map_time(start_time), _service_id(id), _stations(stations), _power(power), _mass(mass),
        _max_speed(max_speed), _max_brake(max_brake_force), _start_speed(start_speed), _description(description), _times(times)  {}
    void setNRepeats(const int& n) { _n_repeats = n;}
    int getIntegerID() const { return _integer_id;}
    void setIDIncrement(const int& n) {_id_increment = n;}
    void setTurnaroundTime(const int& n_secs) {_nsecs_wait_turnaround = n_secs;}
    void setMaxSpeed(const int& speed){_max_speed = speed;}
    void setMaxBrake(const int& force){_max_brake = force;}
    void setStartSpeed(const int& speed){_start_speed = speed;}
    void setMass(const int& mass){_mass = mass;}
    void setPower(const int& power) {_power = power;}
    void setExitPoint(QString& id) {_exit_id = id;}
    void setEntryPoint(QStringList& ids) {_enter_ids = ids;}
    void setExitTime(QTime& time) {_exit_map_time = time;}
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
    void setDaughter(QString daughter_service)
    {
        _daughter_id = daughter_service;
    }
    void addStation(QList<QTime> time, QString station)
    {
        _times.push_back(time);
        _stations.push_back(station);
        _passing_stops.push_back(false);
        _direction_changes.push_back(false);
    }
    void setStopAsPassPoint(int index, bool state) {_passing_stops[index] = state;}
    void setDirectionChangeAtStop(int index, bool state) {_direction_changes[index] = state;}
    void setShuttleRefPosition(QString coordinates[2]);
    QString as_string();
    QStringList summarise();
    const QList<QString> getStations() const
    {
        return _stations;
    }
    const QList<QList<QTime>> getTimes() const
    {
        return _times;
    }
    const QString getID() const {return _service_id; }

private:
    ServiceType _service_type = ServiceType::Service;
    FinishState _finish_as = FinishState::FinishRemainHere;
};

#endif // ROSSERVICE_HXX
