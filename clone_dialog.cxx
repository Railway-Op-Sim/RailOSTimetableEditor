//-------------------------------------------------------------------------//
//         ROS Timetable Editor Clone Service Application Window           //
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

#include "clone_dialog.hxx"
#include "ui_clone_dialog.h"

CloneDialog::CloneDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CloneDialog)
{
    ui->setupUi(this);
    ui->textEditCloneRef->setMaxLength(4);
}

CloneDialog::~CloneDialog()
{
    delete ui;
}

QString CloneDialog::_create_new_id(const QString& current_id)
{
    QString _new_id = _current_srv->getID();

    if(current_id[3].isNumber())
    {
        if(current_id[2].isNumber())
        {
            int _end_num = current_id.right(2).toInt()+1;
            QString _end_num_str = current_id.left(2);
            _end_num_str += (_end_num < 10) ? "0" : "";
            _end_num_str += QString::number(_end_num);
            while(_current_service_list.contains(_end_num_str))
            {
                _end_num++;
                _end_num_str = current_id.left(2);
                _end_num_str += (_end_num < 10) ? "0" : "";
                _end_num_str += QString::number(_end_num);
                if(_end_num > 99) return _new_id;
            }

            QString _as_str = (_end_num < 10) ? "0"+QString::number(_end_num) : QString::number(_end_num);
            _new_id = current_id.left(2)+_as_str;
        }

        else
        {
            int _end_num = current_id.right(1).toInt()+1;
            while(_current_service_list.contains(current_id.left(3)+QString::number(_end_num)))
            {
                _end_num++;
                if(_end_num > 9) _new_id = _current_srv->getID();
            }

            _new_id = current_id.left(3)+QString::number(_end_num);
        }

    }

    return _new_id;
}

void CloneDialog::setInitialValues()
{
    if(_current_srv->getDaughter() == "") ui->textEditCloneDaughter->setEnabled(false);
    ui->timeEditCloneStart->setTime(_current_srv->getStartTime().addSecs(3600));
    const QString _current_id = _current_srv->getID();
    ui->textEditCloneRef->setText(_create_new_id(_current_id));
    if(_current_srv->getDaughter() != "") ui->textEditCloneDaughter->setText(_create_new_id(_current_srv->getDaughter()));

}

bool CloneDialog::_check_new()
{
    if(!_current_srv || !_current_ttb)
    {
        qDebug() << "Failed to record current service/timetable selection during cloning";
        return false;
    }
    /*if(_current_srv->getParent() != "")
    {
        ui->labelCloneInfo->setText(QObject::tr("Currently cannot clone a daughter service."));
        qDebug() << "Attempt by user to clone a daughter service. Due to the fact parent services would also have to be cloned "\
                    "adding complication, this feature is not yet present.";
        QMessageBox::information(0,QObject::tr("error"), "Cloning of daughter services not yet supported due to recursion.");
        return false;
    }*/
    if(ui->timeEditCloneStart->time() == _current_srv->getStartTime())
    {
        ui->labelCloneInfo->setText(QObject::tr("New service start time cannot match old."));
        qDebug() << "Attempt by user to set clone to be at the same time as the current service.";
        return false;
    }

    if(ui->textEditCloneRef->text() == _current_srv->getID())
    {
        ui->labelCloneInfo->setText(QObject::tr("New service ID cannot match old."));
        qDebug() << "Attempt by user to set clone to be have the same ID as the current service.";
        return false;
    }

    return true;
}

void CloneDialog::on_buttonBoxClone_accepted()
{
    if(_check_new())
    {
        this->close();
        _create_clone(true);
        _ttb_table->clear();
        _ttb_table->setRowCount(0);
        for(int i{0}; i < _current_ttb->size(); ++i)
        {
            QStringList out = _current_ttb->operator[](i)->summarise();
            _ttb_table->insertRow(_ttb_table->rowCount());
            for(int j{0}; j < out.size(); ++j)
            {
                QTableWidgetItem* _new_service_item = new QTableWidgetItem(out[j], 0);
                _new_service_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                _ttb_table->setItem(_ttb_table->rowCount()-1, j, _new_service_item);
            }
        }
        return;
    }
}

void CloneDialog::on_buttonBoxClone_rejected()
{
    _cloned_srv = nullptr;
    this->close();
}

void CloneDialog::_create_clone(bool clone_others)
{
    const int _temporal_diff = _current_srv->getStartTime().secsTo(ui->timeEditCloneStart->time());
    _cloned_srv = new ROSService(*_current_srv);
    _cloned_srv->shiftServiceTimes(_temporal_diff);
    QString _srv_id = ui->textEditCloneRef->text();
    _cloned_srv->setID(_srv_id);
    QList<QString> _to_clone = {};
    if(_current_srv->getParent() != "" && clone_others)
    {
        ROSService* _main_service = _current_srv;
        QString _top_level = "";
        ROSService* srv = (*_current_ttb)[_current_srv->getParent()];
        while(srv->getParent() != "" && _to_clone.indexOf(srv->getParent()) == -1)
        {
            srv = (*_current_ttb)[srv->getParent()];
            _to_clone.push_back(srv->getID());
        }

        _to_clone = {};

        while(srv->getDaughter() != "" && _to_clone.indexOf(srv->getDaughter()) == -1)
        {
            _to_clone.push_back(srv->getID());
            srv = (*_current_ttb)[srv->getDaughter()];
            _current_srv = srv;
            _create_clone();
        }

        _current_srv = _main_service;
    }

    if(_current_srv->getDaughter() != "") _cloned_srv->setDaughter(ui->textEditCloneDaughter->text());
    if(_current_srv->getParent() != "") _cloned_srv->setParent(_create_new_id(_current_srv->getParent()));
    if(_cloned_srv->getExitTime() != _current_srv->getExitTime().addSecs(_temporal_diff))
    {
        throw std::runtime_error("Cloned exit time cannot match original.");
    }
    _current_ttb->addService(_cloned_srv);
    qDebug() << "Created Clone: " << _cloned_srv->summarise();
}
