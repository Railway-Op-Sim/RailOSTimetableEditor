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
#include "TOPS.hxx"

QT_BEGIN_NAMESPACE
namespace Ui { class ROSTTBAppWindow; }
QT_END_NAMESPACE

/*! @brief      ROS Timetable Editor Main Application Window
    @details    Main window for viewing, creating and editting services within a ROS timetable
    @version    0.1.0
    @author     Kristian Zarebski
    @date 	    last modified 2020-02-16
    @copyright  GNU Public License v3
*/
class ROSTTBAppWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*! @brief Main ROS Timetable Editor Application window
    */
    ROSTTBAppWindow();

    /*! @brief Destructor for main application window
    */
    ~ROSTTBAppWindow();

    /*! @brief Open File Browser for user file selection
    @return void
    */
    void open_file();

    /*! @brief Verify location ID is a valid form
    @return True if ID candidate is valid
    */
    bool checkLocID(QString id);

    /*! @brief Check that the two location IDs are next to one another
    @return True if ID candidates are neighbours
    */
    bool checkIDsAreNeighbours(QStringList ids);

    /*! @brief Update the displayed output
    @return void
    */
    void update_output(ROSService* current_serv = nullptr);

    /*! @brief Fetch pointer to current timetable parser instance
    @return Pointer to ROSTTBGen instance
    */
    ROSTTBGen* getParser(){return _parser;}

private slots:
    //! Open File Menu Action
    void on_actionOpen_triggered();

    //! New File Menu Action
    void on_actionNew_triggered();

    //! Save File As Menu Action
    void on_actionSave_As_triggered();

    //! Save File Menu Action
    void on_actionSave_triggered();

    //! ROS Application Locate Button Press Action
    void on_pushButtonROSLoc_clicked();

    //! Insert New Service Button Press Action
    void on_pushButtonInsert_clicked();

    //! Delete Current Service Button Press Action
    void on_pushButtonDelete_clicked();

    //! Route Select Button Press Action
    void on_pushButtonRoute_clicked();

    //! Add New Service Location Button Press Action
    void on_pushButtonAddLocation_clicked();

    //! Toggle Service Type Standard Action
    void on_radioButtonStandard_toggled(bool checked);

    //! Toggle Service Type Shuttle Feeder Action
    void on_radioButtonShuttleFeeder_toggled(bool checked);

    //! Toggle Service Type Shuttle From Stop Action
    void on_radioButtonShuttleStop_toggled(bool checked);

    //! Toggle Service Type Feeder Action
    void on_radioButtonFeeder_toggled(bool checked);

    //! Toggle Service Type Service from Other Action
    void on_radioButtonFromOther_toggled(bool checked);

    //! Toggle Cell Clicked in Service List Action
    void on_tableWidgetTimetable_cellClicked(int row, int column);

    //! Confirm Timetable Start Time Button Press Action
    void on_pushButtonTTBTime_clicked();

    //! Edit Timetable Start Time Button Press Action
    void on_pushButtonTTBTimeEdit_clicked();

    //! Toggle Cell Double Clicked in Station List Action
    void on_tableWidgetService_cellDoubleClicked(int row, int column);

    //! Toggle Finish State Remain Here Action
    void on_radioButtonFrh_toggled(bool checked);

    //! Toggle Finish State Form New Action
    void on_radioButtonFns_toggled(bool checked);

    //! Toggle Finish State Join Other Action
    void on_radioButtonFjo_toggled(bool checked);

    //! Toggle Finish State Shuttle Remain Here Action
    void on_radioButtonFrsrh_toggled(bool checked);

    //! Toggle Finish State Shuttle Form New Action
    void on_radioButtonFrsfns_toggled(bool checked);

    //! Toggle Finish State Shuttle Feeder Action
    void on_radioButtonFshf_toggled(bool checked);

    //! Toggle Finish State Exit Railway Action
    void on_radioButtonFer_toggled(bool checked);

    //! Clone Service Button Press Action
    void on_pushButtonClone_clicked();

    //! Clear Current Service Button Press Action
    void on_pushButtonClear_clicked();

    //! Parent ID Text Changed Action
    void on_comboBoxParent_currentTextChanged(const QString &arg1);

    //! Toggle Time Edit Checkbox Action
    void on_checkBoxManualTimeEdit_toggled(bool checked);

    //! Toggle TOPS Service Combo Box Action
    void on_comboBoxTOPS_currentTextChanged(const QString &arg1);

private:
    //! Station Add Dialogue Class member instance
    Station_add* _station_add = new Station_add(nullptr, this);

    //! Clone Service Dialogue Class member instance
    CloneDialog* _clone_srv = new CloneDialog(this);

    //! Location of ROS Timetables
    QDir* _ros_timetables = nullptr;

    //! User Interface Class member instance
    Ui::ROSTTBAppWindow *ui;

    //! Selection model for timetable table widget
    QItemSelectionModel* _tt_model_select;

    //! Current open/active file
    QFileDialog* _current_file = new QFileDialog(this);

    //! String address of current open file
    QString _open_file_str = "";

    //! String address of current open route
    QString _current_route = "";

    //! Timetable Parser Class member instance
    ROSTTBGen* _parser = new ROSTTBGen(this);

    //! Current active/open timetable
    ROSTimetable* _current_timetable = new ROSTimetable;

    //! Array containing Timetable table column widths
    QList<int> _ttb_column_widths = {50, 50, 300, 200, 75, 200, 50};

    //! Array containing Service table column widths
    QList<int> _srv_column_widths = {50, 50, 200, 100};

    //! Current active service under modification
    ROSService* _current_service_selection = nullptr;

    /*! @brief Save the current timetable object to file
    @return void
    */
    void _save_file();

    /*! @brief Reset the application to have parameters set to the defaults
    @return void
    */
    void _reset();

    /*! @brief Check that the Railway Operation Simulator executable has been located
    @return True if application located has been set
    */
    bool _checkROS();

    /*! @brief Clear the service editor form
    @return void
    */
    void _clear();

    /*! @brief Record the current service form values
    @return void
    */
    void _record_current_info();

    /*! @brief Delete selected entries from the service list
    @return void
    */
    void _delete_entries();

    /*! @brief Set the initial file to edit
    @return void
    */
    void _set_initial_open_file();

    /*! @brief Enable/Disable the speed, weight, power property editors in the window
    @return void
    */
    void _enable_integer_info(bool enable);

    /*! @brief Set the values displayed in the service editor
    @return void
    */
    void _set_form_info();

    /*! @brief Fetch all candidate feeder services and forward the information to the Feeder selection box
    @return void
    */
    void _populate_feederboxes();

    /*! @brief Clone the current service
    @return void
    */
    void _clone_current();

};
#endif // ROSTTBAPPWINDOW_HXX
