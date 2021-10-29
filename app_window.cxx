//-------------------------------------------------------------------------//
//              ROS Timetable Editor Main Application Window               //
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

#include "app_window.hxx"
#include "ui_rosttbappwindow.h"

ROSTTBAppWindow::ROSTTBAppWindow()
    : QMainWindow()
    , ui(new Ui::ROSTTBAppWindow)
{
    _about_window->setVersion(VERSION);

    const int SERV_COL_COUNT = 4, TTB_COL_COUNT = 3;
    ui->setupUi(this);
    _tt_model_select = new QItemSelectionModel(ui->tableWidgetTimetable->model());
    ui->ROSStatus->setStyleSheet("QLabel { color : red; }");
    ui->SelectedRoute->setStyleSheet("QLabel { color : red; }");
    ui->checkBoxManualTimeEdit->setEnabled(false);
    QHeaderView* _header_ttb = ui->tableWidgetTimetable->horizontalHeader();
    _header_ttb->setStretchLastSection(true);
    QHeaderView* _header_srv = ui->tableWidgetService->horizontalHeader();
    _header_srv->setStretchLastSection(true);
    ui->tableWidgetTimetable->setColumnCount(TTB_COL_COUNT);
    ui->tableWidgetService->setColumnCount(SERV_COL_COUNT);
    ui->pushButtonTTBTime->setVisible(false);
    ui->pushButtonTTBTimeEdit->setVisible(true);
    ui->timeEditTTBStart->setVisible(false);
    ui->lineEditTemplateName->setVisible(false);
    ui->pushButtonTemplateSave->setVisible(false);
    ui->pushButtonTemplateCancel->setVisible(false);
    ui->labelTemplateName->setVisible(false);
    ui->labelTrackIDFront->setVisible(false);
    ui->labelTrackIDRear->setVisible(false);
    ui->labelStartTime->setText(_current_timetable->getStartTime().toString("HH:mm"));
    for(int i{0}; i < TTB_COL_COUNT; ++i) ui->tableWidgetTimetable->setColumnWidth(i, _ttb_column_widths[i]);
    for(int i{0}; i < SERV_COL_COUNT; ++i) ui->tableWidgetService->setColumnWidth(i, _srv_column_widths[i]);
    ui->tableWidgetTimetable->verticalHeader()->setVisible(false);
    ui->tableWidgetTimetable->horizontalHeader()->setVisible(false);
    ui->tableWidgetTimetable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetTimetable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetTimetable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetService->verticalHeader()->setVisible(false);
    ui->tableWidgetService->horizontalHeader()->setVisible(false);
    ui->checkBoxAtStation->setChecked(true);
    ui->tableWidgetService->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetService->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetService->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->radioButtonFromOther->setEnabled(false);
    ui->radioButtonShuttleFeeder->setEnabled(false);
    ui->radioButtonShuttleFinish->setEnabled(false);
    ui->radioButtonFrh->setChecked(true);
    ui->comboBoxTrainSet->setCurrentIndex(0);
    ui->servicerefEdit->setMaxLength(4);
    ui->serviceFinishServiceEdit->setMaxLength(4);
    ui->textEditShuttlePart2->setMaxLength(4);
    ui->textEditParentShuttleRef->setMaxLength(4);
    _station_add->setServiceTable(ui->tableWidgetService);
    ui->checkBoxAtStation->setChecked(true);
    ui->comboBoxTrackIDs->setEnabled(false);
    ui->comboBoxTrackIDStations->setEnabled(false);
    QFont font = qApp->font();
    font.setPixelSize(12);
    qApp->setFont(font);

    _cache_dir = new QDir(join(_qt_path_sep, QStandardPaths::writableLocation(QStandardPaths::HomeLocation), "Documents", "ROSTTBEditor", "cache"));

    _read_in_custom_templates();

    if(!_cache_dir->exists())
    {
        qDebug() << "Creating Cache Folder at: " << _cache_dir->absolutePath();
        _cache_dir->mkpath(".");
    }
    QFile _cache_file(join("/", _cache_dir->absolutePath(), "ros_location_cache.dat"));

    if(_cache_file.exists())
    {
        qDebug() << "Reading ROS Location from cache: " << _cache_file.fileName();
        if(!_cache_file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(0, QObject::tr("error"), _cache_file.errorString());
        }
        QTextStream in(&_cache_file);

        QString _ros_loc;
        while (!in.atEnd()) {
            _ros_loc = in.readLine();
        }
        ui->ROSStatus->setText(_ros_loc);
        ui->ROSStatus->setStyleSheet("QLabel { color : green; }");

        _make_paths(_ros_loc);
    }

    _cache_file.close();

    ui->radioButtonStandard->setEnabled(true);
    ui->radioButtonStandard->toggle();
    ui->radioButton_kph->toggle();

}

ROSTTBAppWindow::~ROSTTBAppWindow()
{
    delete ui;
}

bool ROSTTBAppWindow::checkLocID(QString id)
{
    const QStringList _parts = id.split("-");
    if(_parts.size() != 2) return false;

    for(auto part : _parts)
    {
        for(int i{1}; i < part.size(); ++i)
        {
            if(part[i].isLetter()) return false;
        }
    }

    return true;
}

bool ROSTTBAppWindow::checkIDsAreNeighbours(QStringList ids)
{
    QString _coordinate_0_0 = ids[0].split("-")[0],
            _coordinate_0_1 = ids[0].split("-")[1],
            _coordinate_1_0 = ids[1].split("-")[0],
            _coordinate_1_1 = ids[1].split("-")[1];

    int _temp = 0, _temp2 = 0;

    if(_coordinate_0_0[0].isLetter() != _coordinate_1_0[0].isLetter()) return false;
    if(_coordinate_0_0[0].isLetter() && _coordinate_0_0[0] != _coordinate_1_0[0]) return false;
    if(_coordinate_0_1[0].isLetter() != _coordinate_1_1[0].isLetter()) return false;
    if(_coordinate_0_1[0].isLetter() && _coordinate_0_1[0] != _coordinate_1_1[0]) return false;

    if(_coordinate_0_0[0].isLetter())
    {

            _temp = _coordinate_0_0.right(_coordinate_0_0.size()-2).toInt();
            _temp2 = _coordinate_1_0.right(_coordinate_1_0.size()-2).toInt();

    }
    else
    {
        _temp = _coordinate_0_0.toInt();
        _temp2 = _coordinate_1_0.toInt();
     }

    if(abs(_temp2-_temp) != 0 && abs(_temp2-_temp) != 1)
    {
        return false;
    }

    if(_coordinate_0_1[0].isLetter())
    {
            _temp = _coordinate_0_1.right(_coordinate_0_0.size()-2).toInt();
            _temp2 = _coordinate_1_1.right(_coordinate_1_0.size()-2).toInt();
    }
    else
    {
        _temp = _coordinate_0_1.toInt();
        _temp2 = _coordinate_1_1.toInt();
     }

    if(abs(_temp2-_temp) != 0 && abs(_temp2-_temp) != 1)
    {
        return false;
    }

       return true;
}

