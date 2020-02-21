//-------------------------------------------------------------------------//
//         ROS Timetable Editor Service Class Definition                   //
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

#ifndef ROSSERVICE_HXX
#define ROSSERVICE_HXX

#include <QString>
#include <QTime>
#include <QMap>
#include <QDebug>

//! Join function for combining up to 8 QStrings using a connector symbol
extern QString join(QString join_symbol, QString a, QString b, QString c="NULL",
                    QString d="NULL", QString e="NULL", QString f="NULL",
                    QString g="NULL", QString h="NULL");

//! Join function for combining multiple QStrings as a QStringList using a connector symbol
extern QString join(QString join_symbol, QStringList list);

//! Join function for combining multiple QStrings as a QStringList using a special character (e.g. null string) as a connector symbol
extern QString join(QChar::SpecialCharacter join_symbol, QStringList list);

/*! @brief      ROS Timetable Editor Service Class
    @details    A class to handle all properties of an ROS service
    @version    0.1.4
    @author     Kristian Zarebski
    @date 	    last modified 2020-02-16
    @copyright  GNU Public License v3
*/
class ROSService
{
private:
    //! Unique integer ID
    int _integer_id = 0;
    //! Whether service starts from location point
    bool _labelled_location_start = true;
    //! Service entry/start time
    QTime _enter_map_time = QTime();
    //! Service exit/end time
    QTime _exit_map_time = QTime();
    //! ROS map location IDs for entrance
    QStringList _enter_ids = {"", ""};
    //! ROS map exit location ID
    QString _exit_id = "";
    //! Times for direction changes
    QList<QTime> _cdt_times = {};
    //! ROS service ID for newly formed service
    QString _daughter_id = "";
    //! ROS service ID for the current service
    QString _service_id = "";
    //! Array containing whether sTrainSet are passed or called at
    QList<bool> _passing_sTrainSet = {};
    //! Array containing whether sTrainSet have direction changes
    QList<bool> _direction_changes = {};
    //! Array containing a single element (where applicable): {rear/front split, {new service ID, time as string}}
    QMap<QString, QStringList> _split_data = QMap<QString, QStringList>();
    //! Array containing a single element (where applicable): {join service ID, {location name, time}}
    QMap<QString, QStringList> _join_data = QMap<QString, QStringList>();
    //! Lists of strings consisting of names of calling/passing points
    QStringList _stations = QStringList();
    //! ROS service ID for service which formed current service (where applicable)
    QString _parent_service = "";
    //! List of two element lists consisting of arrival and departure times
    QList<QList<QTime>> _times = QList<QList<QTime>>();
    //! Maximum Power (kW)
    int _power = -1;
    //! Mass (te)
    int _mass= -1;
    //! Maximum Speed (kph)
    int _max_speed = -1;
    //! Maximum Brake Force (te)
    int _max_brake = -1;
    //! Starting Speed (kph)
    int _start_speed = -1;
    //! Number of service repetitions
    int _n_repeats = 0;
    //! Time interval of service repeats (mins)
    int _repeat_interval = 0;
    //! Increment of ROS service ID with each repeat
    int _id_increment = 1;
    //! Service description
    QString _description = "";

    /*! @brief Create new service definition line
    @return String representing the defining line of the service in ROS
    */
    QString _start_new();

    /*! @brief Summarise all stations in service
    @return A String listing all service sTrainSet
    */
    QString _add_sTrainSet();

    /*! @brief Create service termination string
    @return a string representing service termination in ROS
    */
    QString _finalize();
public:
    //! Service initialisation type
    enum class ServiceType
    {
        Service,                //!< New Standard Service
        ServiceFromSplit,       //!< New Service from a Split Service
        ServiceFromService,     //!< New Service from another Service
        ShuttleFinishService,   //!< New Service which terminates a shuttle
        ShuttleFromStop,        //!< New shuttle service starting from a location
        ShuttleFromFeeder,      //!< New shuttle service formed from another service
    };

    //! Service termination type
    enum class FinishState
    {
        FinishRemainHere,          //!< Finish service and stay at location
        FinishFormNew,             //!< Finish service and form a new service
        FinishJoinOther,           //!< Finish service and join another service
        FinishExit,                //!< Finish service by exiting at location
        FinishShuttleRemainHere,   //!< Finish a shuttle service at location
        FinishShuttleFormNew,      //!< Finish a shuttle service and form a new service
        FinishSingleShuttleFeeder, //!< Finish a feeder service for a shuttle
    };

