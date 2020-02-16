//-------------------------------------------------------------------------//
//              ROS Timetable Editor Main Application Window               //
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

#ifndef ROSTTBAPPWINDOW_HXX
#define ROSTTBAPPWINDOW_HXX

#include <QMainWindow>
#include <QWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include "rosttbgen.hxx"
#include "station_add.hxx"
#include "clone_dialog.hxx"

QT_BEGIN_NAMESPACE
namespace Ui { class ROSTTBAppWindow; }
QT_END_NAMESPACE

class ROSTTBAppWindow : public QMainWindow
{
    Q_OBJECT

public:
    ROSTTBAppWindow(QWidget *parent = nullptr);
    ~ROSTTBAppWindow();
    void open_file();
    bool checkLocID(QString id);
    bool checkIDsAreNeighbours(QStringList ids);
    void _update_output(ROSService* current_serv = nullptr);

    ROSTTBGen* getParser(){return _parser;}

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionSave_As_triggered();

    void on_pushButtonROSLoc_clicked();

    void on_pushButtonInsert_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonRoute_clicked();

    void on_pushButtonAddLocation_clicked();

    void on_radioButtonStandard_toggled(bool checked);

    void on_radioButtonShuttleFeeder_toggled(bool checked);

    void on_radioButtonShuttleStop_toggled(bool checked);

    void on_radioButtonFeeder_toggled(bool checked);

    void on_radioButtonFromOther_toggled(bool checked);

    void on_tableWidgetTimetable_cellClicked(int row, int column);

    void on_actionSave_triggered();

    void on_pushButtonTTBTime_clicked();

    void on_pushButtonTTBTimeEdit_clicked();


    void on_tableWidgetService_cellDoubleClicked(int row, int column);

    void on_radioButtonFrh_toggled(bool checked);

    void on_radioButtonFns_toggled(bool checked);

    void on_radioButtonFjo_toggled(bool checked);

    void on_radioButtonFrsrh_toggled(bool checked);

    void on_radioButtonFrsfns_toggled(bool checked);

    void on_radioButtonFshf_toggled(bool checked);

    void on_radioButtonFer_toggled(bool checked);

    void on_pushButtonClone_clicked();

    void on_pushButtonClear_clicked();

    void on_comboBoxParent_currentTextChanged(const QString &arg1);

    void on_checkBoxManualTimeEdit_toggled(bool checked);

private:
    Station_add* _station_add = new Station_add(nullptr, this);
    CloneDialog* _clone_srv = new CloneDialog(this);
    QDir* _ros_timetables = nullptr;
    Ui::ROSTTBAppWindow *ui;
    QItemSelectionModel* _tt_model_select;
    QFileDialog* _current_file = new QFileDialog(this);
    QString _open_file_str = "";
    QString _current_route = "";
    ROSTTBGen* _parser = new ROSTTBGen(this);
    ROSTimetable* _current_timetable = new ROSTimetable;
    QList<int> _ttb_column_widths = {50, 50, 300, 200, 75, 200, 50};
    QList<int> _srv_column_widths = {50, 50, 200, 100};
    ROSService* _current_service_selection = nullptr;
    void save_file();
    void reset();
    bool checkROS();
    void _clear();
    void _record_current_info();
    void delete_entries();
    void _set_initial_open_file();
    void _enable_integer_info(bool enable);
    void _set_tab(bool tab_mode_on);
    void _set_form_info();
    void _populate_feederboxes();
    void _clone_current();

};
#endif // ROSTTBAPPWINDOW_HXX
