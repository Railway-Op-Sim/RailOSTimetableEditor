#ifndef UTILITIES_HXX
#define UTILITIES_HXX

#include "QString"
#include "QStringList"

QString join(QString join_symbol, QString a, QString b, QString c, QString d, QString e, QString f, QString g, QString h);

QString join(QString join_symbol, QStringList list);

QString join(QChar::SpecialCharacter join_symbol, QStringList list);

#endif // UTILITIES_HXX
