#include "station_add.hxx"
#include "ui_station_add.h"

Station_add::Station_add(ROSService* service, QWidget *parent) :
    _current_srv(service),
    QDialog(parent),
    ui(new Ui::Station_add)
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
    if(ui->timeEditArrival->time() < ui->timeEditDeparture->time())
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

void Station_add::on_buttonBoxAddStation_accepted()
{
    if(!setInfo())
    {
        return;
    }
    this->close();
    _current_srv->addStation({_times[0], _times[1]}, _current_station);
    _current_srv->setStopAsPassPoint(_current_srv->getStations().size()-1, ui->checkBoxPASS->isChecked());

    reset_state();
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