bool ROSTTBAppWindow::_record_current_info()
{
    ui->labelParentOfJoin->setText("");
    if(!_checkROS()) return false;
    QString _srv_id = ui->servicerefEdit->text();
    const QString _desc = ui->descEdit->text();
    const QTime _start_time = ui->starttimeEdit->time();
    const QString _shuttle_partner = ui->textEditShuttlePart2->text();
    if(_current_timetable->getStartTime().msecsTo(_start_time) < 0)
    {
        QMessageBox::critical(this, QObject::tr("Invalid Service"), QObject::tr("Service start time must not precede timetable start time."));
        return false;
    }
    const int _max_power   = ui->spinBoxPower->value(),
              _start_speed = ui->spinBoxStartSpeed->value(),
              _mass        = ui->spinBoxMass->value(),
              _max_brake   = ui->spinBoxForce->value(),
              _max_speed   = ui->spinBoxMaxSpeed->value();

    QStringList _start_ids;

    if(!ui->checkBoxAtStation->isChecked())
    {
        _start_ids.push_back(ui->textEditEnterID1->text());
        _start_ids.push_back(ui->textEditEnterID2->text());
    }
    else
    {
        const int _index = ui->comboBoxTrackIDs->findText(ui->comboBoxTrackIDs->currentText());
        const QStringList _coord_strs = _current_coords[_index].toList();
        _start_ids.push_back(_coord_strs[0]);
        _start_ids.push_back(_coord_strs[1]);
    }

    const bool info_missing = _srv_id == "" || _desc == "";
    if(info_missing)
    {
        QMessageBox::critical(this, QObject::tr("Missing Information"), QObject::tr("Service Identifier or Description Missing"));
        return false;
    }

    if(ui->radioButtonStandard->isChecked() || ui->radioButtonShuttleStop->isChecked())
    {
        if(_max_speed == 0)
        {
            QMessageBox::critical(this, QObject::tr("Invalid Maximum Speed"), QObject::tr("Maximum service speed cannot be 0 kph for Standard Service or Shuttle from Stop."));
            return false;
        }
        if(_max_power == 0)
        {
            QMessageBox::critical(this, QObject::tr("Invalid Maximum Power"), QObject::tr("Maximum service power cannot be 0 kW for Standard Service or Shuttle from Stop."));
            return false;
        }
        if(_mass == 0)
        {
            QMessageBox::critical(this, QObject::tr("Invalid Mass"), QObject::tr("Mass value cannot be 0 te for Standard Service or Shuttle from Stop."));
            return false;
        }
        if(_max_brake == 0)
        {
            QMessageBox::critical(this, QObject::tr("Invalid Maximum Braking Force"), QObject::tr("Maximum brake force cannot be 0 te for Standard Service or Shuttle from Stop."));
            return false;
        }
        if(_start_ids[0] == "" || _start_ids[1] == "")
        {
            QMessageBox::critical(this, QObject::tr("Invalid Start Position"), QObject::tr("IDs for start location rear and front track elements must be provided for a Standard Service or Shuttle from Stop."));
            return false;
        }

        for(const auto& id : _start_ids)
        {
            if(!checkLocID(id))
            {
                QMessageBox::critical(this, QObject::tr("Invalid Start Position"), QObject::tr("Location IDs must have the form 'A-B' where each of A and B can be either a number or a letter followed by a number."));
                return false;
            }
        }

        if(!checkIDsAreNeighbours(_start_ids))
        {
            QMessageBox::critical(this, QObject::tr("Invalid Start Position"), QObject::tr("Location IDs must be neighbouring map elements."));
            return false;
        }
    }

    if(ui->checkBoxManualTimeEdit->isChecked())
    {
        if(ui->timeEditTermination->time().secsTo(_current_service_selection->getStartTime()) > 0)
        {
            QMessageBox::critical(this, QObject::tr("Invalid Exit Time"), QObject::tr("Exit time cannot be before start time"));
            return false;
        }
    }

    if(!_current_timetable->getServices().contains(_srv_id))
    {
        _current_timetable->addService(_current_timetable->size(), _start_time, _srv_id, _desc, _mph_to_kph(_start_speed), _mph_to_kph(_max_speed), _mass, _max_brake, _max_power);
        _current_service_selection = _current_timetable->end();
    }

    else
    {
        _current_service_selection->setEntryTime(_start_time);
        _current_service_selection->setDescription(_desc);
        _current_service_selection->setStartSpeed(_mph_to_kph(_start_speed));
        _current_service_selection->setMaxSpeed(_mph_to_kph(_max_speed));
        _current_service_selection->setMaxBrake(_max_brake);
        _current_service_selection->setPower(_max_power);
    }

    const int n_repeats = ui->spinBoxRepeats->value(), nmins_interval = ui->spinBoxRepeatInterval->value(), id_inc = ui->spinBoxRefIncrement->value();
    _current_service_selection->setNRepeats(n_repeats);
    _current_service_selection->setRepeatInterval(nmins_interval);
    _current_service_selection->setIDIncrement(id_inc);
    _current_service_selection->setLabelledLocationStart(ui->checkBoxAtStation->isChecked());

    if(ui->checkBoxManualTimeEdit->isChecked())
    {
        _current_service_selection->setExitTime(ui->timeEditTermination->time());
    }

    // -------------------------------------- Service Start Type Selection ------------------------------------------------ //

    if(ui->radioButtonShuttleFinish->isChecked())
    {
        const ROSService* _parent = _current_timetable->getServices()[ui->comboBoxFeeder->currentText()];
        QTime _parent_last_time = _parent->getTimes()[_parent->getTimes().size()-1][0];

        const QString _service_id = _current_timetable->getServices()[_current_service_selection->getParent()]->getDaughter();
        if(_service_id == "")
        {
            QMessageBox::critical(this, QObject::tr("No Parent Daughter Found"), QObject::tr("Could not find reference to this new service within definition of parent service. Failed to retrieve ID for current service."));
            return false;
        }
        if(ui->starttimeEdit->time().msecsTo(_parent_last_time) < 0)
        {
            QMessageBox::critical(this, QObject::tr("Invalid Start Time"), QObject::tr("Daughter service start time cannot be before parent arrival time."));
            return false;
        }

        _current_service_selection->setType(ROSService::ServiceType::ShuttleFinishService);
        _current_service_selection->setEntryTime(_parent_last_time);
        _current_service_selection->setParent(ui->comboBoxFeeder->currentText());
        _current_service_selection->setID(_service_id);
        ui->servicerefEdit->setText(_service_id);
    }
    else if(ui->radioButtonShuttleStop->isChecked())
    {
        if(_shuttle_partner.toStdString().empty())
        {
            QMessageBox::critical(this, QObject::tr("No Partner"), QObject::tr("You must specify an accompanying return service ID."));
            return false;
        }
        _current_service_selection->setDaughter(_shuttle_partner);
        _current_service_selection->setType(ROSService::ServiceType::ShuttleFromStop);
        _current_service_selection->setEntryPoint(_start_ids);

    }
    else if(ui->radioButtonShuttleFeeder->isChecked())
    {
        if(_shuttle_partner.toStdString().empty())
        {
            QMessageBox::critical(this, QObject::tr("No Partner"), QObject::tr("You must specify an accompanying return service ID."));
            return false;
        }
        _current_service_selection->setDaughter(_shuttle_partner);
        _current_service_selection->setType(ROSService::ServiceType::ShuttleFromFeeder);
        _current_service_selection->setParent(ui->comboBoxFeeder->currentText());
    }
    else if(ui->radioButtonFromOther->isChecked())
    {
        const ROSService* _parent = _current_timetable->getServices()[ui->comboBoxParent->currentText()];
        QTime _parent_last_time = _parent->getTimes()[_parent->getTimes().size()-1][0];

        if(ui->starttimeEdit->time().msecsTo(_parent_last_time) < 0)
        {
            QMessageBox::critical(this, QObject::tr("Invalid Start Time"), QObject::tr("Daughter service start time cannot be before parent end time."));
            return false;
        }
        const QString _service_id = _parent->getDaughter();
        if(_service_id.toStdString().empty())
        {
            QMessageBox::critical(this, QObject::tr("No Parent Daughter Found"), QObject::tr("Could not find reference to this new service within definition of parent service. Failed to retrieve ID for current service."));
            return false;
        }
        if(ui->starttimeEdit->time().msecsTo(_parent_last_time) < 0)
        {
            QMessageBox::critical(this, QObject::tr("Invalid Start Time"), QObject::tr("Daughter service start time cannot be before parent arrival time."));
            return false;
        }

        if(_current_timetable->getServices()[ui->comboBoxParent->currentText()]->getSplitData() != QMap<QString, QStringList>())
        {
            _current_service_selection->setType(ROSService::ServiceType::ServiceFromSplit);
        }

        else
        {
            _current_service_selection->setType(ROSService::ServiceType::ServiceFromService);
        }


        _current_service_selection->setID(_service_id);
        _current_service_selection->setEntryTime(_parent_last_time);
        _current_service_selection->setParent(ui->comboBoxParent->currentText());
        _current_service_selection->addStation({_parent->getTimes()[_parent->getTimes().size()-1][0], _parent->getTimes()[_parent->getTimes().size()-1][0]}, _parent->getStations()[_parent->getStations().size()-1]);
        ui->servicerefEdit->setText(_service_id);

    }
    else
    {
        _current_service_selection->setType(ROSService::ServiceType::Service);
        _current_service_selection->setEntryPoint(_start_ids);
    }

    // -------------------------------------------------------------------------------------------------------------------- //

    // -------------------------------------- Service Finish Type Selection ----------------------------------------------- //

    if(ui->radioButtonFer->isChecked())
    {
        QStringList _exit_ids = ui->serviceExitEdit->text().split(",");

        // Strip any white space
        for(int i{0}; i < _exit_ids.size(); ++i)
        {
            _exit_ids[i] = _exit_ids[i].replace(" ", "");
        }

        if(_exit_ids.empty())
        {
            QMessageBox::critical(this, QObject::tr("No Exit Location"), QObject::tr("You must provide an exit location IDs."));
            return false;
        }

        for(const auto& id : _exit_ids)
        {
            if(!checkLocID(id))
            {
                QMessageBox::critical(this, QObject::tr("Invalid Start Position"), QObject::tr("Exit location IDs must have the form 'A-B' where each of A and B can be either a number or a letter followed by a number."));
                return false;
            }
        }

        _current_service_selection->setExitPoints(_exit_ids);
        _current_service_selection->setFinishState(ROSService::FinishState::FinishExit);
    }

    else if(ui->radioButtonFjo->isChecked())
    {
        QString _found_candidate = "NULL";
        for(auto service : _current_timetable->getServices())
        {
            if(service->getSplitData().keys().contains(_current_service_selection->getID()))
            {
                _found_candidate = service->getID();
                break;
            }
        }

        if(_found_candidate == QString("NULL"))
        {
            QMessageBox::critical(this, QObject::tr("No Parent Found"), QObject::tr("Could not find split reference in existing service matching current service. You must define a forward or rear split "\
                                  "in an existing service first."));
            return false;
        }

        ui->labelParentOfJoin->setText(_found_candidate);
        _current_service_selection->setParent(_found_candidate);
    }

    else if(ui->radioButtonFns->isChecked() || ui->radioButtonFshf->isChecked() || ui->radioButtonFrsfns->isChecked())
    {
        QString _srv_id = ui->serviceFinishServiceEdit->text();

        if(_srv_id.toStdString().empty())
        {
            QMessageBox::critical(this, QObject::tr("Missing Daughter Service"), QObject::tr("You must provide an identifier for the newly formed service."));
            return false;
        }

        _current_service_selection->setDaughter(_srv_id);
        if(ui->radioButtonFns->isChecked()) _current_service_selection->setFinishState(ROSService::FinishState::FinishFormNew);
        else if(ui->radioButtonFshf->isChecked()) _current_service_selection->setFinishState(ROSService::FinishState::FinishShuttleFormNew);
        else _current_service_selection->setFinishState(ROSService::FinishState::FinishSingleShuttleFeeder);
    }

    else if(ui->radioButtonFrh->isChecked()) _current_service_selection->setFinishState(ROSService::FinishState::FinishRemainHere);
    else if(ui->radioButtonFrsrh->isChecked()) _current_service_selection->setFinishState(ROSService::FinishState::FinishShuttleRemainHere);
    else _current_service_selection->setFinishState(ROSService::FinishState::FinishShuttleRemainHere);

    // -------------------------------------------------------------------------------------------------------------------- //

    update_output();

    return true;
}

