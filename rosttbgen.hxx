#ifndef ROSTTBGEN_HXX
#define ROSTTBGEN_HXX

#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QSet>
#include <QRegularExpression>

#include <map>
#include <vector>

#include "rostimetable.hxx"
#include "rosservice.hxx"


class ROSTTBGen
{
    private:
        QString _cached_text = "";
        QSet<QString> _stations_list= {};
        ROSTimetable* _current_timetable = new ROSTimetable;
        QStringList _input_data = {};
        QStringList _comments = {};
        QWidget* _parent;
        QString _special_char;
        void _process_data();
        bool _check_isTime(QString string);
        bool _check_isID(QString string);
        bool _check_Integer(QString string);
        bool _checkPas(QStringList str_list);
        bool _checkCDT(QStringList str_list);
        bool _checkStationCall(QStringList str_list);
        bool _checkIfStart(QStringList str_list);
        bool _checkIfEnd(QStringList str_list);
        bool _checkIsCoordinate(QString string);
        bool _isServiceDefinition(QStringList str_list);
        bool _isContinuedServiceDefinition(QStringList str_list);
        bool _isStartTypeDefinition(QStringList str_list);
        bool _isCallingPoint(QStringList str_list);
        bool _isStartStopPoint(QStringList str_list);
        bool _isDirChange(QStringList str_list);
        bool _isRepeat(QStringList str_list);
        bool _isSplit(QStringList str_list);
        void _process_service_candidate(int int_id, QStringList service);
        ROSService::ServiceType _parseType(QStringList str_list);
        ROSService::FinishState _parseExit(QStringList str_list);
        QString _make_service_definition(ROSService* service);
        QString _make_type_line(ROSService* service);
        QStringList _add_stations(ROSService* service);
        QString _make_service_termination(ROSService* service);
        QString _make_repeat_line(ROSService* service);
        int _calculate_repeat_time_interval(ROSService* service);
        QMap<ROSService::FinishState, QString> _exit_types = {{ROSService::FinishState::FinishExit, "Fer"},
                                                              {ROSService::FinishState::FinishFormNew, "Fns"},
                                                              {ROSService::FinishState::FinishJoinOther, "Fjo"},
                                                              {ROSService::FinishState::FinishRemainHere, "Frh"},
                                                              {ROSService::FinishState::FinishShuttleRemainHere, "Frh-sh"},
                                                              {ROSService::FinishState::FinishShuttleFormNew, "Fns-sh"},
                                                              {ROSService::FinishState::FinishSingleShuttleFeeder, "F-nshs"}};
        QMap<ROSService::ServiceType, QString> _start_types = {{ROSService::ServiceType::Service, "Snt"},
                                                               {ROSService::ServiceType::ShuttleFromStop, "Snt-sh"},
                                                               {ROSService::ServiceType::ShuttleFromFeeder, "Sns-sh"},
                                                               {ROSService::ServiceType::ServiceFromSplit, "Sfs"},
                                                               {ROSService::ServiceType::ShuttleFinishService, "Sns-fsh"},
                                                               {ROSService::ServiceType::ServiceFromService, "Sns"}};
    public:
        ROSTTBGen(QWidget* parent) : _parent(parent) {}
        QSet<QString> getStations(){return _stations_list;}
        QString getSpecialChar() const {return _special_char;}
        QString parse_file(const QFileDialog* dialog, const QDir* directory);
        QString parse_rly(const QString railways_dir);
        ROSTimetable* getParsedTimetable() { return _current_timetable; }
        QString getFinishID(ROSService::FinishState state) {return _exit_types[state];}
        QString getStartID(ROSService::ServiceType state) {return _start_types[state];}
        QStringList createTimetableStrings(ROSTimetable* timetable);
};

#endif // ROSTTBGEN_HXX
