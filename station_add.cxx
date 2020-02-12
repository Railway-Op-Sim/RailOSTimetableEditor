#include "station_add.hxx"
#include "ui_station_add.h"

Station_add::Station_add(ROSService* service, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Station_add),
    _current_srv(service)
{
    ui->setupUi(this);
    ui->errorLabel->setStyleSheet("QLabel { color : red; }");
    ui->checkBoxCDT->setChecked(false);
    ui->timeEditCDT->setEnabled(false);

}

void Station_add::setStations(QSet<QString> stations)
{
    _station_list = QList<QString>(stations.begin(), stations.end());
    std::sort(_station_list.begin(), _station_list.end());
    ui->comboBoxStations->addItems(_station_list);
}

Station_add::~Station_add()
{
    delete ui;
}

bool Station_add::setInfo()
{
    if(_srv_start.msecsTo(ui->timeEditArrival->time()) < 0)
    {
        ui->errorLabel->setText("Arrival time cannot be before service start");
        return false;
    }
    if(ui->timeEditArrival->time().msecsTo(ui->timeEditDeparture->time()) < 0)
    {
        ui->errorLabel->setText("Departure time cannot be before arrival.");
        return false;
    }
    ui->errorLabel->setText("");
    _current_station = ui->comboBoxStations->currentText();

    _times[0] = (_stop_class != StopType::StartPoint) ? ui->timeEditArrival->time() : QTime();
    _times[1] = (_stop_class != StopType::Terminus) ? ui->timeEditDeparture->time() : QTime();

    return true;
}

void Station_add::fwdCurrentSelection(const QString& station, const QList<QTime>& times, const bool isCDT, const bool isPass)
{
    ui->checkBoxCDT->setEnabled(isCDT);
    ui->checkBoxPASS->setEnabled(isPass);
    ui->timeEditArrival->setTime(times[0]);
    ui->timeEditDeparture->setTime(times[1]);
    ui->comboBoxStations->setCurrentText(station);
}

void Station_add::on_buttonBoxAddStation_accepted()
{
    if(setInfo())
    {
        this->close();

        if(_current_srv->getStations().contains(_current_station))
        {
            _current_srv->updateStation(_current_station, _times, ui->checkBoxCDT->isChecked(), ui->checkBoxPASS->isChecked());

            this->close();

            reset_state();

        }

        else
        {
            _current_srv->addStation(_times, _current_station);
            _current_srv->setStopAsPassPoint(_current_srv->getStations().size()-1, ui->checkBoxPASS->isChecked());
            _current_srv->setDirectionChangeAtStop(_current_srv->getStations().size()-1, ui->checkBoxCDT->isChecked());
        }

        _service_table->setRowCount(0);

        for(int i{0}; i < _current_srv->getStations().size(); ++i)
        {
            QTime _arrival_time = _current_srv->getTimes()[i][0],
                  _depart_time  = _current_srv->getTimes()[i][1];
            QStringList _station_item =  {_arrival_time.toString("HH:mm"), _depart_time.toString("HH:mm"), _current_srv->getStations()[i]};
            _service_table->insertRow(_service_table->rowCount());
            for(int j{0}; j < _station_item.size(); ++j)
            {
                QTableWidgetItem* _new_time_item = new QTableWidgetItem(_station_item[j], 0);
                _new_time_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                _service_table->setItem(_service_table->rowCount()-1, j, _new_time_item);
            }
        }

        _service_table->sortItems(0);

        reset_state();
    }

    else
    {
        this->show();
    }
}

void Station_add::on_buttonBoxAddStation_rejected()
{
    this->close();
}

void Station_add::reset_state()
{
    ui->timeEditArrival->setTime(QTime(0,0));
    ui->timeEditDeparture->setTime(QTime(0,0));
    ui->checkBoxCDT->setChecked(false);
}

void Station_add::on_checkBoxCDT_stateChanged()
{
    if(ui->checkBoxCDT->isChecked())
    {
        ui->timeEditCDT->setEnabled(true);
    }
    else
    {
        ui->timeEditCDT->setEnabled(false);
    }
}