void ROSTTBAppWindow::update_output(ROSService* current_serv)
{
    // Do not allow daughter service creation without at least one timetable entry already present
    if(_current_timetable->size() > 0)
    {
        ui->radioButtonFromOther->setEnabled(true);
        ui->radioButtonShuttleFeeder->setEnabled(true);
        ui->radioButtonShuttleFinish->setEnabled(true);
    }
    ui->tableWidgetService->clear();
    ui->tableWidgetTimetable->clear();
    ui->tableWidgetTimetable->setRowCount(0);
    ui->tableWidgetService->setRowCount(0);
    // IMPORTANT: If Timetable is Empty there is nothing to show!
    if(_current_timetable->size() < 1) return;
    if(current_serv) _current_service_selection = current_serv;
    ui->labelStartTime->setText(_current_timetable->getStartTime().toString("HH:mm"));

    for(int i{0}; i < _current_timetable->size(); ++i)
    {
        QStringList out = _current_timetable->operator[](i)->summarise();
        ui->tableWidgetTimetable->insertRow(ui->tableWidgetTimetable->rowCount());
        for(int j{0}; j < out.size(); ++j)
        {
            QTableWidgetItem* _new_service_item = new QTableWidgetItem(out[j], 0);
            _new_service_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            ui->tableWidgetTimetable->setItem(ui->tableWidgetTimetable->rowCount()-1, j, _new_service_item);
        }
    }

    QStringList _current_element_stations = _current_service_selection->getStations();
    QList<QList<QTime>> _current_element_times = _current_service_selection->getTimes();
    QMap<QString, QStringList> _join_data = _current_service_selection->getJoinData();
    QMap<QString, QStringList> _split_data = _current_service_selection->getSplitData();
    QString _join_service = (_join_data.keys().size() > 0) ? _join_data.keys()[0] : "";
    QString _split_type = (_split_data.keys().size() > 0) ? _split_data.keys()[0] : "";

    bool _join_shown = false, _split_shown = false;

    for(int i{0}; i < _current_element_stations.size(); ++i)
    {
        QTime _arrival_time = _current_element_times[i][0],
              _depart_time  = _current_element_times[i][1];
        QString _ns_string = "";

        if(_current_service_selection->getParent() != "" && i == 0)
        {
            if(_current_service_selection->getParent() == _current_service_selection->getDaughter())
            {
                _ns_string =  "←\t"+_current_service_selection->getParent() + " (Shuttle)";
            }
            else
            {
                _ns_string = "←\t"+_current_service_selection->getParent();
            }
        }

        if(_current_service_selection->getCDTPass(_current_element_stations[i])[0])
        {
            _ns_string += " ⤸ "+_current_service_selection->getCDTTimes()[i].toString("HH:mm");
        }

        if(_current_service_selection->getDaughter() != "" && i == _current_element_stations.size()-1)
        {
            _ns_string += " → "+_current_service_selection->getDaughter();
        }


        else if(_current_service_selection->getCDTPass(_current_element_stations[i])[1])
        {
            _ns_string = "Pass";
        }

        QStringList _station_item = {_arrival_time.toString("HH:mm"), _depart_time.toString("HH:mm"), _current_element_stations[i], _ns_string};

        ui->tableWidgetService->insertRow(ui->tableWidgetService->rowCount());
        for(int j{0}; j < _station_item.size(); ++j)
        {
            QTableWidgetItem* _new_time_item = new QTableWidgetItem(_station_item[j], 0);
            _new_time_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            ui->tableWidgetService->setItem(ui->tableWidgetService->rowCount()-1, j, _new_time_item);
        }

        if(_join_service != "" && _join_data[_join_service][0] == _current_element_stations[i] && !_join_shown)
        {
           QStringList _join_item =  QStringList({_join_data[_join_service][1],
                                                     "",
                                                     "⤹", _join_service});
           ui->tableWidgetService->insertRow(ui->tableWidgetService->rowCount());
           for(int j{0}; j < _join_item.size(); ++j)
           {
               QTableWidgetItem* _new_time_item = new QTableWidgetItem(_join_item[j], 0);
               _new_time_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
               ui->tableWidgetService->setItem(ui->tableWidgetService->rowCount()-1, j, _new_time_item);
           }

           _join_shown = true;
        }

        if(_split_type != "" && _split_data[_split_type][1] == _current_element_stations[i] && !_split_shown)
        {
            QStringList _split_item =  QStringList({_split_data[_split_type][2],
                                                      "",
                                                      "", (_split_type == "rsp") ? _split_data[_split_type][0]+"↤"+_current_service_selection->getID() :
                                                            _current_service_selection->getID()+"↦"+_split_data[_split_type][0]});
            ui->tableWidgetService->insertRow(ui->tableWidgetService->rowCount());
            for(int j{0}; j < _split_item.size(); ++j)
            {
                QTableWidgetItem* _new_time_item = new QTableWidgetItem(_split_item[j], 0);
                _new_time_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                ui->tableWidgetService->setItem(ui->tableWidgetService->rowCount()-1, j, _new_time_item);
            }

            _split_shown = true;
        }

    }

    //ui->tableWidgetService->sortItems(0);

    _station_add->setServiceTable(ui->tableWidgetService);
    _populate_feederboxes();

}

void ROSTTBAppWindow::_enable_integer_info(bool enable)
{
    ui->spinBoxMass->setEnabled(enable);
    ui->spinBoxForce->setEnabled(enable);
    ui->spinBoxMaxSpeed->setEnabled(enable);
    ui->spinBoxStartSpeed->setEnabled(enable);
    ui->spinBoxPower->setEnabled(enable);
}

void ROSTTBAppWindow::on_actionOpen_triggered()
{
    open_file();
}

void ROSTTBAppWindow::on_actionNew_triggered()
{
    _reset();
}

void ROSTTBAppWindow::on_actionSave_As_triggered()
{
    _open_file_str = _current_file->getSaveFileName(this, "Save As", _ros_timetables->dirName(), tr("Timetable (*.ttb)") );
    _save_file();
}

