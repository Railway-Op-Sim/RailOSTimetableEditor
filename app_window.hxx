#ifndef ROSTTBAPPWINDOW_HXX
#define ROSTTBAPPWINDOW_HXX

#include <QMainWindow>
#include <QWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include "rosttbgen.hxx"
#include "station_add.hxx"

QT_BEGIN_NAMESPACE
namespace Ui { class ROSTTBAppWindow; }
QT_END_NAMESPACE

class ROSTTBAppWindow : public QMainWindow
{
    Q_OBJECT

public:
    ROSTTBAppWindow(QWidget *parent = nullptr);
    ~ROSTTBAppWindow();
    void open_file();
    ROSTTBGen* getParser(){return _parser;}

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionSave_As_triggered();

    void on_pushButtonROSLoc_clicked();

    void on_pushButtonInsert_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonRoute_clicked();

    void on_pushButtonAddLocation_clicked();

    void on_stationAdd_accepted();

    void on_radioButtonStandard_toggled(bool checked);

    void on_radioButtonShuttleFeeder_toggled(bool checked);

    void on_radioButtonShuttleStop_toggled(bool checked);

    void on_radioButtonFeeder_toggled(bool checked);

    void on_radioButtonFromOther_toggled(bool checked);

    void on_tableWidgetTimetable_cellClicked(int row, int column);

private:
    Station_add* _station_add = new Station_add(nullptr, this);
    QDir* _ros_timetables = nullptr;
    Ui::ROSTTBAppWindow *ui;
    QItemSelectionModel* _tt_model_select;
    QFileDialog* _current_file = new QFileDialog(this);
    QString _open_file_str = "";
    QString _current_route = "";
    ROSTTBGen* _parser = new ROSTTBGen(this);
    ROSTimetable* _current_timetable = new ROSTimetable;
    QList<int> _ttb_column_widths = {50, 50, 300, 200, 75, 200, 50};
    ROSService* _current_service_selection = nullptr;
    void _save_as();
    void reset();
    bool checkROS();
    void _record_current_info();
    void _update_output();
    void delete_entries();
    void _set_initial_open_file();
    void _enable_integer_info(bool enable);

};
#endif // ROSTTBAPPWINDOW_HXX
