//-------------------------------------------------------------------------//
//         RailOS Timetable Editor Clone Service Application Window           //
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

#ifndef CLONE_DIALOG_HXX
#define CLONE_DIALOG_HXX

#include <QDialog>
#include <QTableWidget>
#include <QMessageBox>

#include "rosservice.hxx"
#include "rostimetable.hxx"

namespace Ui {
class CloneDialog;
}

/*! @brief      RailOS Timetable Editor Service Clone Window
    @details    Pop-up window for cloning an existing service
    @version    0.2.2-alpha
    @author     Kristian Zarebski
    @date 	    last modified 2021-02-16
    @copyright  GNU Public License v3
*/
class CloneDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CloneDialog(QWidget *parent = nullptr);
    ~CloneDialog();

    /*! @brief Forward the current service information from the main application window to this instance
    @param ttb Pointer to current timetable instance
    @param service Pointer to current selected service
    @param ttb_table Pointer to the service list table
    @return void
    */
    void fwdCurrentService(RailOSTimetable* ttb, RailOSService* service, QTableWidget* ttb_table){_current_ttb = ttb; _ttb_table = ttb_table; _current_srv = service;}

    /*! @brief Forward the list of available service identifiers from the main application
    @param services List of strings of service IDs
    @return void
    */
    void fwdServicesList(QStringList& services){_current_service_list = services;}

    /*! @brief Fetch pointer to the newly created service
    @return Pointer to newly created RailOSService instane
    */
    RailOSService* getNewService() const {return _cloned_srv;}

    /*! @brief Reset dialogue to initial values
    @return void
    */
    void setInitialValues();

private slots:
    //! Accept Current Form Values Action
    void on_buttonBoxClone_accepted();

    //! Reject and Close Dialogue Action
    void on_buttonBoxClone_rejected();

private:
    //! User Interface Class member instance
    Ui::CloneDialog *ui = nullptr;

    //! Pointer to current active timetable for edit
    RailOSTimetable* _current_ttb = nullptr;

    //! Pointer to table displaying service list
    QTableWidget* _ttb_table = nullptr;

    //! Pointer to current service open for edit
    RailOSService* _current_srv = nullptr;

    //! Pointer to the newly created clone service
    RailOSService* _cloned_srv = nullptr;

    //! List of services in current timetable
    QStringList _current_service_list = {};

    /*! @brief Check that service to clone is a top level (no parent) service
    @return True if service has no parent
    */
    bool _check_new();

    /*! @brief Create a clone of the current selection with new time and ID
    @return void
    */
    void _create_clone();

    /*! @brief Attempt to create a new ID based on the previous
    @return ID as a string
    */
    QString _create_new_id(const QString& current_id);
};

#endif // CLONE_DIALOG_HXX