void ROSTTBAppWindow::open_file()
{
    ui->checkBoxAtStation->setChecked(false);
    if(!_ros_timetables)
    {
        QMessageBox::critical(this, QObject::tr("Invalid Application Address"), QObject::tr("You need to first set the location of the Railway Operation Simulator executable."));
        return;
    }
    _reset();
    QString _parsed = _parser->parse_file(_current_file, _ros_timetables);
    if(_parsed == QString()) return; // User pressed Cancel
    _open_file_str = (_parsed != "NULL")? _parsed : _open_file_str;
    _current_timetable = _parser->getParsedTimetable();
    if(_current_timetable->getServices().size() > 0)
    {
        ui->radioButtonFromOther->setEnabled(true);
        ui->radioButtonShuttleFeeder->setEnabled(true);
    }
    _current_service_selection = _current_timetable->end();
    update_output();
    _set_form_info();
}

void ROSTTBAppWindow::_make_paths(QString ros_path)
{
    QStringList _dot_split = ros_path.split(".");
    QString _type = _dot_split[_dot_split.size()-1];
    QStringList _locations = ros_path.split(_qt_path_sep);

    if(_type == "app")
    {
      _locations.append("Contents");
      _locations.append("Resources");
      _locations.append("wineprefix");
      _locations.append("drive_c");
      _locations.append("winebottler");
    }
    else
    {
        _locations.pop_back();
    }
    QString _ttb_dir = (QSysInfo::productType() == "windows") ? "Program timetables" : "Program\\ timetables";
    QString _rly_dir = "Railways";

    QStringList _rly_parts(_locations);
    QStringList _ttb_parts(_locations);

    _ttb_parts.append(_ttb_dir);
    _rly_parts.append(_rly_dir);

    _ros_timetables = new QDir(_ttb_parts.join(_qt_path_sep));
    _ros_railways = new QDir(_rly_parts.join(_qt_path_sep));
}

void ROSTTBAppWindow::on_pushButtonROSLoc_clicked()
{
    QString _file_name = _current_file->getOpenFileName(this, tr("Open File"), QDir::currentPath(),
                                                        tr("ROS Executable (*.exe *.app)"));
    if(_file_name == QString()) return;

    ui->ROSStatus->setText(_file_name);
    ui->ROSStatus->setStyleSheet("QLabel { color : green; }");

    if(!_cache_dir->exists())
    {
        qDebug() << "Creating Cache Folder at: " << _cache_dir->absolutePath();
        _cache_dir->mkpath(".");
    }

    QFile _cache_file(join(_qt_path_sep, _cache_dir->absolutePath(), "ros_location_cache.dat"));

    qDebug() << "Writing cache to : " << join(_qt_path_sep, _cache_dir->absolutePath(), "ros_location_cache.dat") << endl;
    if ( _cache_file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &_cache_file );
        stream << _file_name << endl;
    }
    _cache_file.close();
    _make_paths(_file_name);
}

void ROSTTBAppWindow::_reset()
{
    const QSet<QString> _temp = _parser->getStations();
    delete _current_file;
    delete _parser;
    _parser = new ROSTTBGen(this);
    _parser->setStations(_temp);
    ui->tableWidgetService->setRowCount(0);
    ui->tableWidgetTimetable->setRowCount(0);
    ui->tableWidgetService->clear();
    ui->tableWidgetTimetable->clear();
    _current_file = new QFileDialog(this);
    _current_timetable = new ROSTimetable;
    _station_add = new Station_add(nullptr, this);
}

bool ROSTTBAppWindow::_checkROS()
{
    if(!_ros_timetables)
    {
        QMessageBox::critical(this, QObject::tr("Invalid Application Address"), QObject::tr("You need to first set the location of the Railway Operation Simulator executable."));
        return false;
    }

    return true;
}

void ROSTTBAppWindow::_clear()
{
    ui->servicerefEdit->clear();
    ui->comboBoxTrainSet->clear();
    ui->spinBoxMU->setValue(1);
    ui->spinBoxMaxSpeed->setValue(0);
    ui->spinBoxForce->setValue(0);
    ui->spinBoxRepeats->setValue(0);
    ui->spinBoxStartSpeed->setValue(0);
    ui->spinBoxRefIncrement->setValue(0);
    ui->spinBoxRepeatInterval->setValue(0);
    ui->textEditEnterID1->setText("");
    ui->textEditEnterID2->setText("");
    ui->checkBoxAtStation->setChecked(true);
    ui->serviceFinishServiceEdit->clear();
    ui->textEditParentShuttleRef->clear();
    ui->descEdit->clear();
    ui->spinBoxMass->setValue(0);
    ui->spinBoxPower->setValue(0);
}

void ROSTTBAppWindow::on_pushButtonInsert_clicked()
{
    if(_record_current_info())
    {
        _station_add->setCurrentService(_current_service_selection);
        _clear();
    }
}

void ROSTTBAppWindow::_delete_entries()
{
    if(!_current_service_selection)
    {
        qDebug() << "No current selection registered, delete ignored.";
    }
    QModelIndexList _entries = _tt_model_select->selectedRows();
    for(auto i : _entries)
    {
        qDebug() << "Removing row: " << i.row() <<" from Timetable Table.";
        ui->tableWidgetTimetable->removeRow(i.row());
    }

    qDebug() << "Removing service '" << _current_service_selection->getID() << "' from timetable.";
    const int _before = _current_timetable->size();
    _current_timetable->removeService(_current_service_selection->getID());
    qDebug() << "Timetable Size Changed from " << _before << " to " << _current_timetable->size();
    _current_service_selection = (_current_timetable->size() == 0) ? nullptr : _current_timetable->end();
    update_output();
}

void ROSTTBAppWindow::on_pushButtonDelete_clicked()
{
    _delete_entries();
}

void ROSTTBAppWindow::_populate_feederboxes()
{
    const QMap<QString, ROSService*> _services = _current_timetable->getServices();
    if(_services.size() < 1) return;

    QStringList _service_ids = {};

    for(auto key : _services.keys()) _service_ids.push_back(QString(key));

    ui->comboBoxFeeder->clear();
    ui->comboBoxParent->clear();

    std::sort(_service_ids.begin(), _service_ids.end());

    ui->comboBoxFeeder->addItems(_service_ids);
    ui->comboBoxParent->addItems(_service_ids);
}

void ROSTTBAppWindow::on_pushButtonRoute_clicked()
{
    QString _parsed = _parser->parse_rly(_ros_railways->path());
    _current_route = (_parsed != "NULL") ? _parsed : _current_route;
    QStringList _parts = _current_route.split(_qt_path_sep);
    ui->SelectedRoute->setText(_parts[_parts.size()-1]);
    ui->SelectedRoute->setStyleSheet("QLabel { color : green; }");
    QStringList _stations = _parser->getStations().toList();
    _stations.sort();
    ui->comboBoxTrackIDStations->addItems(_stations);
    ui->comboBoxTrackIDStations->setEnabled(true);
    _populate_coordinates(ui->comboBoxTrackIDStations->currentText());
}

void ROSTTBAppWindow::on_pushButtonAddLocation_clicked()
{
    if(_current_timetable->size() == 0)
    {
        QMessageBox::critical(this, QObject::tr("No Service Defined"), QObject::tr("You must add a service before selecting calling points."));
        return;
    }    
    if(_parser->getStations().size() == 0)
    {
      QMessageBox::critical(this, QObject::tr("No Route"), QObject::tr("You must open the relevant route before adjusting an existing timetable."));
      ui->pushButtonRoute->click();
      update_output();
      return;
    }
    _station_add->setCurrentService(_current_service_selection);
    _station_add->setStations(_parser->getStations());
    _station_add->clearForm();
    _station_add->setVisible(true);
    QList<QTime> _times;
    if(_current_service_selection->getTimes().size() > 0)
    {
        _times = _current_service_selection->getTimes()[_current_service_selection->getTimes().size()-1];
    }
    else
    {
        _times = QList<QTime>({QTime(), QTime(), QTime()});
    }

    _station_add->fwdPreviousEventTimes(_times);

    update_output();
}

void ROSTTBAppWindow::on_radioButtonStandard_toggled(bool checked)
{
    _enable_integer_info(true);
    if(checked)
    {
        ui->serviceFinishServiceEdit->clear();
        ui->spinBoxMU->setEnabled(true);
        ui->comboBoxTrainSet->setEnabled(true);
        ui->servicerefEdit->setEnabled(true);
        ui->starttimeEdit->setEnabled(true);
        ui->checkBoxAtStation->setEnabled(true);
        ui->textEditShuttlePart2->clear();
        ui->textEditShuttlePart2->setEnabled(false);
        ui->spinBoxMass->setEnabled(true);
        ui->spinBoxForce->setEnabled(true);
        ui->spinBoxPower->setEnabled(true);
        ui->spinBoxMaxSpeed->setEnabled(true);
        ui->spinBoxStartSpeed->setEnabled(true);
        ui->spinBoxRefIncrement->setEnabled(true);
        ui->spinBoxRepeatInterval->setEnabled(true);
        ui->spinBoxRepeats->setEnabled(true);
        ui->textEditEnterID1->setEnabled(true);
        ui->textEditEnterID2->setEnabled(true);
        ui->comboBoxFeeder->clear();
        ui->comboBoxFeeder->setEnabled(false);
        ui->comboBoxParent->clear();
        ui->comboBoxParent->setEnabled(false);
        ui->textEditParentShuttleRef->clear();
        ui->textEditParentShuttleRef->setEnabled(false);
    }
}

