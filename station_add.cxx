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
    ui->timeEditArrival->setTime(_times[0]);
    ui->timeEditDeparture->setTime(_times[0]);
    ui->timeEditCDT->setTime(_times[0]);
    ui->timeEditSplit->setTime(_times[0]);
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
    if(!_edit_mode && ui->comboBoxStations->currentText() != _current_station)
    {
        for(auto time_pair : _current_srv->getTimes())
        {
            if((_times[0] >= time_pair[0] && _times[0] <= time_pair[1]) || (_times[1] >= time_pair[0] && _times[1] <= time_pair[1]))
            {
                ui->errorLabel->setText("Times overlap with current timetable entries");
                return false;
            }
        }
    }
    if(ui->timeEditArrival->time().msecsTo(ui->timeEditDeparture->time()) < 0)
    {
        ui->errorLabel->setText("Departure time cannot be before arrival.");
        return false;
    }
    ui->errorLabel->setText("");

    _times[0] = ui->timeEditArrival->time();
    _times[1] = ui->timeEditDeparture->time();

    if(ui->checkBoxCDT->isChecked())
    {
        if(ui->timeEditCDT->time().msecsTo(ui->timeEditArrival->time()) > 0)
        {
            ui->errorLabel->setText("Direction Change Time cannot proceed arrival time");
            return false;
        }

    }

    return true;
}

void Station_add::setEditMode(bool on){ui->pushButtonDeleteEntry->setVisible(on); _edit_mode = on;}

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
    _times = {ui->timeEditArrival->time(), ui->timeEditDeparture->time()};

    if(setInfo())
    {
        this->close();

        QTime _cdt_time = (ui->checkBoxCDT->isChecked()) ? ui->timeEditCDT->time() : QTime();

        if(_current_srv->getStations().contains(ui->comboBoxStations->currentText()) || (_current_srv->getStations().size() > 0 && _times == _current_srv->getTimes()[_current_srv->getStations().indexOf(_current_station)]))
        {
            QString _new_station = ui->comboBoxStations->currentText();
            _current_srv->updateStation(_current_station, _times, ui->checkBoxCDT->isChecked(), ui->checkBoxPASS->isChecked(), _cdt_time, _new_station);
            _current_station = _new_station;

            this->close();

            reset_state();

        }

        else
        {
            _current_station = ui->comboBoxStations->currentText();
            _current_srv->addStation(_times, _current_station);
            _current_srv->setStopAsPassPoint(_current_srv->getStations().size()-1, ui->checkBoxPASS->isChecked());
            _current_srv->setDirectionChangeAtStop(_current_srv->getStations().size()-1, ui->checkBoxCDT->isChecked(), _cdt_time);
        }

        _redraw_table();

        if(ui->checkBoxSplit->isChecked())
        {
            _current_srv->setSplitData((ui->radioButtonSplitForward->isChecked()) ? QString("F") : QString("R"), _current_srv->getID(), _current_station, ui->timeEditSplit->time().toString("HH:mm"));
        }

        else if(ui->checkBoxJoinOther->isChecked())
        {
            _current_srv->setJoinData(ui->textEditSplitRef->text(), ui->comboBoxStations->currentText(), ui->timeEditDeparture->time().toString("HH:mm"));
        }


        // Ensure the exit time is always beyond the latest station addition
        if(_current_srv->getExitTime() == QTime() || ui->timeEditArrival->time().secsTo(_current_srv->getExitTime()) < 0) _current_srv->setExitTime(ui->timeEditArrival->time());

        _service_table->sortItems(0);

        reset_state();
    }

    else
    {
        this->show();
    }
}

void Station_add::_redraw_table()
{
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
}

void Station_add::on_buttonBoxAddStation_rejected()
{
    this->close();
}

void Station_add::reset_state()
{
    _edit_mode = false;
    ui->pushButtonDeleteEntry->setVisible(false);
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

void Station_add::on_checkBoxSplit_toggled(bool checked)
{
    ui->checkBoxPASS->setEnabled(!checked);
    ui->checkBoxJoinOther->setEnabled(!checked);
    ui->timeEditSplit->setEnabled(checked);
    ui->textEditSplitRef->setEnabled(checked);
    ui->radioButtonSplitForward->setEnabled(checked);
    ui->radioButtonSplitReverse->setEnabled(checked);
}

void Station_add::on_checkBoxJoinOther_toggled(bool checked)
{
    ui->checkBoxPASS->setEnabled(!checked);
    ui->checkBoxSplit->setEnabled(!checked);
    ui->timeEditSplit->setEnabled(!checked);
    ui->textEditSplitRef->setEnabled(checked);
    ui->radioButtonSplitForward->setEnabled(!checked);
    ui->radioButtonSplitReverse->setEnabled(!checked);
}

void Station_add::on_checkBoxPASS_toggled(bool checked)
{
    ui->checkBoxJoinOther->setEnabled(!checked);
    ui->checkBoxSplit->setEnabled(!checked);
    ui->timeEditSplit->setEnabled(!checked);
    ui->textEditSplitRef->setEnabled(!checked);
    ui->radioButtonSplitForward->setEnabled(!checked);
    ui->radioButtonSplitReverse->setEnabled(!checked);
}


void Station_add::on_pushButtonDeleteEntry_clicked()
{
    _current_srv->deleteEntry(ui->comboBoxStations->currentText());
    reset_state();
    _redraw_table();
    this->close();
}
