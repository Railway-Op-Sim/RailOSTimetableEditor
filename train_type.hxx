//-------------------------------------------------------------------------//
//              ROS Timetable Editor train Train Class                      //
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

#ifndef TRAINTYPE_HXX
#define TRAINTYPE_HXX

#include <QString>

/*! @brief      ROS Timetable Editor train Train Class
    @details    Template train definitions for train
    @version    0.1.7
    @author     Kristian Zarebski
    @date 	    last modified 2020-02-18
    @copyright  GNU Public License v3
*/
class TrainType
{
    private:
        //! Train Classification
        const QString _train_classification = "";

        //! Mass in Short Tons
        const int _mass = -1;           // In short tonnes

        //! Maximum Power in kW
        const int _max_power = -1;      // In kW

        //! Maximum Speed in km/h
        const int _max_speed = -1;      // In kph

        //! Maximum Brake Power in Short Tons
        const int _max_brake = -1;      // In short tonnes

    public:
        //! Empty Constructor
        TrainType() {}

        /*! @brief Constructor for Initialising All Parameters
        @param train_class The train classification of the multiple unit
        @param mass Mass of full multiple unit in Short Tons
        @param power Maximum power in kW
        @param max_speed Maximum speed in km/h
        @param max_brake Maximum braking force in Short Tons
        @return void
        */
        TrainType(const QString& train_class, const int& mass,
                  const int& power, const int& max_speed,
                  const int& max_brake) :
                    _train_classification(train_class), _mass(mass),
                    _max_power(power), _max_speed(max_speed), _max_brake(max_brake) {}

        /*! @brief Fetch Maximum Speed
        @return Maximum speed in km/h
        */
        int getMaxSpeed() const {return _max_speed;}

        /*! @brief Fetch Maximum Power
        @return Maximum power in kW
        */
        int getMaxPower() const {return _max_power;}

        /*! @brief Fetch Maximum Braking Force
        @return Maximum braking force in short tons
        */
        int getMaxBrake() const {return _max_brake;}

        /*! @brief Fetch Mass
        @return Mass in short tons
        */
        int getMass() const {return _mass;}

        /*! @brief Fetch train Class
        @return train Classification label
        */
        QString getClass() const {return _train_classification;}

        /*! @brief Summarise information
        @return A summary of all properties for this class
        */
        QString summarise();
};

#endif // TRAINTYPE_HXX