void ROSTTBAppWindow::on_radioButtonShuttleFeeder_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
        ui->serviceFinishServiceEdit->clear();
        ui->spinBoxMU->setEnabled(false);
        ui->comboBoxTrainSet->setEnabled(false);
        ui->servicerefEdit->setEnabled(false);
        ui->starttimeEdit->setEnabled(false);
        ui->checkBoxAtStation->setEnabled(false);
        ui->textEditShuttlePart2->setEnabled(true);
        ui->servicerefEdit->clear();
        ui->servicerefEdit->setEnabled(false);
        ui->spinBoxMass->clear();
        ui->spinBoxMass->setEnabled(false);
        ui->spinBoxForce->clear();
        ui->spinBoxForce->setEnabled(false);
        ui->spinBoxPower->clear();
        ui->spinBoxPower->setEnabled(false);
        ui->spinBoxMaxSpeed->clear();
        ui->spinBoxMaxSpeed->setEnabled(false);
        ui->spinBoxStartSpeed->clear();
        ui->spinBoxStartSpeed->setEnabled(false);
        ui->spinBoxRefIncrement->setEnabled(true);
        ui->spinBoxRepeatInterval->setEnabled(true);
        ui->spinBoxRepeats->setEnabled(true);
        ui->textEditEnterID1->clear();
        ui->textEditEnterID1->setEnabled(false);
        ui->textEditEnterID2->clear();
        ui->textEditEnterID2->setEnabled(false);
        ui->comboBoxFeeder->setEnabled(true);
        ui->comboBoxParent->clear();
        ui->comboBoxParent->setEnabled(false);
        ui->textEditParentShuttleRef->clear();
        ui->textEditParentShuttleRef->setEnabled(false);
    }
}

void ROSTTBAppWindow::on_radioButtonShuttleStop_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
        ui->serviceFinishServiceEdit->clear();
        ui->spinBoxMU->setEnabled(true);
        ui->comboBoxTrainSet->setEnabled(true);
        ui->servicerefEdit->setEnabled(true);
        ui->starttimeEdit->setEnabled(true);
        ui->checkBoxAtStation->setEnabled(true);
        ui->textEditShuttlePart2->setEnabled(true);
        ui->servicerefEdit->setEnabled(true);
        ui->spinBoxMass->setEnabled(true);
        ui->spinBoxForce->setEnabled(true);
        ui->spinBoxPower->setEnabled(true);
        ui->spinBoxMaxSpeed->setEnabled(true);
        ui->spinBoxStartSpeed->setEnabled(true);
        ui->spinBoxRefIncrement->setEnabled(true);
        ui->spinBoxRepeatInterval->setEnabled(true);
        ui->spinBoxRepeats->setEnabled(true);
        ui->textEditEnterID1->setEnabled(true);
        ui->textEditEnterID2->setEnabled(true);
        ui->comboBoxFeeder->clear();
        ui->comboBoxFeeder->setEnabled(false);
        ui->comboBoxParent->clear();
        ui->comboBoxParent->setEnabled(false);
        ui->textEditParentShuttleRef->clear();
        ui->textEditParentShuttleRef->setEnabled(false);
    }
}

void ROSTTBAppWindow::on_radioButtonFromOther_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
        ui->serviceFinishServiceEdit->clear();
        ui->spinBoxMU->setEnabled(false);
        ui->comboBoxTrainSet->setEnabled(false);
        ui->starttimeEdit->setEnabled(false);
        ui->checkBoxAtStation->setEnabled(false);
        ui->textEditShuttlePart2->clear();
        ui->textEditShuttlePart2->setEnabled(false);
        ui->servicerefEdit->setEnabled(false);
        ui->spinBoxMass->clear();
        ui->spinBoxMass->setEnabled(false);
        ui->spinBoxForce->clear();
        ui->spinBoxForce->setEnabled(false);
        ui->spinBoxPower->clear();
        ui->spinBoxPower->setEnabled(false);
        ui->spinBoxMaxSpeed->clear();
        ui->spinBoxMaxSpeed->setEnabled(false);
        ui->spinBoxStartSpeed->clear();
        ui->spinBoxStartSpeed->setEnabled(false);
        ui->spinBoxRefIncrement->setEnabled(true);
        ui->spinBoxRepeatInterval->setEnabled(true);
        ui->spinBoxRepeats->setEnabled(true);
        ui->textEditEnterID1->clear();
        ui->textEditEnterID1->setEnabled(false);
        ui->textEditEnterID2->clear();
        ui->textEditEnterID2->setEnabled(false);
        ui->comboBoxFeeder->clear();
        ui->comboBoxFeeder->setEnabled(false);
        ui->comboBoxParent->setEnabled(true);
        ui->textEditParentShuttleRef->clear();
        ui->textEditParentShuttleRef->setEnabled(false);

        QString _current_parent = ui->comboBoxParent->currentText();
        ui->servicerefEdit->setText(_current_timetable->getServices()[_current_parent]->getDaughter());
        ui->starttimeEdit->setTime(_current_timetable->getServices()[_current_parent]->getExitTime());
    }
}

void ROSTTBAppWindow::on_radioButtonShuttleFinish_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
        ui->serviceFinishServiceEdit->clear();
        ui->spinBoxMU->setEnabled(false);
        ui->comboBoxTrainSet->setEnabled(false);
        ui->servicerefEdit->setEnabled(false);
        ui->starttimeEdit->setEnabled(false);
        ui->checkBoxAtStation->setEnabled(false);
        ui->textEditShuttlePart2->clear();
        ui->textEditShuttlePart2->setEnabled(false);
        ui->servicerefEdit->setEnabled(true);
        ui->spinBoxMass->clear();
        ui->spinBoxMass->setEnabled(false);
        ui->spinBoxForce->clear();
        ui->spinBoxForce->setEnabled(false);
        ui->spinBoxPower->clear();
        ui->spinBoxPower->setEnabled(false);
        ui->spinBoxMaxSpeed->clear();
        ui->spinBoxMaxSpeed->setEnabled(false);
        ui->spinBoxStartSpeed->clear();
        ui->spinBoxStartSpeed->setEnabled(false);
        ui->spinBoxRefIncrement->setEnabled(true);
        ui->spinBoxRepeatInterval->setEnabled(true);
        ui->spinBoxRepeats->setEnabled(true);
        ui->textEditEnterID1->clear();
        ui->textEditEnterID1->setEnabled(false);
        ui->textEditEnterID2->clear();
        ui->textEditEnterID2->setEnabled(false);
        ui->comboBoxFeeder->clear();
        ui->comboBoxFeeder->setEnabled(false);
        ui->comboBoxParent->setEnabled(true);
        ui->textEditParentShuttleRef->clear();
        ui->textEditParentShuttleRef->setEnabled(true);
    }
}

int ROSTTBAppWindow::_mph_to_kph(const int speed)
{
    return (ui->radioButton_kph->isChecked()) ? speed : int(1.609344*speed);
}

int ROSTTBAppWindow::_kph_to_mph(const int speed)
{
    return (ui->radioButton_kph->isChecked()) ? speed : int(speed/1.609344);
}

