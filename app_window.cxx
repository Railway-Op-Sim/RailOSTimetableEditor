#include "app_window.hxx"
#include "ui_rosttbappwindow.h"

ROSTTBAppWindow::ROSTTBAppWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ROSTTBAppWindow)
{
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
    ui->radioButtonFrh->setChecked(true);
    ui->servicerefEdit->setMaxLength(4);
    ui->serviceFinishServiceEdit->setMaxLength(4);
    ui->textEditShuttlePart2->setMaxLength(4);
    ui->textEditParentShuttleRef->setMaxLength(4);
    _station_add->setServiceTable(ui->tableWidgetService);

    QDir cache_dir("cache");
    if(!cache_dir.exists()) cache_dir.mkpath(".");
    QFile _cache_file(cache_dir.absolutePath()+"/"+"ros_location_cache.dat");

    if(_cache_file.exists())
    {
        if(!_cache_file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(0,"error",_cache_file.errorString());
        }
        QTextStream in(&_cache_file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            _ros_timetables = new QDir(line);
            ui->ROSStatus->setText(line);
            ui->ROSStatus->setStyleSheet("QLabel { color : green; }");
            break;
        }
        _set_initial_open_file();
    }

    _cache_file.close();

    ui->radioButtonStandard->setEnabled(true);
    ui->radioButtonStandard->toggle();

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

void ROSTTBAppWindow::_record_current_info()
{
    ui->labelParentOfJoin->setText("");
    if(!checkROS()) return;
    QString _srv_id = ui->servicerefEdit->text();
    const QString _desc = ui->descEdit->text();
    const QTime _start_time = ui->starttimeEdit->time();
    const QString _shuttle_partner = ui->textEditShuttlePart2->text();
    if(_current_timetable->getStartTime().msecsTo(_start_time) < 0)
    {
        QMessageBox::critical(this, "Invalid Service", "Service start time must not precede timetable start time.");
        return;
    }
    const int _max_power   = ui->spinBoxPower->value(),
              _start_speed = ui->spinBoxStartSpeed->value(),
              _mass        = ui->spinBoxMass->value(),
              _max_brake   = ui->spinBoxForce->value(),
              _max_speed   = ui->spinBoxMaxSpeed->value();
    const QStringList _start_ids = {ui->textEditEnterID1->text(), ui->textEditEnterID2->text()};
    const bool info_missing = _srv_id == "" || _desc == "";
    if(info_missing)
    {
        QMessageBox::critical(this, "Missing Information", "Service Identifier or Description Missing");
        return;
    }

    if(ui->radioButtonStandard->isChecked() || ui->radioButtonShuttleStop->isChecked())
    {
        if(_max_speed == 0)
        {
            QMessageBox::critical(this, "Invalid Maximum Speed", "Maximum service speed cannot be 0 kph for Standard Service or Shuttle from Stop.");
            return;
        }
        if(_max_power == 0)
        {
            QMessageBox::critical(this, "Invalid Maximum Power", "Maximum service power cannot be 0 kW for Standard Service or Shuttle from Stop.");
            return;
        }
        if(_mass == 0)
        {
            QMessageBox::critical(this, "Invalid Mass", "Mass value cannot be 0 te for Standard Service or Shuttle from Stop.");
            return;
        }
        if(_max_brake == 0)
        {
            QMessageBox::critical(this, "Invalid Maximum Braking Force", "Maximum brake force cannot be 0 te for Standard Service or Shuttle from Stop.");
            return;
        }
        if(_start_ids[0] == "" || _start_ids[1] == "")
        {
            QMessageBox::critical(this, "Invalid Start Position", "IDs for start location rear and front track elements must be provided for a Standard Service or Shuttle from Stop.");
            return;
        }

        for(auto id : _start_ids)
        {
            if(!checkLocID(id))
            {
                QMessageBox::critical(this, "Invalid Start Position", "Location IDs must have the form 'A-B' where each of A and B can be either a number or a letter followed by a number.");
                return;
            }
        }

        if(!checkIDsAreNeighbours(_start_ids))
        {
            QMessageBox::critical(this, "Invalid Start Position", "Location IDs must be neighbouring map elements.");
            return;
        }
    }

    if(ui->checkBoxManualTimeEdit->isChecked())
    {
        if(ui->timeEditTermination->time().secsTo(_current_service_selection->getStartTime()) > 0)
        {
            QMessageBox::critical(this, "Invalid Exit Time", "Exit time cannot be before start time");
            return;
        }
    }

    if(!_current_timetable->getServices().contains(_srv_id))
    {
        _current_timetable->addService(_current_timetable->size(), _start_time, _srv_id, _desc, _start_speed, _max_speed, _mass, _max_brake, _max_power);
        _current_service_selection = _current_timetable->operator[](-1);
        _current_service_selection->setLabelledLocationStart(ui->checkBoxAtStation);
    }

    else
    {
        _current_service_selection->setEntryTime(_start_time);
        _current_service_selection->setDescription(_desc);
        _current_service_selection->setStartSpeed(_start_speed);
        _current_service_selection->setMaxSpeed(_max_speed);
        _current_service_selection->setMaxBrake(_max_brake);
        _current_service_selection->setPower(_max_power);
        _current_service_selection->setLabelledLocationStart(ui->checkBoxAtStation);
    }

    if(ui->checkBoxManualTimeEdit->isChecked())
    {
        _current_service_selection->setExitTime(ui->timeEditTermination->time());
    }

    if(ui->radioButtonShuttleFinish->isChecked())
    {
        const ROSService* _parent = _current_timetable->getServices()[ui->comboBoxFeeder->currentText()];
        QTime _parent_last_time = _parent->getTimes()[_parent->getTimes().size()-1][0];

        const QString _service_id = _current_timetable->getServices()[_current_service_selection->getParent()]->getDaughter();
        if(_service_id == "")
        {
            QMessageBox::critical(this, "No Parent Daughter Found", "Could not find reference to this new service within definition of parent service. Failed to retrieve ID for current service.");
            return;
        }
        if(ui->starttimeEdit->time().msecsTo(_parent_last_time) < 0)
        {
            QMessageBox::critical(this, "Invalid Start Time", "Daughter service start time cannot be before parent arrival time.");
            return;
        }

        _current_service_selection->setType(ROSService::ServiceType::ShuttleFinishService);
        _current_service_selection->setEntryTime(_parent_last_time);
        _current_service_selection->setParent(ui->comboBoxFeeder->currentText());
        _current_service_selection->setID(_service_id);
        ui->servicerefEdit->setText(_service_id);
    }
    else if(ui->radioButtonShuttleStop->isChecked())
    {
        if(_shuttle_partner == "")
        {
            QMessageBox::critical(this, "No Partner", "You must specify an accompanying return service ID.");
            return;
        }
        _current_service_selection->setDaughter(_shuttle_partner);
        _current_service_selection->setType(ROSService::ServiceType::ShuttleFromStop);
        _current_service_selection->setEntryPoint(_start_ids);
        int n_repeats = ui->spinBoxRepeats->value(), nmins_interval = ui->spinBoxRepeatInterval->value(), id_inc = ui->spinBoxRefIncrement->value();
        _current_service_selection->setNRepeats(n_repeats);
        _current_service_selection->setRepeatInterval(nmins_interval);
        _current_service_selection->setIDIncrement(id_inc);
    }
    else if(ui->radioButtonShuttleFeeder->isChecked())
    {
        if(_shuttle_partner == "")
        {
            QMessageBox::critical(this, "No Partner", "You must specify an accompanying return service ID.");
            return;
        }
        _current_service_selection->setDaughter(_shuttle_partner);
        _current_service_selection->setType(ROSService::ServiceType::ShuttleFromFeeder);

        int n_repeats = ui->spinBoxRepeats->value(), nmins_interval = ui->spinBoxRepeatInterval->value(), id_inc = ui->spinBoxRefIncrement->value();
        _current_service_selection->setNRepeats(n_repeats);
        _current_service_selection->setRepeatInterval(nmins_interval);
        _current_service_selection->setIDIncrement(id_inc);
        _current_service_selection->setParent(ui->comboBoxFeeder->currentText());
    }
    else if(ui->radioButtonFromOther->isChecked())
    {
        const ROSService* _parent = _current_timetable->getServices()[ui->comboBoxParent->currentText()];
        QTime _parent_last_time = _parent->getTimes()[_parent->getTimes().size()-1][0];

        if(ui->starttimeEdit->time().msecsTo(_parent_last_time) < 0)
        {
            QMessageBox::critical(this, "Invalid Start Time", "Daughter service start time cannot be before parent end time.");
            return;
        }
        const QString _service_id = _parent->getDaughter();
        if(_service_id == "")
        {
            QMessageBox::critical(this, "No Parent Daughter Found", "Could not find reference to this new service within definition of parent service. Failed to retrieve ID for current service.");
            return;
        }
        if(ui->starttimeEdit->time().msecsTo(_parent_last_time) < 0)
        {
            QMessageBox::critical(this, "Invalid Start Time", "Daughter service start time cannot be before parent arrival time.");
            return;
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

    if(ui->radioButtonFer->isChecked())
    {
        const QString _exit_id = ui->serviceExitEdit->text();
        if(_exit_id == "")
        {
            QMessageBox::critical(this, "No Exit Location", "You must provide an exit location ID.");
            return;
        }

        if(!checkLocID(_exit_id))
        {
            QMessageBox::critical(this, "Invalid Start Position", "Location ID must have the form 'A-B' where each of A and B can be either a number or a letter followed by a number.");
            return;
        }

        _current_service_selection->setExitPoint(_exit_id);
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

        if(_found_candidate == "NULL")
        {
            QMessageBox::critical(this, "No Parent Found", "Could not find split reference in existing service matching current service. You must define a forward or rear split "\
                                  "in an existing service first.");
            return;
        }

        ui->labelParentOfJoin->setText(_found_candidate);
        _current_service_selection->setParent(_found_candidate);
    }

    else if(ui->radioButtonFns->isChecked() || ui->radioButtonFshf->isChecked() || ui->radioButtonFrsfns->isChecked())
    {
        QString _srv_id = ui->serviceFinishServiceEdit->text();

        if(_srv_id == "")
        {
            QMessageBox::critical(this, "Missing Daughter Service", "You must provide an identifier for the newly formed service.");
            return;
        }

        _current_service_selection->setDaughter(_srv_id);
        if(ui->radioButtonFns->isChecked()) _current_service_selection->setFinishState(ROSService::FinishState::FinishFormNew);
        else if(ui->radioButtonFshf->isChecked()) _current_service_selection->setFinishState(ROSService::FinishState::FinishShuttleFormNew);
        else _current_service_selection->setFinishState(ROSService::FinishState::FinishSingleShuttleFeeder);
    }

    else if(ui->radioButtonFrh->isChecked()) _current_service_selection->setFinishState(ROSService::FinishState::FinishRemainHere);
    else _current_service_selection->setFinishState(ROSService::FinishState::FinishShuttleRemainHere);

    _update_output();
}

void ROSTTBAppWindow::_update_output(ROSService* current_serv)
{
    // Do not allow daughter service creation without at least one timetable entry already present
    if(_current_timetable->size() > 0)
    {
        ui->radioButtonFromOther->setEnabled(true);
        ui->radioButtonShuttleFeeder->setEnabled(true);
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

        if(_current_service_selection->getDaughter() != "" && i == _current_element_stations.size()-1)
        {
            _ns_string = "→\t"+_current_service_selection->getDaughter();
        }

        if(_current_service_selection->getCDTPass(_current_element_stations[i])[0])
        {
            _ns_string += "Change Direction "+_current_service_selection->getCDTTimes()[i].toString("HH:mm")+"\t";
        }

        else if(_current_service_selection->getCDTPass(_current_element_stations[i])[1])
        {
            _ns_string = "Pass";
        }

        QStringList _station_item =  {_arrival_time.toString("HH:mm"), _depart_time.toString("HH:mm"), _current_element_stations[i], _ns_string};
        ui->tableWidgetService->insertRow(ui->tableWidgetService->rowCount());
        for(int j{0}; j < _station_item.size(); ++j)
        {
            QTableWidgetItem* _new_time_item = new QTableWidgetItem(_station_item[j], 0);
            _new_time_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            ui->tableWidgetService->setItem(ui->tableWidgetService->rowCount()-1, j, _new_time_item);
        }
    }

    ui->tableWidgetService->sortItems(0);
    ui->tableWidgetTimetable->sortItems(0);

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
    reset();
}

void ROSTTBAppWindow::on_actionSave_As_triggered()
{
    _open_file_str = _current_file->getSaveFileName(this, "Save As", _ros_timetables->dirName(), "Timetable (*.ttb)" );
    save_file();
}

void ROSTTBAppWindow::open_file()
{
    if(!_ros_timetables)
    {
        QMessageBox::critical(this, "Invalid Application Address", "You need to first set the location of the Railway Operation Simulator executable.");
        return;
    }
    reset();
    QString _parsed = _parser->parse_file(_current_file, _ros_timetables);
    _open_file_str = (_parsed != "NULL")? _parsed : _open_file_str;
    _current_timetable = _parser->getParsedTimetable();
    if(_current_timetable->getServices().size() > 0)
    {
        ui->radioButtonFromOther->setEnabled(true);
        ui->radioButtonShuttleFeeder->setEnabled(true);
    }
    _current_service_selection = _current_timetable->operator[](-1);
    _update_output();
}

void ROSTTBAppWindow::_set_initial_open_file()
{
    QString _split_str = (QSysInfo::productType() == "windows") ? "\\" : "/";
    _open_file_str = _ros_timetables->path()+_split_str+_current_timetable->getFileName();
}

void ROSTTBAppWindow::on_pushButtonROSLoc_clicked()
{
    QString _file_name = _current_file->getOpenFileName(this, "Open File", QDir::currentPath(),
                                                        "ROS Executable (*.exe, *.app)");
    if(_file_name == QString()) return;

    QStringList _dot_split = _file_name.split(".");
    QString _type = _dot_split[_dot_split.size()-1];
    QString _split_str = (QSysInfo::productType() == "windows") ? "\\" : "/";
    QStringList _locations = _file_name.split(_split_str);
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
    QString _directory = (QSysInfo::productType() == "windows") ? "Program timetables" : "Program\\ timetables";
    _locations.append(_directory);
    _ros_timetables = new QDir(_locations.join(_split_str));
    ui->ROSStatus->setText(_file_name);
    ui->ROSStatus->setStyleSheet("QLabel { color : green; }");
    _set_initial_open_file();
    QFile _cache_file("cache/ros_location_cache.dat");
    if ( _cache_file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &_cache_file );
        stream << _ros_timetables->absolutePath() << endl;
    }
}

void ROSTTBAppWindow::reset()
{
    delete _current_file;
    delete _parser;
    _parser = new ROSTTBGen(this);
    ui->tableWidgetService->setRowCount(0);
    ui->tableWidgetTimetable->setRowCount(0);
    ui->tableWidgetService->clear();
    ui->tableWidgetTimetable->clear();
    _current_file = new QFileDialog(this);
    _current_timetable = new ROSTimetable;
    _station_add = new Station_add(nullptr, this);
}

bool ROSTTBAppWindow::checkROS()
{
    if(!_ros_timetables)
    {
        QMessageBox::critical(this, "Invalid Application Address", "You need to first set the location of the Railway Operation Simulator executable.");
        return false;
    }

    return true;
}

void ROSTTBAppWindow::_clear()
{
    ui->servicerefEdit->clear();
    ui->spinBoxMaxSpeed->setValue(0);
    ui->spinBoxForce->setValue(0);
    ui->spinBoxRepeats->setValue(0);
    ui->spinBoxStartSpeed->setValue(0);
    ui->spinBoxRefIncrement->setValue(0);
    ui->spinBoxRepeatInterval->setValue(0);
    ui->textEditEnterID1->setText("");
    ui->textEditEnterID2->setText("");
    ui->checkBoxAtStation->setChecked(true);
    ui->serviceFinishServiceEdit->setText("");
    ui->textEditParentShuttleRef->setText("");
    ui->descEdit->setText("");
    ui->spinBoxMass->setValue(0);
    ui->spinBoxPower->setValue(0);
}

void ROSTTBAppWindow::on_pushButtonInsert_clicked()
{
    _record_current_info();
    _station_add->setCurrentService(_current_service_selection);
    _clear();
}

void ROSTTBAppWindow::delete_entries()
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
    _current_timetable->removeService(_current_service_selection->getID());
    _current_service_selection = (_current_timetable->size() == 0) ? nullptr : _current_timetable->operator[](-1);
    _update_output();
}

void ROSTTBAppWindow::on_pushButtonDelete_clicked()
{
    delete_entries();
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
    QString _split_str = (QSysInfo::productType() == "windows") ? "\\" : "/";
    QStringList _parts = _open_file_str.split(_split_str);
    _parts.pop_back();
    _parts.pop_back();
    _parts.push_back("Railways");
    QString _railways_dir = _parts.join(_split_str);
    QString _parsed = _parser->parse_rly(_railways_dir);
    _current_route = (_parsed != "NULL") ? _parsed : _current_route;
    _parts = _current_route.split(_split_str);
    ui->SelectedRoute->setText(_parts[_parts.size()-1]);
    ui->SelectedRoute->setStyleSheet("QLabel { color : green; }");

}

void ROSTTBAppWindow::on_pushButtonAddLocation_clicked()
{
    if(_current_timetable->size() == 0)
    {
        QMessageBox::critical(this, "No Service Defined", "You must add a service before selecting calling points.");
        return;
    }
    if(_parser->getStations().size() == 0)
    {
      QMessageBox::critical(this, "No Route", "You must open the relevant route before adjusting an existing timetable.");
      ui->pushButtonRoute->click();
      _update_output();
      return;
    }
    _station_add->setCurrentService(_current_service_selection);
    _station_add->setStations(_parser->getStations());
    _station_add->show();
    _station_add->fwdPreviousEventTime((_current_service_selection->getTimes().size() > 0) ? _current_service_selection->getTimes()[_current_service_selection->getTimes().size()-1][1] : _current_service_selection->getStartTime());

    _update_output();
}

void ROSTTBAppWindow::on_radioButtonStandard_toggled(bool checked)
{
    _enable_integer_info(true);
    if(checked)
    {
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

void ROSTTBAppWindow::on_radioButtonFeeder_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
        ui->servicerefEdit->setEnabled(true);
        ui->starttimeEdit->setEnabled(true);
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

        ui->spinBoxRefIncrement->setEnabled(false);
        ui->spinBoxRefIncrement->clear();
        ui->spinBoxRepeatInterval->setEnabled(false);
        ui->spinBoxRepeats->clear();
        ui->spinBoxRepeats->setEnabled(false);
        ui->textEditEnterID1->clear();
        ui->textEditEnterID1->setEnabled(false);
        ui->textEditEnterID2->clear();
        ui->textEditEnterID2->setEnabled(false);
        ui->comboBoxFeeder->clear();
        ui->comboBoxFeeder->setEnabled(false);
        ui->comboBoxParent->clear();
        ui->comboBoxParent->setEnabled(false);
        ui->textEditParentShuttleRef->setEnabled(true);
    }
}

void ROSTTBAppWindow::on_radioButtonFromOther_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
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
        ui->textEditParentShuttleRef->setEnabled(false);
    }
}

