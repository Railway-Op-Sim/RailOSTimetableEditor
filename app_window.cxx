#include "app_window.hxx"
#include "ui_rosttbappwindow.h"

ROSTTBAppWindow::ROSTTBAppWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ROSTTBAppWindow)
{
    ui->setupUi(this);
    connect(_station_add, SIGNAL(accepted()), this, SLOT(on_stationAdd_accepted()));
    ui->ROSStatus->setStyleSheet("QLabel { color : red; }");
    ui->SelectedRoute->setStyleSheet("QLabel { color : red; }");
    _timetable_table->setGeometry(440,50,900,541);
    _service_table->setGeometry(50,50,371,541);
    QHeaderView* _header_ttb = _timetable_table->horizontalHeader();
    _header_ttb->setStretchLastSection(true);
    QHeaderView* _header_srv = _service_table->horizontalHeader();
    _header_srv->setStretchLastSection(true);
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
    if(_max_speed == 0)
    {
        QMessageBox::critical(this, "Invalid Maximum Speed", "Maximum service speed cannot be 0 kph");
        return;
    }
    if(_max_power == 0)
    {
        QMessageBox::critical(this, "Invalid Maximum Power", "Maximum service power cannot be 0 kW");
        return;
    }
    if(_mass == 0)
    {
        QMessageBox::critical(this, "Invalid Mass", "Mass value cannot be 0 tonnes");
        return;
    }
    if(_max_brake == 0)
    {
        QMessageBox::critical(this, "Invalid Maximum Braking Force", "Maximum brake force cannot be 0N");
        return;
    }
    _current_timetable->addService(_start_time, _srv_id);
    _current_timetable->operator[](-1)->setDescription(_desc);
    _current_timetable->operator[](-1)->setMass(_mass);
    _current_timetable->operator[](-1)->setMaxBrake(_max_brake);
    _current_timetable->operator[](-1)->setStartSpeed(_start_speed);
    _current_timetable->operator[](-1)->setMaxSpeed(_max_speed);
    _current_timetable->operator[](-1)->setPower(_max_power);
    qDebug() << "Added Service: " << _current_timetable->operator[](-1)->as_string() << "\n";
    _update_output();
}

void ROSTTBAppWindow::_update_output()
{
    _timetable_table->setRowCount(0);
    _service_table->setRowCount(0);

    _current_timetable->orderServices();

    for(int i{0}; i < _current_timetable->size(); ++i)
    {
        QString out = _current_timetable->operator[](i)->summarise();
        QTableWidgetItem* _new_service_item = new QTableWidgetItem(out, 0);
        _new_service_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        if(_timetable_table->columnCount() == 0) _timetable_table->insertColumn(_timetable_table->columnCount());
        _timetable_table->insertRow(_timetable_table->rowCount());
        _timetable_table->setItem(_timetable_table->rowCount()-1, 0, _new_service_item);
    }

    QList<QString> _current_element_stations = _current_timetable->operator[](_current_service_selection)->getStations();
    QMap<int, QList<QTime>> _current_element_times = _current_timetable->operator[](_current_service_selection)->getTimes();

    qDebug() << "Processing Stations..." << "\n";
    for(int i{0}; i < _current_element_stations.size(); ++i)
    {
        QTime _arrival_time = _current_element_times[i][0],
              _depart_time  = _current_element_times[i][1];
        QString _station_item =  _arrival_time.toString("HH:mm")+"\t"+_depart_time.toString("HH:mm")+"\t"+_current_element_stations[i];
        qDebug() << "Appending Station: " << _station_item << "\n";
        QTableWidgetItem* _new_time_item = new QTableWidgetItem(_station_item, 0);
        _new_time_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        if(_service_table->columnCount() == 0) _service_table->insertColumn(_service_table->columnCount());
        _service_table->insertRow(_service_table->rowCount());
        _service_table->setItem(_service_table->rowCount()-1, 0, _new_time_item);
    }
}

void ROSTTBAppWindow::on_actionOpen_triggered()
{
    open_file();
}

void ROSTTBAppWindow::on_actionNew_triggered()
{
    reset();
}

void ROSTTBAppWindow::_save_as()
{
    _open_file_str = _current_file->getSaveFileName(this, "Save As", _ros_timetables->dirName(), "Timetable (*.ttb)" );
}

void ROSTTBAppWindow::on_actionSave_As_triggered()
{
    _save_as();
}

void ROSTTBAppWindow::open_file()
{
    _open_file_str = _current_file->getOpenFileName(this, "Open File", QDir::currentPath(),
                                   "All files (*.*) ;; ROS Timetable Files (*.ttb)");
    if(_open_file_str == QString()) return;
    _parser->parse_file(_current_file);
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
    _open_file_str = _ros_timetables->path()+_split_str+_current_timetable->getFileName();
    qDebug() << "ROS Timetables Location Set to " << _locations.join(_split_str) << "\n";
}

void ROSTTBAppWindow::reset()
{
    delete _current_file;
    _current_file = new QFileDialog(this);
    delete _parser;
    _parser = new ROSTTBGen;
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
    _current_service_selection = _current_timetable->size()-1;
    _station_add->setCurrentService(_current_timetable->operator[](_current_service_selection));
}

void ROSTTBAppWindow::delete_entries()
{
    QModelIndexList _entries = _tt_model_select->selectedRows();
    for(auto i : _entries)
    {
        _timetable_table->removeRow(i.row());
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
    qDebug() << "Railways Directory: " << _railways_dir << "\n";
    QString _parsed = _parser->parse_rly(this, _railways_dir);
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