void ROSTTBAppWindow::_set_form_info()
{

    const QString _serv_id = _current_service_selection->getID(),
          _description = _current_service_selection->getDescription(),
          _feeder_ref  = _current_service_selection->getParent(),
          _daughter_ref = _current_service_selection->getDaughter();

    const QStringList _exit_locs = _current_service_selection->getExitIDs();

    const ROSService::ServiceType _type = _current_service_selection->getType();
    const ROSService::FinishState _exit_as = _current_service_selection->getFinState();

    const int _max_power   = _current_service_selection->getPower(),
              _start_speed = _current_service_selection->getStartSpeed(),
              _mass        = _current_service_selection->getMass(),
              _max_brake   = _current_service_selection->getMaxBrake(),
              _max_speed   = _current_service_selection->getMaxSpeed(),
              _n_repeats   = _current_service_selection->getNRepeats(),
              _interval    = _current_service_selection->getIDIncrement(),
              _t_interv    = _current_service_selection->getRepeatInterval();
    const QStringList _start_ids = _current_service_selection->getStartPoint();
    const QTime _start_time = _current_service_selection->getStartTime(),
                _exit_time = _current_service_selection->getExitTime();

    ui->spinBoxMass->setValue(_mass);
    ui->spinBoxForce->setValue(_max_brake);
    ui->spinBoxPower->setValue(_max_power);
    ui->spinBoxMaxSpeed->setValue(_max_speed);
    ui->spinBoxStartSpeed->setValue(_start_speed);

    // Need to be careful, check array is actually filled before filling form
    if(!ui->checkBoxAtStation->isChecked())
    {
        if(_start_ids.size() > 0)ui->textEditEnterID1->setText(_start_ids[0]);
        else{ui->textEditEnterID1->clear();}
        if(_start_ids.size() > 1)ui->textEditEnterID2->setText(_start_ids[1]);
        else{ui->textEditEnterID2->clear();}
    }

    ui->spinBoxRepeats->setValue(_n_repeats);
    ui->spinBoxRefIncrement->setValue(_interval);
    ui->spinBoxRepeatInterval->setValue(_t_interv);

    ui->descEdit->setText(_description);
    ui->comboBoxFeeder->setCurrentText(_feeder_ref);
    ui->comboBoxParent->setCurrentText(_feeder_ref);
    ui->textEditParentShuttleRef->setText(_feeder_ref);
    ui->servicerefEdit->setText(_serv_id);
    ui->starttimeEdit->setTime(_start_time);
    ui->timeEditTermination->setTime(_exit_time);

    ui->checkBoxAtStation->setChecked(_current_service_selection->labelledLocationStart());

    switch(_type)
    {
        case ROSService::ServiceType::Service:
            ui->radioButtonStandard->setChecked(true);
            break;
        case ROSService::ServiceType::ShuttleFromStop:
            ui->textEditShuttlePart2->setText(_daughter_ref);
            ui->radioButtonShuttleStop->setChecked(false);
            break;
        case ROSService::ServiceType::ServiceFromSplit:
            ui->radioButtonFromOther->setChecked(true);
            break;
        case ROSService::ServiceType::ServiceFromService:
            ui->radioButtonFromOther->setChecked(true);
            break;
        case ROSService::ServiceType::ShuttleFinishService:
            ui->radioButtonShuttleFinish->setChecked(true);
            break;
        case ROSService::ServiceType::ShuttleFromFeeder:
            ui->textEditShuttlePart2->setText(_daughter_ref);
            ui->radioButtonShuttleFeeder->setChecked(true);
            break;
        default:
            break;
    }

    switch(_exit_as)
    {
        case ROSService::FinishState::FinishRemainHere:
            ui->checkBoxManualTimeEdit->setChecked(false);
            ui->radioButtonFrh->setChecked(true);
            break;
        case ROSService::FinishState::FinishExit:
            ui->checkBoxManualTimeEdit->setChecked(true);
            ui->serviceExitEdit->setText(join(",", _exit_locs));
            ui->radioButtonFer->setChecked(true);
            break;
        case ROSService::FinishState::FinishFormNew:
            ui->checkBoxManualTimeEdit->setChecked(false);
            ui->serviceFinishServiceEdit->setText(_daughter_ref);
            ui->radioButtonFns->setChecked(true);
            break;
        case ROSService::FinishState::FinishJoinOther:
            ui->checkBoxManualTimeEdit->setChecked(false);
            ui->labelParentOfJoin->setText(_feeder_ref);
            ui->radioButtonFjo->setChecked(true);
            break;
        case ROSService::FinishState::FinishShuttleFormNew:
            ui->checkBoxManualTimeEdit->setChecked(false);
            ui->serviceFinishServiceEdit->setText(_daughter_ref);
            ui->radioButtonFrsfns->setChecked(true);
            break;
        case ROSService::FinishState::FinishShuttleRemainHere:
            ui->checkBoxManualTimeEdit->setChecked(false);
            ui->radioButtonFrsrh->setChecked(true);
            break;
        case ROSService::FinishState::FinishSingleShuttleFeeder:
            ui->checkBoxManualTimeEdit->setChecked(false);
            ui->radioButtonFshf->setChecked(true);
            break;
        default:
            break;
    }

}

void ROSTTBAppWindow::on_tableWidgetTimetable_cellClicked(int row, int column)
{
    if(ui->tableWidgetTimetable->rowCount() < 1) return;
    ROSService* _selection = _current_timetable->operator[](ui->tableWidgetTimetable->takeItem(row, 1)->text());
    qDebug() << "Selected: " << _selection->summarise();
    update_output(_selection);
    ui->tableWidgetTimetable->selectRow(row);
    _set_form_info();
}

void ROSTTBAppWindow::_save_file()
{
    QStringList _ttb = _parser->createTimetableStrings(_current_timetable);
    if(_ttb.size() < 2)
    {
        QMessageBox::critical(this, "Failed Timetable Conversion", "Failed to Write Timetable to File, No Services Detected.");
        return;
    }
    QString _output = join(QChar::Null, _ttb);

    qDebug() << "Writing timetable output to: " <<  _open_file_str;

    QFile _file(_open_file_str);
    if ( _file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream out(&_file);
        out << _output;
    }

    _file.close();
}

void ROSTTBAppWindow::on_actionSave_triggered()
{
    _save_file();
}

void ROSTTBAppWindow::on_pushButtonTTBTime_clicked()
{
    QTime _temp_time = ui->timeEditTTBStart->time();
    ui->starttimeEdit->setTime(_temp_time);
    for(auto service : _current_timetable->getServices())
    {
        if(service->getStartTime().msecsTo(_temp_time) < 0)
        {
            QMessageBox::critical(this, QObject::tr("Invalid Start Time"), QObject::tr("Start time for timetable cannot be after first service time."));
            return;
        }
    }
    _current_timetable->setStartTime(_temp_time);
    ui->pushButtonTTBTime->setVisible(false);
    ui->pushButtonTTBTimeEdit->setVisible(true);
    ui->timeEditTTBStart->setVisible(false);
    ui->labelStartTime->setText(_temp_time.toString("HH:mm"));
    ui->labelStartTime->setVisible(true);
}

void ROSTTBAppWindow::on_pushButtonTTBTimeEdit_clicked()
{
    ui->pushButtonTTBTime->setVisible(true);
    ui->pushButtonTTBTimeEdit->setVisible(false);
    ui->timeEditTTBStart->setVisible(true);
    ui->labelStartTime->setVisible(false);
}

void ROSTTBAppWindow::on_tableWidgetService_cellDoubleClicked(int row, int column)
{
    if(_parser->getStations().size() == 0)
    {
      QMessageBox::critical(this, QObject::tr("No Route"), QObject::tr("You must open the relevant route before adjusting an existing timetable."));
      ui->pushButtonRoute->click();
      update_output();
      return;
    }
    _station_add->setEditMode(true);
    _station_add->setCurrentService(_current_service_selection);
    _station_add->setStations(_parser->getStations());
    const QTableWidgetItem* _table_cell = ui->tableWidgetService->takeItem(row, 2);
    if(!_table_cell)
    {
        QMessageBox::warning(this, QObject::tr("Empty cell."), QObject::tr("Failed to retrieve station info for entry."));
        return;
    }

    const QString _station_name = _table_cell->text();

    if(!_parser->getStations().contains(_station_name))
    {
        QMessageBox::critical(this, QObject::tr("No Route"), QObject::tr("You must open the relevant route before adjusting an existing timetable (station not found in current route)."));
        ui->pushButtonRoute->click();
        update_output();
        return;
    }

    const QList<bool> _cdtPass = _current_service_selection->getCDTPass(_station_name);
    const QString _arrive = ui->tableWidgetService->takeItem(row, 0)->text(),
                _depart = ui->tableWidgetService->takeItem(row, 1)->text();

    const QTableWidgetItem* _info_cell = ui->tableWidgetService->takeItem(row, 3);

    if(_info_cell)
    {
        QStringList _info = _info_cell->text().split(" ");
        QTime _cdt_time;
        for(int i{0}; i < _info.size(); ++i)
        {
            if(_info[i].contains("⤸"))
            {
                _cdt_time = (_parser->checkStringIsTime(_info[i+1])) ? QTime::fromString(_info[i+1], "HH:mm") : QTime();
                break;
            }
        }
        if(!_station_add->fwdCurrentSelection(_station_name, {QTime::fromString(_arrive, "HH:mm"), QTime::fromString(_depart, "HH:mm"), _cdt_time}, _cdtPass[0], _cdtPass[1]))
        {
            QMessageBox::critical(this, "Invalid Time Array", "Times array should be of size 3, {arrival, depart, cdt}");
            return;
        }
    }

    _station_add->setVisible(true);
    update_output();
}

