#ifndef ROSTIMETABLE_HXX
#define ROSTIMETABLE_HXX

#include <map>
#include <vector>
#include <QDateTime>

#include "rosservice.hxx"

class ROSTimetable
{
    private:
        std::map<QString, ROSService*> _services = {};
        std::vector<QString> _stored_locations = {};
        QString _file_name = "";
    public:
        ROSTimetable(QString file_name="NewTimetable_"+QDateTime::currentDateTime().toString("HH_mm_dd-MM-yy")+".ttb") : _file_name(file_name) {}
        void addService(QTime start_time, QString id)
        {
            _services[id] = new ROSService(start_time, id);
        }
        QString getFileName() const {return _file_name;}
        void setFileName(QString file_name) {_file_name = file_name;}
        ROSService* operator[](const QString&);
        ROSService* operator[](const int&);
        ROSService* begin() {return this->operator[](0);}
        ROSService* end() {return this->operator[](this->_services.size()-1);}
        int size() const {return _services.size();}
};

#endif // ROSTIMETABLE_HXX
