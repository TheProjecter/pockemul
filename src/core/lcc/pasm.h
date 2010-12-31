#ifndef PASM_H
#define PASM_H

#include <QtGui>

class Cpasm:public QObject {
    Q_OBJECT
public:

    Cpasm();

    int mathparse(QByteArray s, int w);
    static const QString opcode[];
    static const unsigned char nbargu[];
    static QString replace_text(QString text, QString such, QString ers);

    void abort(QString t);

    bool findnlabel(QString l);
    void addnlabel(QString l);
    void delnlabel(int l);

    bool findlabel(QString label);
    void addlabel(QString l);

    bool findsymbol(QString l);
    void addsymbol(QString s1, QString s2);

    bool findop(QString l);
    void addcode(unsigned char b);
    void extractop(QString s);
    int calcadr(void);

    int nlabp;
    int nlabcnt;
    QList<QString> nlab;
    QList<int> nlabpos;
    QList<QString> nlabasm;


    int labcnt;

    int labp;
    QList<QString> lab;
    QList<int> labpos;

    QList<QString> sym;
    QList<QString> symval;
    int symcnt;

    int codpos;
    QByteArray code;

    int cline;
    QString cf;
    bool mcase;
    QString op;
    QString params;
    int startadr;
    QString tok;
    int opp;
    QString param1,param2;

static const QList<char> JRPLUS;
static const QList<char> JRMINUS;
static const QList<char> JR;


#endif // PASM_H
