//-------------------------------------------------------------------------//
//         ROS Timetable Editor Timetable Class Definition                 //
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

#ifndef ROSTIMETABLE_HXX
#define ROSTIMETABLE_HXX

#include <QMap>
#include <QDateTime>
#include <QSet>
#include <QDebug>

#include "rosservice.hxx"

/*! @brief      ROS Timetable Editor Timetable Class
    @details    A class that acts as a container for multiple services forming the basis for a .ttb file
    @version    0.1.0
    @author     Kristian Zarebski
    @date 	    last modified 2020-02-16
    @copyright  GNU Public License v3
*/
class ROSTimetable
{
    private:
        QTime _start_time = QTime::fromString("00:00", "HH:mm");
        QMap<QString, ROSService*> _services;
        QList<QString> _stored_locations = {};
        QString _file_name = "";
    public:
        ROSTimetable(QString file_name="NewTimetable_"+QDateTime::currentDateTime().toString("HH_mm_dd-MM-yy")+".ttb") : _file_name(file_name) {}
        void addService(int int_id, QTime start_time, QString id, QString description,
                        int start_speed=-1, int max_speed=-1, int mass=-1, int max_brake_force=-1, int power=-1)
        {
            _services[id] = new ROSService(int_id, start_time, id, description, start_speed, max_speed, mass, max_brake_force, power);
        }
        void addService(ROSService* service)
        {
            _services[service->getID()] = service;
        }
        void removeService(const QString& service_ref)
        {
            _services.erase(_services.find(service_ref));
        }
        QMap<QString, ROSService*> getServices() const {return _services;}
        QString getFileName() const {return _file_name;}
        void setFileName(QString file_name) {_file_name = file_name;}
        ROSService* operator[](const QString&);
        ROSService* operator[](const int&);
        ROSService* begin() {return this->operator[](0);}
        ROSService* end() {return this->operator[](_services.size()-1);}
        QTime getStartTime() const {return _start_time;}
        int size() const {return _services.size();}
        void setStartTime(QTime& time) {_start_time = time;}
};

#endif // ROSTIMETABLE_HXX
