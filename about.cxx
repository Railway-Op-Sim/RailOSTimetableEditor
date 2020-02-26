#include "about.hxx"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
}

About::~About()
{
    delete ui;
}

void About::setVersion(const QString &version)
{
    ui->labelVersion->setText(version);
}
