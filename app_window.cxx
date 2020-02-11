#include "app_window.hxx"
#include "ui_rosttbappwindow.h"

ROSTTBAppWindow::ROSTTBAppWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ROSTTBAppWindow)
{
    ui->setupUi(this);
    _tt_model_select = new QItemSelectionModel(ui->tableWidgetTimetable->model());
    connect(_station_add, SIGNAL(on_stationAdd_accepted()), this, SLOT(on_stationAdd_accepted()));
    ui->ROSStatus->setStyleSheet("QLabel { color : red; }");
    ui->SelectedRoute->setStyleSheet("QLabel { color : red; }");
    QHeaderView* _header_ttb = ui->tableWidgetTimetable->horizontalHeader();
    _header_ttb->setStretchLastSection(true);
    QHeaderView* _header_srv = ui->tableWidgetService->horizontalHeader();
    _header_srv->setStretchLastSection(true);
    ui->tableWidgetTimetable->setColumnCount(3);
    ui->tableWidgetService->setColumnCount(3);
    for(int i{0}; i < 3; ++i)
    {
        ui->tableWidgetTimetable->setColumnWidth(i, _ttb_column_widths[i]);
        ui->tableWidgetService->setColumnWidth(i, _ttb_column_widths[i]);
    }
    ui->tableWidgetTimetable->verticalHeader()->setVisible(false);
    ui->tableWidgetTimetable->horizontalHeader()->setVisible(false);
    ui->tableWidgetTimetable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetTimetable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetService->verticalHeader()->setVisible(false);
    ui->tableWidgetService->horizontalHeader()->setVisible(false);
    ui->tableWidgetService->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetService->setSelectionMode(QAbstractItemView::SingleSelection);
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

void ROSTTBAppWindow::_record_current_info()
{
    if(!checkROS()) return;
    QString _srv_id = ui->servicerefEdit->toPlainText();
    QString _desc = ui->descEdit->toPlainText();
    QTime _start_time = ui->starttimeEdit->time();
    const int _max_power   = ui->spinBoxPower->value(),
              _start_speed = ui->spinBoxStartSpeed->value(),
              _mass        = ui->spinBoxMass->value(),
              _max_brake   = ui->spinBoxForce->value(),
              _max_speed   = ui->spinBoxMaxSpeed->value();
    const bool info_missing = _srv_id == "" || _desc == "";
    if(_srv_id.size() != 4)
    {
        _srv_id = _srv_id.left(4);
        QMessageBox::warning(this, "Service ID Length", "Service ID must be 4 characters in length, ID will be spliced to '" + _srv_id+ "'.");

    }
    if(info_missing)
    {
        QMessageBox::critical(this, "Missing Information", "Service Identifier or Description Missing");
        return;
    }
    if(_max_speed == 0 && ui->radioButtonStandard->isChecked())
    {
        QMessageBox::critical(this, "Invalid Maximum Speed", "Maximum service speed cannot be 0 kph for Standard Service");
        return;
    }
    if(_max_power == 0  && !ui->radioButtonStandard->isChecked())
    {
        QMessageBox::critical(this, "Invalid Maximum Power", "Maximum service power cannot be 0 kW for Standard Service");
        return;
    }
    if(_mass == 0  && !ui->radioButtonStandard->isChecked())
    {
        QMessageBox::critical(this, "Invalid Mass", "Mass value cannot be 0 te for Standard Service");
        return;
    }
    if(_max_brake == 0  && !ui->radioButtonStandard->isChecked())
    {
        QMessageBox::critical(this, "Invalid Maximum Braking Force", "Maximum brake force cannot be 0 te Standard Service");
        return;
    }

    _current_timetable->addService(_current_timetable->size(), _start_time, _srv_id, _desc, {}, {}, _start_speed, _max_speed, _mass, _max_brake, _max_power);

    if(ui->radioButtonFeeder->isChecked()) _current_timetable->operator[](-1)->setType(ROSService::ServiceType::ShuttleFinishService);
    else if(ui->radioButtonShuttleStop->isChecked())
    {
        _current_timetable->operator[](-1)->setType(ROSService::ServiceType::ShuttleFromStop);
        int n_repeats = ui->spinBoxRepeats->value(), nmins_interval = ui->spinBoxRepeatInterval->value(), id_inc = ui->spinBoxRefIncrement->value();
        _current_timetable->operator[](-1)->setNRepeats(n_repeats);
        _current_timetable->operator[](-1)->setRepeatInterval(nmins_interval);
        _current_timetable->operator[](-1)->setIDIncrement(id_inc);
    }
    else if(ui->radioButtonShuttleFeeder->isChecked())
    {
        _current_timetable->operator[](-1)->setType(ROSService::ServiceType::ShuttleFromFeeder);

        int n_repeats = ui->spinBoxRepeats->value(), nmins_interval = ui->spinBoxRepeatInterval->value(), id_inc = ui->spinBoxRefIncrement->value();
        _current_timetable->operator[](-1)->setNRepeats(n_repeats);
        _current_timetable->operator[](-1)->setRepeatInterval(nmins_interval);
        _current_timetable->operator[](-1)->setIDIncrement(id_inc);
    }
    else if(ui->radioButtonFromOther->isChecked() && ui->checkBoxParentSplit->isChecked()) _current_timetable->operator[](-1)->setType(ROSService::ServiceType::ServiceFromSplit);
    else if(ui->radioButtonFromOther->isChecked()) _current_timetable->operator[](-1)->setType(ROSService::ServiceType::ServiceFromService);
    else _current_timetable->operator[](-1)->setType(ROSService::ServiceType::Service);

    _update_output();
}

void ROSTTBAppWindow::_update_output()
{
    // IMPORTANT: If Timetable is Empty there is nothing to show!
    if(_current_timetable->size() < 1) return;
    ui->tableWidgetTimetable->setRowCount(0);
    ui->tableWidgetService->setRowCount(0);

    if(_current_timetable->size() > 0) _current_timetable->orderServices();

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


    QList<QString> _current_element_stations = _current_service_selection->getStations();
    QList<QList<QTime>> _current_element_times = _current_service_selection->getTimes();

    for(int i{0}; i < _current_element_stations.size(); ++i)
    {
        QTime _arrival_time = _current_element_times[i][0],
              _depart_time  = _current_element_times[i][1];
        QStringList _station_item =  {_arrival_time.toString("HH:mm"), _depart_time.toString("HH:mm"), _current_element_stations[i]};
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

}

void ROSTTBAppWindow::_enable_integer_info(bool enable)
{
    ui->descEdit->setEnabled(enable);
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
    _open_file_str = _parser->parse_file(_current_file, _ros_timetables);
    _current_timetable = _parser->getParsedTimetable();
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
    _current_file = new QFileDialog(this);
    delete _parser;
    _parser = new ROSTTBGen(this);
    _current_timetable = new ROSTimetable;
    Station_add* _station_add = new Station_add(nullptr, this);
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

void ROSTTBAppWindow::on_pushButtonInsert_clicked()
{
    _record_current_info();
    _current_service_selection = _current_timetable->operator[](-1);
    _station_add->setCurrentService(_current_service_selection);
}

void ROSTTBAppWindow::delete_entries()
{
    QModelIndexList _entries = _tt_model_select->selectedRows();
    for(auto i : _entries)
    {
        ui->tableWidgetTimetable->removeRow(i.row());
    }
}

void ROSTTBAppWindow::on_pushButtonDelete_clicked()
{
    delete_entries();
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
    _station_add->setStations(_parser->getStations());
    _station_add->show();

}

void ROSTTBAppWindow::on_stationAdd_accepted()
{
    _update_output();
}

void ROSTTBAppWindow::on_radioButtonStandard_toggled(bool checked)
{
    _enable_integer_info(true);
    if(checked)
    {
        ui->textEditEnterID1->setEnabled(true);
        ui->textEditEnterID2->setEnabled(true);
        ui->textEditFeederRef->setEnabled(false);
        ui->textEditParentRef->setEnabled(false);
        ui->textEditShuttleRef->setEnabled(false);
        ui->spinBoxRepeats->setEnabled(false);
        ui->spinBoxRepeatInterval->setEnabled(false);
        ui->checkBoxParentSplit->setEnabled(false);
        ui->spinBoxRefIncrement->setEnabled(false);
    }
}

void ROSTTBAppWindow::on_radioButtonShuttleFeeder_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
        ui->textEditEnterID1->setEnabled(false);
        ui->textEditEnterID2->setEnabled(false);
        ui->textEditFeederRef->setEnabled(true);
        ui->textEditParentRef->setEnabled(false);
        ui->textEditShuttleRef->setEnabled(false);
        ui->spinBoxRepeats->setEnabled(true);
        ui->spinBoxRepeatInterval->setEnabled(true);
        ui->checkBoxParentSplit->setEnabled(false);
        ui->spinBoxRefIncrement->setEnabled(true);

    }
}

void ROSTTBAppWindow::on_radioButtonShuttleStop_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
        ui->textEditEnterID1->setEnabled(true);
        ui->textEditEnterID2->setEnabled(true);
        ui->textEditFeederRef->setEnabled(false);
        ui->textEditParentRef->setEnabled(false);
        ui->textEditShuttleRef->setEnabled(false);
        ui->spinBoxRepeats->setEnabled(true);
        ui->spinBoxRepeatInterval->setEnabled(true);
        ui->checkBoxParentSplit->setEnabled(false);
        ui->spinBoxRefIncrement->setEnabled(true);
    }
}

