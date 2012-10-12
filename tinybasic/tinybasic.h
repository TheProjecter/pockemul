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

    enum Mode{RUN,COMMAND };

       CTinyBasic(QObject *parent = 0);

       void Interpret(QByteArray, int pos = 0);
       void Command(QByteArray);
       void Parse();

       QByteArray commandBuffer;
       QMap<QByteArray,CKeyword> CommandMap;

       QMap<int,QByteArray> basicLines;

       Mode mode;
       void saveBasicLine();
       void test();
       void command(QByteArray code);
       void go_LIST(QByteArray code);
       void go_RUN(QByteArray code);
};

#endif // TINYBASIC_H