    /*! @brief Create a new ROS Service object
    @param int_id Unique integer identifier for service
    @param start_time Entry time/service start time
    @param description A short comment describing the service
    @param start_speed Service start speed in kph
    @param max_speed Maximum service speed in kph
    @param mass Train mass in te
    @param max_brake_force Maximum brake force in te
    @param power Maximum power in kW
    @return void
    */
    ROSService(int int_id, QTime start_time, QString id, QString description, int start_speed=-1, int max_speed=-1, int mass=-1, int max_brake_force=-1, int power=-1) :
        _integer_id(int_id), _enter_map_time(start_time), _service_id(id), _power(power), _mass(mass), _max_speed(max_speed), _max_brake(max_brake_force), _start_speed(start_speed), _description(description)  {}

    /*! @brief Set number of service repetitions
    @param n Number of repetitions
    @return void
    */
    void setNRepeats(const int& n) { _n_repeats = n;}

    /*! @brief Get the unique internal service integer id
    @return integer ID
    */
    int getIntegerID() const { return _integer_id;}

    /*! @brief Increment all times corresponding to a service station call by @f$n@f$ seconds
    @param n_secs Number of seconds
    @param station Name of station/location in service
    @return void
    */
    void shiftStationTimes(const int n_secs, QString& station);

    /*! @brief Increment all times for a given service by @f$n@f$ seconds
    @param n_secs
    @return void
    */
    void shiftServiceTimes(const int n_secs);

    /*! @brief Set the increment of service ID between repetitions
    @param n Integer increase
    @return void
    */
    void setIDIncrement(const int& n) {_id_increment = n;}

    /*! @brief Set the repeat time interval
    @param n_mins Number of minutes between intervals
    @return void
    */
    void setRepeatInterval(const int& n_mins) {_repeat_interval = n_mins;}

    /*! @brief Set maximum speed limit of service
    @param speed Maximum speed in kph
    @return void
    */
    void setMaxSpeed(const int& speed){_max_speed = speed;}

    /*! @brief Set maximum braking force
    @param force Maximum braking force in te
    @return void
    */
    void setMaxBrake(const int& force){_max_brake = force;}

    /*! @brief Set whether the service starts from a location
    @param isLoc Whether start point is a location
    @return void
    */
    void setLabelledLocationStart(bool isLoc) {_labelled_location_start = isLoc;}

    /*! @brief Set the initial service speed (should be 0 for starting at location)
    @param speed Initial speed in kph
    @return void
    */
    void setStartSpeed(const int& speed){_start_speed = speed;}

    /*! @brief Set the train mass
    @param mass Mass of service in te
    @return void
    */
    void setMass(const int& mass){_mass = mass;}

    /*! @brief Set the ROS service identifier
    @param id Identifier of length 4
    @return void
    */
    void setID(const QString& id) {_service_id = id;}

    /*! @brief Set the maximum power of the service
    @param power Maximum power in kW
    @return void
    */
    void setPower(const int& power) {_power = power;}

    /*! @brief Set the service split data
    @param fr_rear Whether service is front "fsp" or rear "rsp" split
    @param service_id Newly formed service ROS service ID
    @param station Station where split takes place
    @param time Time at which split occurs
    @return void
    */
    void setSplitData(const QString& fr_rear, const QString& service_id, const QString& station, const QString& time)
    {
        _split_data = {{fr_rear, {service_id, station, time}}};
    }

    /*! @brief set the service join data
    @param joining_srv ROS Service ID of joining service
    @param station Station at which join occurs
    @param time Time at which join occurs as string
    @return void
    */
    void setJoinData(const QString& joining_srv, const QString& station, const QString& time)
    {
        _join_data = {{joining_srv, {station, time}}};
    }

    /*! @brief Fetch the split data
    @return QMap<QString, QStringList> of the form {"fsp"/"rsp", {station, time as string}}
    */
    QMap<QString, QStringList> getSplitData() const {return _split_data;}

    /*! @brief Fetch the join data
    @return QMap<QString, QStringList> of the form {other service id, {station, time as string}}
    */
    QMap<QString, QStringList> getJoinData() const {return _split_data;}

    /*! @brief Set the map ID for the exit location of ther service
    @param id ROS Map ID for exit location
    @return void
    */
    void setExitPoint(const QString& id) {_exit_id = id;}

