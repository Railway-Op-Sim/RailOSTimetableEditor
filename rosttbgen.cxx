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
    QStringList _stations = {};
    QFile open_file(_current_route);
    if (!open_file.open(QIODevice::ReadOnly | QFile::Text))
        return "NULL";

    QTextStream in(&open_file);
    QString previous = "";
    int counter = 0;
    while (!in.atEnd()) {

        QString line = in.readLine();
        if(previous[0].isLetter() && line[0] == "*")
        {
            qDebug() << "Found Station: " << previous << "\n";
            _stations.push_back(previous);
        }
        if(counter > 0)
        {
            previous = line;
        }
        ++counter;
    }
    _stations_list = QSet<QString>(_stations.begin(), _stations.end());

    return _current_route;
}
