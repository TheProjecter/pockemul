#ifndef LCPP_H
#define LCPP_H

#include <QtGui>

#include "mainwindowpockemul.h"
extern MainWindowPockemul *mainwindow;


class Clcpp:public QObject {
    Q_OBJECT
public:

    Clcpp(QMap<QString,QByteArray> *sources,QMap<QString,QByteArray> *out) {
        this->sources = sources;
        this->out = out;
        symcnt = 0;
    }

    void run();

void abort(QString t);
QString replace_text(QString text, QString such, QString ers);
bool findsymbol(QString l);
void addsymbol(QString s1, QString s2);
QString extractparam(QString s,int p) ;
QString readline(QStringListIterator *linesIter);
QString parsefile(QString srcName,QString source);
void writeln(QString srcName,QString s);

    QMap<QString,QByteArray> *sources;
    QMap<QString,QByteArray> *out;

    int cline;
    int lcnt;
    QString cf;
    bool lcom;

    QList<QString> sym;
    QList<QString> symval;
    int symcnt;

};

#endif // LCPP_H