    /*! @brief Set the map IDs (rear, front) for the entrance location
    @param ids An array of size 2 containing rear element ID and front element ID
    @return void
    */
    void setEntryPoint(const QStringList& ids) {_enter_ids = ids;}

    /*! @brief Set the time for service finish/exit
    @param time Time of service termination/exit
    @return void
    */
    void setExitTime(const QTime& time) {_exit_map_time = time;}

    /*! @brief Set the service description
    @param desc Single line describing the current service
    @return void
    */
    void setDescription(const QString& desc) {_description = desc;}

    /*! @brief Set the service entry time/start time
    @param time Time of service start
    @return void
    */
    void setEntryTime(const QTime& time) {_enter_map_time = time;}

    /*! @brief Fetch the two booleans describing whether the service changes direction or passes at a location
    @param station Name of location/station in service
    @return QList<bool> containing {direction change?, passing point?}
    */
    QList<bool> getCDTPass(const QString& station_name)
    {
        for(int i{0}; i < _stations.size(); ++i)
        {
            if(_stations[i] == station_name)
            {
                return {_direction_changes[i], _passing_sTrainSet[i]};
            }
        }

        return {false, false};
    }

    /*! @brief Fetch the service maximum speed
    @return Integer maximum speed in kph
    */
    int getMaxSpeed() const {return _max_speed;}

    /*! @brief Fetch maximum braking power
    @return Integer maximum brake power in te
    */
    int getMaxBrake() const {return _max_brake;}

    /*! @brief Fetch starting speed
     @return Integer starting speed in kph
    */
    int getStartSpeed() const {return _start_speed;}

    /*! @brief Fetch mass of service
    @return Integer mass in te
    */
    int getMass() const {return _mass;}

    /*! @brief Fetch maximum power of service
    @return Integer maximum power in kW
    */
    int getPower() const {return _power;}

    /*! @brief Fetch entry location
     @return QStringList of size 2 {rear element location, front element location}
    */
    QStringList getStartPoint() const {return _enter_ids;}

    /*! @brief Fetch service start time
    @return QTime of service start
    */
    QTime getStartTime() const {return _enter_map_time;}

    /*! @brief Fetch service exit/finish time
    @return QTime of service termination/exit time
    */
    QTime getExitTime() const {return _exit_map_time;}

    /*! @brief Fetch service description
    @return QString of service single line description
    */
    QString getDescription() const {return _description;}

    /*! @brief Fetch service daughter ID (default is empty string)
    @return QString of the service formed when current service finishes
    */
    QString getDaughter() const {return _daughter_id;}

    /*! @brief Fetch service parent ID (default is empty string)
    @return QString of the service from which the current service was formed
    */
    QString getParent() const {return _parent_service;}

    /*! @brief Fetch the location ID for the service exit (where appropriate)
    @return QString of the exit location ID
    */
    QString getExitID() const {return _exit_id;}

    /*! @brief Fetch the number of service repeats
    @return Integer number of repeats
    */
    int getNRepeats() const {return _n_repeats;}

    /*! @brief Fetch the passing point boolean list
    @return QList<bool> list of whether stations are passing points or not
    */
    QList<bool> getPassList() const {return _passing_sTrainSet;}

    /*! @brief Fetch the change direction boolean list
    @return QList<bool> list of whether stations are points where service changes direction
    */
    QList<bool> getDirectionChanges() const {return _direction_changes;}

    /*! @brief Fetch the repeat interval of this service
    @return Integer number of minutes between service repeats
    */
    int getRepeatInterval() const {return _repeat_interval;}

    /*! @brief Fetch the ID increment between repeats
    @return Integer number by which the service ID increases between repetitions
    */
    int getIDIncrement() const {return _id_increment;}

    /*! @brief Fetch times of direction changes
    @return QList<QTime> of the times (where appropriate) that a service changes direction (matching stations by index)
    */
    QList<QTime> getCDTTimes() const {return _cdt_times;}

    /*! @brief Check service is valid
    @returns True if service passes all requirement checks
    */
    bool checkService();

    /*! @brief Check if service starts from a map location
    @return True if service commences at a valid map location
    */
    bool labelledLocationStart() const {return _labelled_location_start;}

    /*! @brief Set the service type
    @param type A valid ROSService::ServiceType describing the initialisation state
    @return void
    */
    void setType(ROSService::ServiceType type) {_service_type = type;}

