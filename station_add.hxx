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

enum class StopType
{
    CallingPoint,
    StartPoint,
    Terminus,
    PassingPoint
};

/*! @brief      ROS Timetable Editor Location Addition Window
    @details    Pop-up window for adding a new location to an existing service
    @version    0.1.0
    @author     Kristian Zarebski
    @date 	    last modified 2020-02-16
    @copyright  GNU Public License v3
*/
class Station_add : public QDialog
{
    Q_OBJECT

public:
    explicit Station_add(ROSService* service=nullptr, QWidget *parent = nullptr);
    ~Station_add();
    void setStations(QSet<QString> stations);
    bool setInfo();
    QTime _srv_start = QTime();
    void setCurrentService(ROSService* service)
    {
        if(!service || service->getStartTime() == QTime()) return;
        _current_srv = service;
        _srv_start = service->getStartTime();
    }
    QList<QTime> getTimes() const {return _times;}
    QString getStation() const {return _current_station;}
    bool setEditMode(bool on);
    void setType(StopType type) {_stop_class = type;}
    StopType getType() const {return _stop_class;}
    void reset_state();
    void setServiceTable(QTableWidget* serv_table){_service_table = serv_table;}
    void fwdCurrentSelection(const QString& station, const QList<QTime>& times, bool isCDT, bool isPass);
    void fwdPreviousEventTime(const QTime& time) {_times = {time};}

private slots:
    void on_buttonBoxAddStation_accepted();

    void on_buttonBoxAddStation_rejected();

    void on_checkBoxCDT_stateChanged();

    void on_checkBoxSplit_toggled(bool checked);

    void on_checkBoxJoinOther_toggled(bool checked);

    void on_checkBoxPASS_toggled(bool checked);

    void on_pushButtonDeleteEntry_clicked();

private:
    Ui::Station_add *ui;
    QList<QString> _station_list = {};
    bool _edit_mode = false;
    ROSService* _current_srv;
    QTableWidget* _service_table;
    QString _current_station = "";
    QList<QTime> _times = {QTime(), QTime()};
    StopType _stop_class = StopType::CallingPoint;
    void _redraw_table();
};

#endif // STATION_ADD_HXX
