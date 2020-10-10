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

#ifndef ROSTTBGEN_HXX
#define ROSTTBGEN_HXX

#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QSet>
#include <QRegularExpression>

#include <map>
#include <vector>

#include "rostimetable.hxx"
#include "rosservice.hxx"

/*! @brief      ROS Timetable Editor Parse Class
    @details    A class to parse existing timetables and generate new ones using current data
    @version    0.1.9
    @author     Kristian Zarebski
    @date 	    last modified 2020-02-16
    @copyright  GNU Public License v3
*/
class ROSTTBGen
{
    private:
        //! Stored text
        QString _cached_text = "";

        //! Container for parse list of stations
        QSet<QString> _stations_list= {};

        //! Current timetable to be modified
        ROSTimetable* _current_timetable = new ROSTimetable;

        //! Container for holding parsed timetable data
        QStringList _input_data = {};

        //! Container for holding other lines
        QStringList _comments = {};

        //! Pointer to the parent application window object
        QWidget* _parent;

        /*! @brief Process the current recorded data
        @return void
        */
        void _process_data();

        /*! @brief Check string is a time
        @return True if string is a valid time
        */
        bool _isTime(QString string);

        /*! @brief Check string is an ROS ID
        @return True if string is valid ID
        */
        bool _isID(QString string);

        /*! @brief Check string is an integer
        @return True if string represents integer
        */
        bool _isInteger(QString string);

        /*! @brief Check string list describes pass statement
        @return True if list is a pass statement
        */
        bool _isPass(QStringList str_list);

        /*! @brief Check string list describes a service start
        @return True if list is a service start statement
        */
        bool _isStart(QStringList str_list);

        /*! @brief Check string list describes a service finish
        @return True if list is a service termination statement
        */
        bool _isEnd(QStringList str_list);

        /*! @brief Check string is a map coordinate
         @return True if string matches coordinate template
        */
        bool _isCoordinate(QString string);

        /*! @brief Check string list is a service definition
        @return True if list is a service definition
        */
        bool _isServiceDefinition(QStringList str_list);

        /*! @brief Check string list is a service continuation
        @return True if list is a continuation (new service from other)
        */
        bool _isContinuedServiceDefinition(QStringList str_list);

        /*! @brief Check string list is a type declaration
        @return True if list matches type definition template
        */
        bool _isStartTypeDefinition(QStringList str_list);

        /*! @brief Check string list is a valid station
        @return True if list matches calling point definition
        */
        bool _isCallingPoint(QStringList str_list);

        /*! @brief Check string list is a start or terminate location
        @return True if the list describes an arrive/depart location
        */
        bool _isStartStopPoint(QStringList str_list);

        /*! @brief Check string list is a direction change
        @return True if string list describes direction change
        */
        bool _isDirChange(QStringList str_list);

        /*! @brief Check string list is a repeat declaration
        @return True if the list matches template for repeats
        */
        bool _isRepeat(QStringList str_list);

        /*! @brief Check string list is a split declaration
        @return True if string list describes split
        */
        bool _isSplit(QStringList str_list);

        /*! @brief Check string list is a join declaration
        @return True if string list describes join
        */
        bool _isJoin(QStringList str_list);

        /*! @brief Process the information for the current service candidate
        @return void
        */
        bool _process_service_candidate(int int_id, QStringList service);

        /*! @brief Fetch service join partner
        @return ROS service ID for partner service
        */
        QString _get_partner(const QString& id);

        /*! @brief Convert string service type to ROSService::ServiceType
        @return Enum service type ROSService::ServiceType
        */
        ROSService::ServiceType _parseType(QStringList str_list);

        /*! @brief Convert string service finish state to ROSService::FinishState
        @return Enum service finish state ROSService::FinishState
        */
        ROSService::FinishState _parseExit(QStringList str_list);

        /*! @brief Construct the service definition string
        @return String for ROS to initialise service
        */
        QString _make_service_definition(ROSService* service);

        /*! @brief Construct service type statement
        @return String for ROS to set service type
        */
        QString _make_type_line(ROSService* service);

        /*! @brief Construct station list strings
        @return String list of all calling points
        */
        QStringList _add_stations(ROSService* service);

        /*! @brief Construct service termination statement
        @return String for ROS to set service termination
        */
        QString _make_service_termination(ROSService* service);

        /*! @brief Construct service repetition statement
        @return String for ROS describing repeat options
        */
        QString _make_repeat_line(ROSService* service);

        //! Map object to map ROSService::FinishState onto the ROS syntax
        QMap<ROSService::FinishState, QString> _exit_types = {{ROSService::FinishState::FinishExit, "Fer"},
                                                              {ROSService::FinishState::FinishFormNew, "Fns"},
                                                              {ROSService::FinishState::FinishJoinOther, "Fjo"},
                                                              {ROSService::FinishState::FinishRemainHere, "Frh"},
                                                              {ROSService::FinishState::FinishShuttleRemainHere, "Frh-sh"},
                                                              {ROSService::FinishState::FinishShuttleFormNew, "Fns-sh"},
                                                              {ROSService::FinishState::FinishSingleShuttleFeeder, "F-nshs"}};

        //! Map object to map ROSService::ServiceType onto the ROS syntax
        QMap<ROSService::ServiceType, QString> _start_types = {{ROSService::ServiceType::Service, "Snt"},
                                                               {ROSService::ServiceType::ShuttleFromStop, "Snt-sh"},
                                                               {ROSService::ServiceType::ShuttleFromFeeder, "Sns-sh"},
                                                               {ROSService::ServiceType::ServiceFromSplit, "Sfs"},
                                                               {ROSService::ServiceType::ShuttleFinishService, "Sns-fsh"},
                                                               {ROSService::ServiceType::ServiceFromService, "Sns"}};
    public:
        /*! @brief Construct a parser instance
        @param parent Pointer to parent application window
        */
        ROSTTBGen(QWidget* parent) : _parent(parent) {}

        /*! @brief Check string is a time (public)
        @return True if string is a valid time
        */
        bool checkStringIsTime(QString string) {return _isTime(string);}

        /*! @brief Get list of parsed stations
        @return A set of unique location labels
        */
        QSet<QString> getStations(){return _stations_list;}

        /*! @brief Parse the data from an input ROS .ttb file
        @return void
        */
        QString parse_file(const QFileDialog* dialog, const QDir* directory);

        /*! @brief Parse the station list from an input ROS .rly file
        @return void
        */
        QString parse_rly(const QString railways_dir);

        /*! @brief Fetch the constructed timetable
        @return Pointer to newly created timetable
        */
        ROSTimetable* getParsedTimetable() { return _current_timetable; }

        /*! @brief Retrieve ROS syntax for given finish state
        @return String to represent the finish state
        */
        QString getFinishID(ROSService::FinishState state) {return _exit_types[state];}

        /*! @brief Retrieve ROS syntax for given service type
        @return String to represent the service type
        */
        QString getStartID(ROSService::ServiceType state) {return _start_types[state];}

        /*! @brief Construct string representation
        @return List of strings representing lines for output .ttb file
        */
        QStringList createTimetableStrings(ROSTimetable* timetable);

        /*! @brief Set the Station List (used to maintain station listing when parser is reset/remade
        @return void
        */
        void setStations(QSet<QString> stations) {_stations_list = stations;}
};

#endif // ROSTTBGEN_HXX