void ROSTTBAppWindow::on_radioButtonFeeder_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
        ui->textEditEnterID1->setEnabled(false);
        ui->textEditEnterID2->setEnabled(false);
        ui->textEditFeederRef->setEnabled(false);
        ui->textEditParentRef->setEnabled(false);
        ui->textEditShuttleRef->setEnabled(true);
        ui->spinBoxRepeats->setEnabled(false);
        ui->spinBoxRepeatInterval->setEnabled(false);
        ui->checkBoxParentSplit->setEnabled(false);
        ui->spinBoxRefIncrement->setEnabled(false);
    }
}

void ROSTTBAppWindow::on_radioButtonFromOther_toggled(bool checked)
{
    _enable_integer_info(false);
    if(checked)
    {
        ui->textEditEnterID1->setEnabled(false);
        ui->textEditEnterID2->setEnabled(false);
        ui->textEditFeederRef->setEnabled(false);
        ui->textEditParentRef->setEnabled(true);
        ui->textEditShuttleRef->setEnabled(false);
        ui->spinBoxRepeats->setEnabled(false);
        ui->spinBoxRepeatInterval->setEnabled(false);
        ui->checkBoxParentSplit->setEnabled(true);
        ui->spinBoxRefIncrement->setEnabled(false);
    }
}

void ROSTTBAppWindow::on_tableWidgetTimetable_cellClicked(int row, int column)
{
    if(ui->tableWidgetTimetable->rowCount() < 1) return;
    _current_service_selection = _current_timetable->operator[](ui->tableWidgetTimetable->takeItem(row, 1)->text());
    _update_output();
    ui->tableWidgetTimetable->selectRow(row);
}

void ROSTTBAppWindow::save_file()
{
    QStringList _ttb = _parser->createTimetableStrings(_current_timetable);
    QString _output = join(QChar::Null, _ttb);

    QFile _file(_open_file_str);
    if ( _file.open(QIODevice::ReadWrite) )
    {
        QTextStream out(&_file);
        out << _output;
    }
}

void ROSTTBAppWindow::on_actionSave_triggered()
{
    save_file();
}
