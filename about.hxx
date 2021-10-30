#ifndef ABOUT_HXX
#define ABOUT_HXX

#include <QDialog>

const QString VERSION="0.2.1-alpha";

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();
    void setVersion(const QString& version);

private:
    Ui::About *ui;
};

#endif // ABOUT_HXX
