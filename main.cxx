#include "app_window.hxx"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ROSTTBAppWindow w;
    w.show();
    return a.exec();
}
