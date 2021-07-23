#include "utilities.hxx"

QString join(QString join_symbol, QString a, QString b, QString c, QString d, QString e, QString f, QString g, QString h)
{
    if(a == "") return b;
    if(b == "") return "";
    QString _temp = a+join_symbol+b;

    if(c != "NULL") _temp += join_symbol+c;
    if(d != "NULL") _temp += join_symbol+d;
    if(e != "NULL") _temp += join_symbol+e;
    if(f != "NULL") _temp += join_symbol+f;
    if(g != "NULL") _temp += join_symbol+g;
    if(h != "NULL") _temp += join_symbol+h;

    return _temp;
}

QString join(QString join_symbol, QStringList list)
{
    QString _temp = list[0];

    for(int i{1}; i < list.size(); ++i) _temp += join_symbol+list[i];

    return _temp;
}

QString join(QChar::SpecialCharacter join_symbol, QStringList list)
{
    QString _temp = list[0];

    for(int i{1}; i < list.size(); ++i) _temp += join_symbol+list[i];

    return _temp;
}