    /*! @brief Set the service termination category
    @param fin_state A valid ROSService::FinishState describing the service conclusion
    @return void
    */
    void setFinishState(ROSService::FinishState fin_state);

    /*! @brief Set the ID of the parent of the current service
    @param parent_service ROS service identifier for the parent
    @return void
    */
    void setParent(QString parent_service)
    {
        _parent_service = parent_service;
    }

    /*! @brief Set the ID of the daughter of the current service
     @param daughter_service ROS service identifier for the daughter
     @return void
    */
    void setDaughter(QString daughter_service)
    {
        _daughter_id = daughter_service;
    }

    /*! @brief Add a station to the current service timetable
    @param time A QList<QTime> of size 2 giving the arrival and departure time respectively
    @param station Name of the station to add
    @return void
    */
    void addStation(QList<QTime> time, QString station)
    {
        _times.push_back(time);
        _stations.push_back(station);
        _passing_sTrainSet.push_back(false);
        _direction_changes.push_back(false);
        _cdt_times.push_back(QTime());
    }

    /*! @brief Update the current calling point information
    @param station Name of station to modify in service
    @param time A QList<QTime> of size 2 giving the arrival and departure times respectively
    @param CDT Where the stop is a place where a direction change occurs
    @param Pass If the service should pass through the location without stopping
    @param cdt_time If the service changes direction here, the time the change occurs
    @param new_station Swap current station for another (for correcting mistakes)
    @return void
    */
    void updateStation(QString station, QList<QTime> time, bool CDT=false, bool Pass=false, QTime cdt_time=QTime(), QString new_station=QString())
    {
        const int index = _stations.indexOf(station);
        _times[index] = time;
        _direction_changes[index] = CDT;
        _passing_sTrainSet[index] = Pass;
        _cdt_times[index] = cdt_time;
        _stations[index] = new_station;

    }

    /*! @brief Set a given stop (by index) to be a passing point
    @param index Index of the station to alter
    @param state True if a passing point
    @return void
    */
    void setStopAsPassPoint(int index, bool state) {_passing_sTrainSet[index] = state;}

    /*! @brief Change direction at a stop (by index)
    @param index Index of the station to alter
    @param state True if direction change occurs
    @param time QTime of the direction change
    @return void
    */
    void setDirectionChangeAtStop(int index, bool state, QTime time=QTime()) {_direction_changes[index] = state; _cdt_times[index] = time;}

    /*! @brief Change whether a service split occurs at a location
    @param index Index of location in service
    @param type Split type either "fsp" or "rsp" for front and rear split respectively
    @param id Newly formed service ID
    @param time QTime of when split occurs
    @return void
    */
    void setSplitAtStop(int index, QString type, QString id, QTime time=QTime()) {_split_data = QMap<QString, QStringList>({{type, {id, _stations[index], time.toString("HH:mm")}}});}

    /*! @brief Full length summary of service
    @return String of service information
    */
    QString as_string();

    /*! @brief Brief service summary for easy debug viewing: start time, ID, description & stations
    @return String of summarised information
    */
    QStringList summarise();

    /*! @brief Get list of stations
    @return A QStringList of the calling/passing points in the service
    */
    QStringList getStations() const
    {
        return _stations;
    }

    /*! @brief Returns the time array
    @return QList<QList<QTime>> an array of all the arrival and departure times for each location
    */
    QList<QList<QTime>> getTimes() const
    {
        return _times;
    }

    /*! @brief Fetch service ID
     @return A string of the ROS service identifier
    */
    QString getID() const {return _service_id; }

    /*! @brief Fetch if service starts from stop
     @return True if service commences from a location
    */
    bool hasLocationStart() const {return _labelled_location_start;}

private:
    ServiceType _service_type = ServiceType::Service;
    FinishState _finish_as = FinishState::FinishRemainHere;

public:
      /*! @brief Fetch the initialisation type
      @return ROSService::ServiceType which describes the type of service
      */
      ROSService::ServiceType getType() const {return _service_type;}

      /*! @brief Fetch termination type
      @return ROSService:FinishState describing the service state at termination
      */
      ROSService::FinishState getFinState() const {return _finish_as;}

      /*! @brief Remove a station by index from the service
      @param index Index of station in stations list
      */
      void deleteEntry(const int& index);

      /*! @brief Remove a station by name from the service
       @param station Name of the station to remove
       */
      void deleteEntry(const QString& station);
};

#endif // ROSSERVICE_HXX
