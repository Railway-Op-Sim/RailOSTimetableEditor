#ifndef ROSTTBGEN_HXX
#define ROSTTBGEN_HXX

#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>

#include <map>
#include <vector>

#include "rostimetable.hxx"


class ROSTTBGen
{
    private:
        QString _cached_text = "";
    public:
        ROSTTBGen();
        void parse_file(const QFileDialog* dialog);
        QString parse_rly(QWidget* parent, const QString railways_dir);

};

#endif // ROSTTBGEN_HXX