void ROSTTBAppWindow::_set_form_info()
{

    const QString _serv_id = _current_service_selection->getID(),
          _description = _current_service_selection->getDescription(),
          _feeder_ref  = _current_service_selection->getParent(),
          _daughter_ref = _current_service_selection->getDaughter(),
          _exit_loc = _current_service_selection->getExitID();

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

    ui->textEditEnterID1->setText(_start_ids[0]);
    ui->textEditEnterID2->setText(_start_ids[1]);
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
            ui->serviceExitEdit->setText(_exit_loc);
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
    _current_service_selection = _current_timetable->operator[](ui->tableWidgetTimetable->takeItem(row, 1)->text());
    qDebug() << "Selected: " << _current_service_selection->summarise();
    _update_output();
    ui->tableWidgetTimetable->selectRow(row);
    _set_form_info();
}

void ROSTTBAppWindow::save_file()
{
    QStringList _ttb = _parser->createTimetableStrings(_current_timetable);
    QString _output = join(QChar::Null, _ttb);

    qDebug() << _open_file_str;

    QFile _file(_open_file_str);
    if ( _file.open(QIODevice::ReadWrite) )
    {
        QTextStream out(&_file);
        out << _output;
    }

    _file.close();
}

void ROSTTBAppWindow::on_actionSave_triggered()
{
    save_file();
}

