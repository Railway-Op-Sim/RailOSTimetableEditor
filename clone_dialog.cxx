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

void CloneDialog::setInitialValues()
{
    ui->timeEditCloneStart->setTime(_current_srv->getStartTime().addSecs(3600));
    const QString _current_id = _current_srv->getID();
    QString _new_id = "";
    if(_current_id[3].isNumber())
    {
        if(_current_id[2].isNumber())
        {
            int _end_num = _current_id.right(2).toInt()+1;
            while(_current_service_list.contains(_current_id.left(2)+QString::number(_end_num)))
            {
                _end_num++;
                if(_end_num > 99) _new_id = _current_srv->getID();
            }

            QString _as_str = (_end_num < 10) ? "0"+QString::number(_end_num) : QString::number(_end_num);
            _new_id = _current_id.left(2)+_as_str;
        }

        else
        {
            int _end_num = _current_id.right(1).toInt()+1;
            while(_current_service_list.contains(_current_id.left(3)+QString::number(_end_num)))
            {
                _end_num++;
                if(_end_num > 9) _new_id = _current_srv->getID();
            }

            _new_id = _current_id.left(3)+QString::number(_end_num);
        }

        ui->textEditCloneRef->setText(_new_id);
    }
}

bool CloneDialog::_check_new()
{
    if(!_current_srv || !_current_ttb)
    {
        qDebug() << "Failed to record current service/timetable selection during cloning";
        return false;
    }
    if(_current_srv->getParent() != "")
    {
        ui->labelCloneInfo->setText("Currently cannot clone a daughter service.");
        qDebug() << "Attempt by user to clone a daughter service. Due to the fact parent services would also have to be cloned "\
                    "adding complication, this feature is not yet present.";
        return false;
    }
    if(ui->timeEditCloneStart->time() == _current_srv->getStartTime())
    {
        ui->labelCloneInfo->setText("New service start time cannot match old.");
        qDebug() << "Attempt by user to set clone to be at the same time as the current service.";
        return false;
    }

    if(ui->textEditCloneRef->text() == _current_srv->getID())
    {
        ui->labelCloneInfo->setText("New service ID cannot match old.");
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
        _create_clone();
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

void CloneDialog::_create_clone()
{
    const int _temporal_diff = _current_srv->getStartTime().secsTo(ui->timeEditCloneStart->time());
    _cloned_srv = new ROSService(*_current_srv);
    _cloned_srv->shiftServiceTimes(_temporal_diff);
    QString _srv_id = ui->textEditCloneRef->text();
    _cloned_srv->setID(_srv_id);
    _current_ttb->addService(_cloned_srv);
    qDebug() << "Created Clone: " << _cloned_srv->summarise();
}
