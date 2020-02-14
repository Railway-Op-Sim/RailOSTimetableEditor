#ifndef ROSSERVICE_HXX
#define ROSSERVICE_HXX

#include <QString>
#include <QTime>
#include <QMap>
#include <QDebug>

extern QString join(QString join_symbol, QString a, QString b, QString c="NULL", QString d="NULL", QString e="NULL", QString f="NULL", QString g="NULL", QString h="NULL");
extern QString join(QString join_symbol, QStringList list);
extern QString join(QChar::SpecialCharacter join_symbol, QStringList list);

class ROSService
{
private:
    int _integer_id = 0;
    QTime _enter_map_time = QTime();
    QTime _exit_map_time = QTime();
    QStringList _enter_ids = {"", ""};
    QString _exit_id = "";
    QList<QTime> _cdt_times = {};
    QString _daughter_id = "";
    QString _service_id = "";
    QList<bool> _passing_stops = {};
    QList<bool> _direction_changes = {};
    QMap<QString, QStringList> _split_data = QMap<QString, QStringList>();
    QStringList _stations = QStringList();
    QString _parent_service = "";
    QList<QList<QTime>> _times = QList<QList<QTime>>();
    int _power = -1;
    int _mass= -1;
    int _max_speed = -1;
    int _max_brake = -1;
    int _start_speed = -1;
    int _n_repeats = 0;
    int _repeat_interval = 0;
    int _id_increment = 1;
    QString _description = "";
    QString _start_new();
    QString _add_stops();
    QString _finalize();
public:
    enum class ServiceType
    {
        Service,
        ServiceFromSplit,
        ServiceFromService,
        ShuttleFinishService,
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
    ROSService(int int_id, QTime start_time, QString id, QString description, int start_speed=-1, int max_speed=-1, int mass=-1, int max_brake_force=-1, int power=-1) :
        _integer_id(int_id), _enter_map_time(start_time), _service_id(id), _power(power), _mass(mass), _max_speed(max_speed), _max_brake(max_brake_force), _start_speed(start_speed), _description(description)  {}
    void setNRepeats(const int& n) { _n_repeats = n;}
    int getIntegerID() const { return _integer_id;}
    void shiftStationTimes(const int n_secs, QString& station);
    void shiftServiceTimes(const int n_secs);
    void setIDIncrement(const int& n) {_id_increment = n;}
    void setRepeatInterval(const int& n_mins) {_repeat_interval = n_mins;}
    void setMaxSpeed(const int& speed){_max_speed = speed;}
    void setMaxBrake(const int& force){_max_brake = force;}
    void setStartSpeed(const int& speed){_start_speed = speed;}
    void setMass(const int& mass){_mass = mass;}
    void setID(const QString& id) {_service_id = id;}
    void setPower(const int& power) {_power = power;}
    void setSplitData(const QString& fr_rear, const QString& service_id, const QString& station, const QString& time)
    {
        _split_data = {{fr_rear, {service_id, station, time}}};
    }
    QMap<QString, QStringList> getSplitData() const {return _split_data;}
    void setExitPoint(const QString& id) {_exit_id = id;}
    void setEntryPoint(const QStringList& ids) {_enter_ids = ids;}
    void setExitTime(const QTime& time) {_exit_map_time = time;}
    void setDescription(const QString& desc) {_description = desc;}
    void setEntryTime(const QTime& time) {_enter_map_time = time;}
    QList<bool> getCDTPass(const QString& station_name)
    {
        for(int i{0}; i < _stations.size(); ++i)
        {
            if(_stations[i] == station_name)
            {
                return {_direction_changes[i], _passing_stops[i]};
            }
        }

        return {false, false};
    }
    int getMaxSpeed() const {return _max_speed;}
    int getMaxBrake() const {return _max_brake;}
    int getStartSpeed() const {return _start_speed;}
    int getMass() const {return _mass;}
    int getPower() const {return _power;}
    QStringList getStartPoint() const {return _enter_ids;}
    QTime getStartTime() const {return _enter_map_time;}
    QTime getExitTime() const {return _exit_map_time;}
    QString getDescription() const {return _description;}
    QString getDaughter() const {return _daughter_id;}
    QString getParent() const {return _parent_service;}
    QString getExitID() const {return _exit_id;}
    int getNRepeats() const {return _n_repeats;}
    QList<bool> getPassList() const {return _passing_stops;}
    QList<bool> getDirectionChanges() const {return _direction_changes;}
    int getRepeatInterval() const {return _repeat_interval;}
    int getIDIncrement() const {return _id_increment;}
    QList<QTime> getCDTTimes() const {return _cdt_times;}
    bool checkService();
    void setType(ROSService::ServiceType type) {_service_type = type;}
    void setFinishState(ROSService::FinishState fin_state);
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
        _cdt_times.push_back(QTime());
    }
    void updateStation(QString station, QList<QTime> time, bool CDT=false, bool Pass=false, QTime cdt_time=QTime(), QString new_station=QString())
    {
        const int index = _stations.indexOf(station);
        _times[index] = time;
        _direction_changes[index] = CDT;
        _passing_stops[index] = Pass;
        _cdt_times[index] = cdt_time;
        _stations[index] = new_station;

    }
    void setStopAsPassPoint(int index, bool state) {_passing_stops[index] = state;}
    void setDirectionChangeAtStop(int index, bool state, QTime time=QTime()) {_direction_changes[index] = state; _cdt_times[index] = time;}
    void setSplitAtStop(int index, QString type, QString id, QTime time=QTime()) {_split_data = QMap<QString, QStringList>({{type, {id, _stations[index], time.toString("HH:mm")}}});}
    void setShuttleRefPosition(QString coordinates[2]);
    QString as_string();
    QStringList summarise();
    QStringList getStations() const
    {
        return _stations;
    }
    QList<QList<QTime>> getTimes() const
    {
        return _times;
    }
    QString getID() const {return _service_id; }

private:
    ServiceType _service_type = ServiceType::Service;
    FinishState _finish_as = FinishState::FinishRemainHere;

public:
      ROSService::ServiceType getType() const {return _service_type;}
      ROSService::FinishState getFinState() const {return _finish_as;}
};

#endif // ROSSERVICE_HXX
