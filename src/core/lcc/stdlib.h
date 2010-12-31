#ifndef STDLIB_H
#define STDLIB_H

#include <QtGui>

class Cstdlib{
public:
    Cstdlib(void);

    void setModel(QString m);
    QString getModel(void) { return model;}

    void LoadLibs(void);

    QMap<QString,QByteArray> libmap;

private:
    QString model;


};

#endif // STDLIB_H
