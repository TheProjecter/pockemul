#ifndef TINYBASIC_H
#define TINYBASIC_H

#include <QObject>
#include <QByteArray>
#include <QMap>

class CKeyword {
public:
    CKeyword(QString name = QString(),quint8 code = 0);

    QString Name;
    quint8  Code;
};

class CTinyBasic : public QObject
{
    Q_OBJECT
public:
       CTinyBasic(QObject *parent = 0);

       void Interpret(QByteArray, int pos);
       void Command(QByteArray);
       void Parse();

       QByteArray commandBuffer;
       QMap<QByteArray,CKeyword> CommandMap;
};

#endif // TINYBASIC_H
