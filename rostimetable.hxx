#ifndef ROSTIMETABLE_HXX
#define ROSTIMETABLE_HXX

#include <QMap>
#include <QDateTime>
#include <QSet>
#include <QDebug>

#include "rosservice.hxx"

class ROSTimetable
{
    private:
        QTime _start_time = QTime();
        QMap<QString, ROSService*> _services;
        QList<QString> _stored_locations = {};
        QString _file_name = "";
    public:
        ROSTimetable(QString file_name="NewTimetable_"+QDateTime::currentDateTime().toString("HH_mm_dd-MM-yy")+".ttb") : _file_name(file_name) {}
        void addService(int int_id, QTime start_time, QString id, QString description, QList<QString> stations = {}, QList<QList<QTime>> times = {},
                        int start_speed=-1, int max_speed=-1, int mass=-1, int max_brake_force=-1, int power=-1)
        {
            _services[id] = new ROSService(int_id, start_time, id, description, stations, times, start_speed, max_speed, mass, max_brake_force, power);
        }
        void addService(ROSService* service)
        {
            _services[service->getID()] = service;
        }
        QMap<QString, ROSService*> getServices() const {return _services;}
        void orderServices();
        QString getFileName() const {return _file_name;}
        void setFileName(QString file_name) {_file_name = file_name;}
        ROSService* operator[](const QString&);
        ROSService* operator[](const int&);
        ROSService* begin() {return this->operator[](0);}
        ROSService* end() {return this->operator[](_services.size()-1);}
        QTime getStartTime() const {return _start_time;}
        int size() const {return _services.size();}
        void setStartTime(QTime& time) {_start_time = time;}
};

#endif // ROSTIMETABLE_HXX
