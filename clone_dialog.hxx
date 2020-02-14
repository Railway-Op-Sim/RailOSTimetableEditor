#ifndef CLONE_DIALOG_HXX
#define CLONE_DIALOG_HXX

#include <QDialog>
#include <QTableWidget>

#include "rosservice.hxx"
#include "rostimetable.hxx"

namespace Ui {
class CloneDialog;
}

class CloneDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CloneDialog(QWidget *parent = nullptr);
    ~CloneDialog();
    void fwdCurrentService(ROSTimetable* ttb, ROSService* service, QTableWidget* ttb_table){_current_ttb = ttb; _ttb_table = ttb_table; _current_srv = service;}
    void fwdServicesList(QStringList& services){_current_service_list = services;}
    ROSService* getNewService() const {return _cloned_srv;}
    void setInitialValues();
private slots:
    void on_buttonBoxClone_accepted();
    void on_buttonBoxClone_rejected();

private:
    Ui::CloneDialog *ui;
    ROSTimetable* _current_ttb = nullptr;
    QTableWidget* _ttb_table = nullptr;
    ROSService* _current_srv = nullptr;
    ROSService* _cloned_srv = nullptr;
    QStringList _current_service_list = {};
    bool _check_new();
    void _create_clone();
};

#endif // CLONE_DIALOG_HXX
