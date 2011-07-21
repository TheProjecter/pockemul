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
    const char*	GetClassName(){ return("Ccesimu");}


    Cconnector	*pCONNECTOR; qint64 pCONNECTOR_value;
    Cconnector  *pSavedCONNECTOR;

    virtual bool run(void);

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end

    DialogSimulator *dialogconsole;

    Q_INVOKABLE void ScriptLog(QString s);
    Q_INVOKABLE bool GoDown(int pin);
    Q_INVOKABLE bool GoUp(int pin);
    Q_INVOKABLE bool Change(int pin);
    Q_INVOKABLE void timerInit(int id);
    Q_INVOKABLE int timerMsElapsed(int id);
    Q_INVOKABLE int timerUsElapsed(int id);
    Q_INVOKABLE void setMarker(int markId);

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

    qint64  states[20];

protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void ShowDialog(void);
    void HideDialog(void);
    void HelpDialog(void);

private:
    qint64  run_oldstate;
    qint64  latency;

};


#endif // CESIMU_H
