#ifndef STATION_ADD_HXX
#define STATION_ADD_HXX

#include <QDialog>
#include <QSet>
#include <QTime>

#include "rosservice.hxx"

namespace Ui {
class Station_add;
}

enum class StopType
{
    CallingPoint,
    StartPoint,
    Terminus,
    PassingPoint
};

class Station_add : public QDialog
{
    Q_OBJECT

public:
    explicit Station_add(ROSService* service=nullptr, QWidget *parent = nullptr);
    ~Station_add();
    void setStations(QSet<QString> stations);
    bool setInfo();
    void setCurrentService(ROSService* service) {_current_srv = service;}
    QList<QTime> getTimes() const {return _times;}
    QString getStation() const {return _current_station;}
    void setType(StopType type) {_stop_class = type;}
    StopType getType() const {return _stop_class;}
    void reset_state();

private slots:
    void on_buttonBoxAddStation_accepted();

    void on_buttonBoxAddStation_rejected();

    void on_checkBoxCDT_stateChanged();

private:
    Ui::Station_add *ui;
    QList<QString> _station_list = {};
    ROSService* _current_srv;
    QString _current_station = "";
    QList<QTime> _times = {QTime(), QTime()};
    StopType _stop_class = StopType::CallingPoint;
};

#endif // STATION_ADD_HXX
