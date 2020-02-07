#include "rosttbgen.hxx"

ROSTTBGen::ROSTTBGen()
{
}

void ROSTTBGen::parse_file(const QFileDialog* file)
{
    _cached_text = "";
    QFile open_file(file->getOpenFileName());
    if (!open_file.open(QIODevice::ReadOnly | QFile::Text))
        return;

    QTextStream in(&open_file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        _cached_text += line;
    }

    qDebug() << _cached_text << "\n";

}

QString ROSTTBGen::parse_rly(QWidget* parent, const QString railways_dir)
{
    QFileDialog* _route_diag = new QFileDialog;
    QString _current_route = _route_diag->getOpenFileName(parent, "Open Route", railways_dir,
                                   "ROS Railway Files (*.rly)");
    _cached_text = "";
    QFile open_file(_current_route);
    if (!open_file.open(QIODevice::ReadOnly | QFile::Text))
        return "NULL";

    QTextStream in(&open_file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        _cached_text += line;
    }

    return _current_route;
}