void ROSTTBAppWindow::on_pushButtonTTBTime_clicked()
{
    QTime _temp_time = ui->timeEditTTBStart->time();
    ui->starttimeEdit->setTime(_temp_time);
    for(auto service : _current_timetable->getServices())
    {
        if(service->getStartTime().msecsTo(_temp_time) < 0)
        {
            QMessageBox::critical(this, "Invalid Start Time", "Start time for timetable cannot be after first service time.");
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
      QMessageBox::critical(this, "No Route", "You must open the relevant route before adjusting an existing timetable.");
      ui->pushButtonRoute->click();
      _update_output();
      return;
    }
    _station_add->setEditMode(true);
    _station_add->setCurrentService(_current_service_selection);
    _station_add->setStations(_parser->getStations());
    const QString _station_name = ui->tableWidgetService->takeItem(row, 2)->text();

    if(!_parser->getStations().contains(_station_name))
    {
        QMessageBox::critical(this, "No Route", "You must open the relevant route before adjusting an existing timetable (station not found in current route).");
        ui->pushButtonRoute->click();
        _update_output();
        return;
    }

    const QList<bool> _cdtPass = _current_service_selection->getCDTPass(_station_name);
    const QString _arrive = ui->tableWidgetService->takeItem(row, 0)->text(),
                _depart = ui->tableWidgetService->takeItem(row, 1)->text();

    _station_add->fwdCurrentSelection(_station_name, {QTime::fromString(_arrive, "HH:mm"), QTime::fromString(_depart, "HH:mm")}, _cdtPass[0], _cdtPass[1]);

    _station_add->show();
    _update_output();
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
    _clone_srv->show();
}

void ROSTTBAppWindow::on_pushButtonClone_clicked()
{
    _clone_current();
    if(!_clone_srv->getNewService()) return;
    _update_output();
}

void ROSTTBAppWindow::on_pushButtonClear_clicked()
{
    _clear();
}

void ROSTTBAppWindow::on_comboBoxParent_currentTextChanged(const QString &arg1)
{
    if(_current_timetable->size() < 1 || !_current_timetable->getServices().contains(arg1) || _current_service_selection->getID() == arg1) return;
    const ROSService* _parent = _current_timetable->getServices()[arg1];
    QTime _parent_last_time = _parent->getTimes()[_parent->getTimes().size()-1][0];
    ui->starttimeEdit->setTime(_parent_last_time);
    ui->servicerefEdit->setText(_parent->getDaughter());
}

void ROSTTBAppWindow::on_checkBoxManualTimeEdit_toggled(bool checked)
{
    ui->timeEditTermination->setEnabled(checked);
}