void ROSTTBAppWindow::on_radioButtonFrh_toggled(bool checked)
{
    if(checked)
    {
        ui->checkBoxManualTimeEdit->setEnabled(false);
        ui->serviceExitEdit->clear();
        ui->serviceFinishServiceEdit->clear();
        ui->serviceExitEdit->setEnabled(false);
        ui->serviceFinishServiceEdit->setEnabled(false);
    }
}

void ROSTTBAppWindow::on_radioButtonFns_toggled(bool checked)
{
    if(checked)
    {
        ui->checkBoxManualTimeEdit->setEnabled(false);
        ui->serviceExitEdit->clear();
        ui->serviceExitEdit->setEnabled(false);
        ui->serviceFinishServiceEdit->setEnabled(true);
    }
}

void ROSTTBAppWindow::on_radioButtonFjo_toggled(bool checked)
{
    if(checked)
    {
        ui->checkBoxManualTimeEdit->setEnabled(false);
        ui->serviceExitEdit->clear();
        ui->serviceFinishServiceEdit->clear();
        ui->serviceExitEdit->setEnabled(false);
        ui->serviceFinishServiceEdit->setEnabled(false);
    }
}

void ROSTTBAppWindow::on_radioButtonFrsrh_toggled(bool checked)
{
    if(checked)
    {
        ui->checkBoxManualTimeEdit->setEnabled(false);
        ui->serviceExitEdit->clear();
        ui->serviceFinishServiceEdit->clear();
        ui->serviceExitEdit->setEnabled(false);
        ui->serviceFinishServiceEdit->setEnabled(false);
    }
}

void ROSTTBAppWindow::on_radioButtonFrsfns_toggled(bool checked)
{
    if(checked)
    {
        ui->checkBoxManualTimeEdit->setEnabled(false);
        ui->serviceExitEdit->clear();
        ui->serviceExitEdit->setEnabled(false);
        ui->serviceFinishServiceEdit->setEnabled(true);
    }
}

void ROSTTBAppWindow::on_radioButtonFshf_toggled(bool checked)
{
    if(checked)
    {
        ui->checkBoxManualTimeEdit->setEnabled(false);
        ui->serviceExitEdit->clear();
        ui->serviceExitEdit->setEnabled(false);
        ui->serviceFinishServiceEdit->setEnabled(true);
    }
}

void ROSTTBAppWindow::on_radioButtonFer_toggled(bool checked)
{
    if(checked)
    {
        ui->checkBoxManualTimeEdit->setEnabled(true);
        ui->serviceFinishServiceEdit->clear();
        ui->serviceExitEdit->setEnabled(true);
        ui->serviceFinishServiceEdit->setEnabled(false);
    }
}

void ROSTTBAppWindow::_clone_current()
{
    if(!_current_service_selection) return;
    _clone_srv->fwdCurrentService(_current_timetable, _current_service_selection, ui->tableWidgetTimetable);
    QStringList _service_ids = {};
    for(auto s : _current_timetable->getServices()) _service_ids.push_back(s->getID());
    _clone_srv->fwdServicesList(_service_ids);
    _clone_srv->setInitialValues();
    _clone_srv->setVisible(true);
}

void ROSTTBAppWindow::on_pushButtonClone_clicked()
{
    _clone_current();
    if(!_clone_srv->getNewService()) return;
    update_output();
}

void ROSTTBAppWindow::on_pushButtonClear_clicked()
{
    _clear();
}

void ROSTTBAppWindow::on_comboBoxParent_currentTextChanged(const QString &arg1)
{
    if(_current_timetable->size() < 1 || !_current_timetable->getServices().contains(arg1) || _current_service_selection->getID() == arg1) return;
    const ROSService* _parent = _current_timetable->getServices()[arg1];
    if(!_parent)
    {
        qDebug() << "Failed to Retrieve Parent from Services";
    }

    QTime _parent_last_time;

    if(_parent->getExitTime() != QTime())
    {
        _parent_last_time = _parent->getExitTime();
    }
    else if(_parent->getTimes().size() > 0)
    {
        _parent_last_time = _parent->getTimes()[_parent->getTimes().size()-1][0];
    }
    else{
        return;
    }

    ui->starttimeEdit->setTime(_parent_last_time);
    ui->servicerefEdit->setText(_parent->getDaughter());
}

void ROSTTBAppWindow::on_checkBoxManualTimeEdit_toggled(bool checked)
{
    ui->timeEditTermination->setEnabled(checked);
}

void ROSTTBAppWindow::on_comboBoxTrainSet_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Custom" || arg1 == "") return;
    const int factor = ui->spinBoxMU->value();
    if(TrainSet::TrainSet.keys().contains(arg1))
    {
        const int _max_speed = TrainSet::TrainSet[arg1]->getMaxSpeed();
        const int _max_power = TrainSet::TrainSet[arg1]->getMaxPower();
        const int _max_brake = TrainSet::TrainSet[arg1]->getMaxBrake()*factor;
        const int _mass = TrainSet::TrainSet[arg1]->getMass()*factor;

        _is_template_change = true;
        ui->spinBoxMass->setValue(_mass);
        _is_template_change = true;
        ui->spinBoxForce->setValue(_max_brake);
        _is_template_change = true;
        ui->spinBoxPower->setValue(_max_power);
        _is_template_change = true;
        ui->spinBoxMaxSpeed->setValue(_max_speed);
    }

    else
    {
        const int _max_speed = _custom_types[arg1]->getMaxSpeed();
        const int _max_power = _custom_types[arg1]->getMaxPower();
        const int _max_brake = _custom_types[arg1]->getMaxBrake()*factor;
        const int _mass = _custom_types[arg1]->getMass()*factor;

        _is_template_change = true;
        ui->spinBoxMass->setValue(_mass);
        _is_template_change = true;
        ui->spinBoxForce->setValue(_max_brake);
        _is_template_change = true;
        ui->spinBoxPower->setValue(_max_power);
        _is_template_change = true;
        ui->spinBoxMaxSpeed->setValue(_max_speed);
    }

}

void ROSTTBAppWindow::on_spinBoxMU_valueChanged(int arg1)
{
    on_comboBoxTrainSet_currentTextChanged(ui->comboBoxTrainSet->currentText());
}

void ROSTTBAppWindow::on_spinBoxMaxSpeed_valueChanged(int arg1)
{
    if(!_is_template_change) ui->comboBoxTrainSet->setCurrentIndex(0);
    _is_template_change = false;
}

void ROSTTBAppWindow::on_spinBoxMass_valueChanged(int arg1)
{
    if(!_is_template_change) ui->comboBoxTrainSet->setCurrentIndex(0);
    _is_template_change = false;
}

void ROSTTBAppWindow::on_spinBoxForce_valueChanged(int arg1)
{
    if(!_is_template_change) ui->comboBoxTrainSet->setCurrentIndex(0);
    _is_template_change = false;
}

void ROSTTBAppWindow::on_spinBoxPower_valueChanged(int arg1)
{
    if(!_is_template_change) ui->comboBoxTrainSet->setCurrentIndex(0);
    _is_template_change = false;
}

void ROSTTBAppWindow::_read_in_custom_templates()
{

    _custom_types = QMap<QString, TrainType*>();

    QFile _cache_file(join(_qt_path_sep, _cache_dir->absolutePath(), "trainset_templates_cache.dat"));

    if(_cache_file.exists())
    {

        if(!_cache_file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(0,QObject::tr("error"), _cache_file.errorString());
        }
        QTextStream in(&_cache_file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList _elements = line.split(";");

            const QString _name = _elements[0].split(":")[1];
            const int _max_speed = _elements[1].split(":")[1].toInt(),
                      _mass      = _elements[2].split(":")[1].toInt(),
                      _force     = _elements[3].split(":")[1].toInt(),
                      _power     = _elements[4].split(":")[1].toInt();

            _custom_types[_name] = new TrainType(_name, _mass, _power, _max_speed, _force);

            break;
        }

        _cache_file.close();
    }
    ui->comboBoxTrainSet->clear();
    QStringList _temp = TrainSet::TrainSet.keys()+_custom_types.keys();
    QSet<QString> _temp_set = _temp.toSet();
    _temp = _temp_set.toList();
    std::sort(_temp.begin(), _temp.end());
    ui->comboBoxTrainSet->addItem("Custom");
    ui->comboBoxTrainSet->addItems(_temp);
    ui->comboBoxTrainSet->setCurrentIndex(0);
}

