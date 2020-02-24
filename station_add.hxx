//-------------------------------------------------------------------------//
//             ROS Timetable Editor Location Add Window                    //
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

#ifndef STATION_ADD_HXX
#define STATION_ADD_HXX

#include <QDialog>
#include <QSet>
#include <QTime>
#include <QTableWidget>

#include "rosservice.hxx"

namespace Ui {
class Station_add;
}

/*! @brief      ROS Timetable Editor Location Addition Window
    @details    Pop-up window for adding a new location to an existing service
    @version    0.1.6
    @author     Kristian Zarebski
    @date 	    last modified 2020-02-16
    @copyright  GNU Public License v3
*/
class Station_add : public QDialog
{
    Q_OBJECT

public:
    /*! @brief Add Location Dialogue Class
    @param service Current service open for edit
    @param parent Parent application window
    */
    explicit Station_add(ROSService* service=nullptr, QWidget *parent = nullptr);

    //! Destructor for Dialogue
    ~Station_add();

    /*! @brief Set the stations in the choice list
    @return void
    */
    void setStations(QSet<QString> stations);

    /*! @brief Perform checks on whether information is acceptable
    @return True if all checks are passed
    */
    bool setInfo();

    //! Current service start time
    QTime _srv_start = QTime();

    /*! @brief Set the current editable service
    @param service Pointer to service to edit
    @return void
    */
    void setCurrentService(ROSService* service)
    {
        if(!service || service->getStartTime() == QTime()) return;
        _current_srv = service;
        _srv_start = service->getStartTime();
    }

    /*! @brief Fetch current time values
    @return QList<QTime> of arrival and departure times
    */
    QList<QTime> getTimes() const {return _times;}

    /*! @brief Fetch the current station being edited
    @return Station name
    */
    QString getStation() const {return _current_station;}

    /*! @brief Switch between adding new station and editting current
    @return void
    */
    void setEditMode(bool on);

    /*! @brief Reset Dialogue state
    @return void
    */
    void reset_state();

    /*! @brief Set pointer to main application service table
    @return void*/
    void setServiceTable(QTableWidget* serv_table){_service_table = serv_table;}

    /*! @brief Forward to this class information from parent on current selected location
    @param station Current selection name
    @param times Current selection arrival and departure times
    @param isCDT Whether current location includes a service direction change
    @param isPass Whether current location is a passed location
    @return void
    */
    void fwdCurrentSelection(const QString& station, const QList<QTime>& times, bool isCDT, bool isPass);

    /*! @brief Forward time from last timetable event
    @param time Most recent event time from current timetable
    @return void
    */
    void fwdPreviousEventTime(const QTime& time) {_times = {time};}

    /*! @brief Clear the form contents
    @return void
    */
    void clearForm();

private slots:
    //! Station add accepted Action
    void on_buttonBoxAddStation_accepted();

    //! Station add rejected Action
    void on_buttonBoxAddStation_rejected();

    //! Toggle Direction Change Action
    void on_checkBoxCDT_stateChanged();

    //! Toggle Service Split Action
    void on_checkBoxSplit_toggled(bool checked);

    //! Toggle Service Join Action
    void on_checkBoxJoinOther_toggled(bool checked);

    //! Toggle Service Pass Action
    void on_checkBoxPASS_toggled(bool checked);

    //! Delete Current Location Button Press Action
    void on_pushButtonDeleteEntry_clicked();

private:
    //! Current window user interface pointer
    Ui::Station_add *ui;

    //! List of currently added stations
    QList<QString> _station_list = {};

    //! Whether to open dialogue as edit mode
    bool _edit_mode = false;

    //! Pointer to currently editable service
    ROSService* _current_srv;

    //! Pointer to display table for locations
    QTableWidget* _service_table;

    //! Name of currently editable location stop
    QString _current_station = "";

    //! Arrive and Depart times
    QList<QTime> _times = {QTime(), QTime()};

    /*! @brief Redraw the table in the main window
    @return void
    */
    void _redraw_table();

};

#endif // STATION_ADD_HXX
