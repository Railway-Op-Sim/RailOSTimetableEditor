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

private:
    QTableWidget* _service_table = new QTableWidget(this);
    QTableWidget* _timetable_table = new QTableWidget(this);
    Station_add* _station_add = new Station_add(nullptr, this);
    QItemSelectionModel* _tt_model_select = new QItemSelectionModel(_timetable_table->model());
    QDir* _ros_timetables = nullptr;
    Ui::ROSTTBAppWindow *ui;
    QFileDialog* _current_file = new QFileDialog(this);
    QString _open_file_str = "";
    QString _current_route = "";
    ROSTTBGen* _parser = new ROSTTBGen;
    ROSTimetable* _current_timetable = new ROSTimetable;
    int _current_service_selection = -1;
    void _save_as();
    void reset();
    bool checkROS();
    void _record_current_info();
    void _update_output();
    void delete_entries();

};
#endif // ROSTTBAPPWINDOW_HXX
