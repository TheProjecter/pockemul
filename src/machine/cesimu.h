#ifndef CESIMU_H
#define CESIMU_H

#include <QMenu>
#include <QtScript/QScriptEngine>
#include <QTextBrowser>
#include <QHBoxLayout>

#include "Connect.h"
#include "pobject.h"
#include "Inter.h"
#include "init.h"

#define TICKS_BDS	(pTIMER->pPC->getfrequency()/baudrate)
class DialogSimulator;

class Ccesimu:public CPObject{
Q_OBJECT

public:
    const char*	GetClassName(){ return("Ccesimu");};


    Cconnector	*pCONNECTOR; qint64 pCONNECTOR_value;

    virtual bool run(void);

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end

    DialogSimulator *dialogconsole;

    Q_INVOKABLE void ScriptLog(QString s);

    Ccesimu(CPObject *parent = 0);

    virtual ~Ccesimu(){
        delete(pCONNECTOR);
    };
public:
    void paintEvent(QPaintEvent *);
    QScriptEngine   *engine;
    QScriptValue    *script;
    QScriptValue    *mainfunction;

    QDialog         *helpDialog;
    QTextBrowser    *textbrowser;
    QHBoxLayout     *layout;

protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void ShowDialog(void);
    void HideDialog(void);
    void HelpDialog(void);


};


#endif // CESIMU_H