QStringList ROSTTBAppWindow::_create_custom_template_strings()
{
    QStringList _temp = {};

    for(auto templt : _custom_types)
    {
        QString _template = join(";", join(":", "Name", templt->getClass()),
                                 join(":", "MaxSpeed", QString::number(templt->getMaxSpeed())),
                                 join(":", "Mass", QString::number(templt->getMass())),
                                 join(":", "Brake Force", QString::number(templt->getMaxBrake())),
                                 join(":", "Power", QString::number(templt->getMaxPower())));
        _temp.append(_template);
    }

    return _temp;
}

void ROSTTBAppWindow::_save_template()
{
    if(!_cache_dir->exists())
    {
        qDebug() << "Creating Cache Folder at: " << _cache_dir->absolutePath();
        _cache_dir->mkpath(".");
    }
    QFile _cache_file(join(_qt_path_sep, _cache_dir->absolutePath(), "trainset_templates_cache.dat"));

    const int _max_speed = ui->spinBoxMaxSpeed->value(),
              _mass      = ui->spinBoxMass->value(),
              _power     = ui->spinBoxPower->value(),
              _brake     = ui->spinBoxForce->value();

    QStringList _current_templates = _create_custom_template_strings();

    QString _template = join(";", join(":","Name", ui->lineEditTemplateName->text()),
                             join(":", "MaxSpeed", QString::number(_max_speed)),
                             join(":", "Mass", QString::number(_mass)),
                             join(":", "Brake Force", QString::number(_brake)),
                             join(":", "Power", QString::number(_power)));

    _current_templates.append(_template);

    if ( _cache_file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &_cache_file );

        for(auto temp : _current_templates) stream << temp << Qt::endl;
    }

    _cache_file.close();

    _read_in_custom_templates();
}

void ROSTTBAppWindow::on_pushButtonTemplateSave_clicked()
{
    ui->labelDescription->setVisible(true);
    ui->labelInMultiple->setVisible(true);
    ui->labelServiceRef->setVisible(true);
    ui->comboBoxTrainSet->setVisible(true);
    ui->spinBoxMU->setVisible(true);
    ui->servicerefEdit->setVisible(true);
    ui->pushButtonTemplateSave->setVisible(false);
    ui->pushButtonTemplateCancel->setVisible(false);
    ui->lineEditTemplateName->setVisible(false);
    ui->descEdit->setVisible(true);
    ui->labelFromTrainTemplate->setVisible(true);
    ui->pushButtonCreateTemplate->setVisible(true);
    ui->labelTemplateName->setVisible(false);

    _save_template();
}

void ROSTTBAppWindow::on_pushButtonTemplateCancel_clicked()
{
    ui->labelDescription->setVisible(true);
    ui->labelInMultiple->setVisible(true);
    ui->labelServiceRef->setVisible(true);
    ui->comboBoxTrainSet->setVisible(true);
    ui->spinBoxMU->setVisible(true);
    ui->servicerefEdit->setVisible(true);
    ui->pushButtonTemplateSave->setVisible(false);
    ui->pushButtonTemplateCancel->setVisible(false);
    ui->lineEditTemplateName->setVisible(false);
    ui->descEdit->setVisible(true);
    ui->labelFromTrainTemplate->setVisible(true);
    ui->pushButtonCreateTemplate->setVisible(true);
    ui->labelTemplateName->setVisible(false);

}

void ROSTTBAppWindow::on_pushButtonCreateTemplate_clicked()
{
    ui->labelDescription->setVisible(false);
    ui->labelInMultiple->setVisible(false);
    ui->labelServiceRef->setVisible(false);
    ui->comboBoxTrainSet->setVisible(false);
    ui->spinBoxMU->setVisible(false);
    ui->servicerefEdit->setVisible(false);
    ui->pushButtonTemplateSave->setVisible(true);
    ui->pushButtonTemplateCancel->setVisible(true);
    ui->lineEditTemplateName->setVisible(true);
    ui->descEdit->setVisible(false);
    ui->labelFromTrainTemplate->setVisible(false);
    ui->pushButtonCreateTemplate->setVisible(false);
    ui->labelTemplateName->setVisible(true);

}

void ROSTTBAppWindow::on_actionAbout_ROSTTBGen_triggered()
{
    _about_window->setVisible(true);
}

void ROSTTBAppWindow::on_checkBoxAtStation_toggled(bool checked)
{
    // Cannot start a service from a station with non-zero start speed
    if(checked) ui->spinBoxStartSpeed->setValue(0);
    ui->labelTrackIDs->setVisible(checked);
    ui->labelTrackIDstations->setVisible(checked);
    ui->comboBoxTrackIDs->setVisible(checked);
    ui->comboBoxTrackIDStations->setVisible(checked);
    ui->labelDirection->setVisible(checked);
    ui->labelTrackIDRear->setVisible(!checked);
    ui->labelTrackIDFront->setVisible(!checked);
}

void ROSTTBAppWindow::on_radioButton_kph_toggled(bool checked)
{
    ui->start_speed_label->setText(checked ? "Start Speed (kph)" : "Start Speed (mph)");
    ui->max_speed_label->setText(checked ? "Max Speed (kph)" : "Max Speed (mph)");
    ui->spinBoxMaxSpeed->setValue(checked ? std::round(ui->spinBoxMaxSpeed->value()*1.609344) : std::round(ui->spinBoxMaxSpeed->value()/1.609344));
    ui->spinBoxStartSpeed->setValue(checked ? std::round(ui->spinBoxStartSpeed->value()*1.609344) : std::round(ui->spinBoxStartSpeed->value()/1.609344));
}

void ROSTTBAppWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

void ROSTTBAppWindow::_populate_coordinates(QString location)
{
    const QMap<QString, QList<QList<int>>> _coords = _parser->getCoordinates();
    QStringList _coords_for_loc = {};
    _arrows.clear();

    for(auto& coord_set_1 : QSet<QList<int>>(_coords[location].begin(), _coords[location].end()))
    {
        const int x_1 = coord_set_1[0];
        const int y_1 = coord_set_1[1];

        for(auto& coord_set_2 : QSet<QList<int>>(_coords[location].begin(), _coords[location].end()))
        {
            const int x_2 = coord_set_2[0];
            const int y_2 = coord_set_2[1];

            if(x_1 == x_2 && y_1 == y_2) continue;

            bool is_up = x_1 == x_2 && y_2 - y_1 == 1;
            bool is_down = x_1 == x_2 && y_1 - y_2 == 1;
            bool is_left = x_1 - x_2 == 1 && y_1 == y_2;
            bool is_right = x_2 - x_1 == 1 && y_1 == y_2;

            bool is_valid = is_up || is_down || is_left || is_right;

            if(!is_valid) continue;

            const QString _str_repr = "("+QString::fromStdString(std::to_string(coord_set_1[0]))+","+QString::fromStdString(std::to_string(coord_set_1[1]))+") " +
                    "("+QString::fromStdString(std::to_string(coord_set_2[0]))+","+QString::fromStdString(std::to_string(coord_set_2[1]))+")";
            if(_coords_for_loc.contains(_str_repr)) continue;
            _coords_for_loc.push_back(_str_repr);
            _current_coords.push_back({
                QString::fromStdString(std::to_string(coord_set_1[0])).replace("-", "N")+"-"+QString::fromStdString(std::to_string(coord_set_1[1])).replace("-", "N"),
                QString::fromStdString(std::to_string(coord_set_2[0])).replace("-", "N")+"-"+QString::fromStdString(std::to_string(coord_set_2[1])).replace("-", "N")
            });

            if(is_up) _arrows.push_back("↑");
            if(is_down) _arrows.push_back("↓");
            if(is_left) _arrows.push_back("←");
            if(is_right) _arrows.push_back("→");
        }
    }
    ui->comboBoxTrackIDs->clear();
    ui->comboBoxTrackIDs->addItems(_coords_for_loc);
}

void ROSTTBAppWindow::on_comboBoxTrackIDStations_currentTextChanged(const QString &arg1)
{
    if(!arg1.isEmpty())
    {
        ui->comboBoxTrackIDs->setEnabled(true);
        ui->comboBoxTrackIDs->clear();
        _populate_coordinates(arg1);
    }
    else
    {
        ui->comboBoxTrackIDs->setEnabled(false);
    }
}


void ROSTTBAppWindow::on_comboBoxTrackIDs_currentIndexChanged(int index)
{
    if(_arrows.size() > 0 && index >= 0 && index < _arrows.size()) ui->labelDirection->setText(_arrows[index]);
}

