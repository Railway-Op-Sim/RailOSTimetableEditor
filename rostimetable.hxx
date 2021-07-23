//-------------------------------------------------------------------------//
//         ROS Timetable Editor Timetable Class Definition                 //
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

#ifndef ROSTIMETABLE_HXX
#define ROSTIMETABLE_HXX

#include <QMap>
#include <QDateTime>
#include <QSet>
#include <QDebug>

#include "rosservice.hxx"

/*! @brief      ROS Timetable Editor Timetable Class
    @details    A class that acts as a container for multiple services forming the basis for a .ttb file
    @version    0.2.0-alpha
    @author     Kristian Zarebski
    @date 	    last modified 2021-02-16
    @copyright  GNU Public License v3
*/
class ROSTimetable
{
    private:
        //! Timetable start time
        QTime _start_time = QTime::fromString("00:00", "HH:mm");

        //! List of services as map with service ID as keys
        QMap<QString, ROSService*> _services;

        //! List of known locations read from route
        QList<QString> _stored_locations = {};

        //! Name of current open file
        QString _file_name = "";
    public:
        /*! @brief Create a new timetable object
         @param file_name Name of file to edit/new file name
         @return void
        */
        ROSTimetable(QString file_name="NewTimetable_"+QDateTime::currentDateTime().toString("HH_mm_dd-MM-yy")+".ttb") : _file_name(file_name) {}

        /*! @brief Add a new service to the timetable
        @param int_id Unique integer ID for service
        @param start_time Service start time
        @param id ROS style service ID
        @param description Single line service description
        @param start_speed Service starting speed in kph
        @param max_speed Maximum service speed in kph
        @param mass Service mass in te
        @param max_brake_force Maximum braking force in te
        @param power Maximum power in kW
        @return void
        */
        void addService(int int_id, QTime start_time, QString id, QString description,
                        int start_speed=-1, int max_speed=-1, int mass=-1, int max_brake_force=-1, int power=-1)
        {
            _services[id] = new ROSService(int_id, start_time, id, description, start_speed, max_speed, mass, max_brake_force, power);
        }

        /*! @brief Add a new service to the timetable
        @param service An ROSService to add
        @return void
        */
        void addService(ROSService* service)
        {
            _services[service->getID()] = service;
        }

        /*! @brief Remove a service from the timetable
        @param service_ref ROS service ID of service to remove
        @return void
        */
        void removeService(const QString& service_ref)
        {
            _services.erase(_services.find(service_ref));
        }

        /*! @brief Fetch the service list map
        @return A map object containing all services in timetable with entries as {service ID, service pointer}
        */
        QMap<QString, ROSService*> getServices() const {return _services;}

        /*! @brief Fetch the current active file name
        @return The file address of the current file
        */
        QString getFileName() const {return _file_name;}

        /*! @brief Set the name of the current active file
        @param file_name New file name
        @return void
        */
        void setFileName(QString file_name) {_file_name = file_name;}

        /*! @brief Access services from the timetable by ID
        @return Pointer to service
        */
        ROSService* operator[](const QString&);

        /*! @brief Access services from the timetable by index
        @return Pointer to service
        */
        ROSService* operator[](const int&);

        /*! @brief Pointer to first timetable entry
        @return Pointer to service
        */
        ROSService* begin() {return this->operator[](0);}

        /*! @brief Pointer to last timetable entry
        @return Pointer to service
        */
        ROSService* end() {return this->operator[](_services.size()-1);}

        /*! @brief Fetch the service start time
        @return The service start time
        */
        QTime getStartTime() const {return _start_time;}

        /*! @brief Get timetable size
        @return Number of services in timetable
        */
        int size() const {return _services.size();}

        /*! @brief Set the start time
        @return void
        */
        void setStartTime(QTime& time) {_start_time = time;}
};

#endif // ROSTIMETABLE_HXX
