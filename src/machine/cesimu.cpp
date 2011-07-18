#include <QTextBrowser>

#include "common.h"
#include "Log.h"
#include "cesimu.h"
#include "dialogsimulator.h"
#include "dialoganalog.h"

#define DOWN	0
#define UP		1

Ccesimu::Ccesimu(CPObject *parent): CPObject(this)
{							//[constructor]

    //ToDestroy = false;

    pCONNECTOR = new Cconnector(this,11,0,"Connector 11 pins",true,QPoint(130,7)); publish(pCONNECTOR);
    pSavedCONNECTOR = new Cconnector(this,11,0,"Saved Connector 11 pins",true,QPoint(130,7));
    setfrequency( 0);
    BackGroundFname	= ":/EXT/ext/simu.png";

    pTIMER		= new Ctimer(this);
    Pc_DX	= 160;
    Pc_DY	= 160;
//		Pc_DX	= 85;
//		Pc_DY	= 173;
//		SnapPts = QPoint(672,150);
    stackBehind = true;
};

void Ccesimu::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    BuildContextMenu(&menu);

    menu.addSeparator();

    menu.addAction(tr("Show console"),this,SLOT(ShowDialog()));
    menu.addAction(tr("Hide console"),this,SLOT(HideDialog()));
    menu.addAction(tr("Help"),this,SLOT(HelpDialog()));


    menu.exec(event->globalPos () );
}

void Ccesimu::ShowDialog(void) {
    dialogconsole->show();
}
void Ccesimu::HideDialog(void) {
    dialogconsole->hide();
}
void Ccesimu::HelpDialog(void) {

    helpDialog = new QDialog(this);
    textbrowser = new QTextBrowser();
    layout = new QHBoxLayout;
    layout->addWidget(textbrowser);
    helpDialog->setLayout(layout);
    helpDialog->setWindowTitle(tr("Script Help"));
    textbrowser->setSource(QUrl("qrc:/POCKEMUL/pockemul/script_help.html"));
    helpDialog->show();
}

bool Ccesimu::init(void){
    WatchPoint.add(&pCONNECTOR_value,64,15,this,"Connector 11 pins");
    dialogconsole = new DialogSimulator(this);
    dialogconsole->show();
    engine = new QScriptEngine(this);
    QScriptValue objectValue = engine->newQObject(pCONNECTOR);
    engine->globalObject().setProperty("Connector", objectValue);

    objectValue = engine->newQObject(this);
    engine->globalObject().setProperty("Simulator", objectValue);
    mainfunction = 0;
    run_oldstate = -1;
    for (int i=0;i<20;i++) states[i]=0;

    return true;
}

bool Ccesimu::exit(void){
    return true;
}
bool Ccesimu::run(void){


// Try to introduce a latency
    qint64			deltastate = 0;

    if (run_oldstate == -1) run_oldstate = pTIMER->state;
    deltastate = pTIMER->state - run_oldstate;
//    if (deltastate < CESIMULATENCY ) return true;
//    run_oldstate	= pTIMER->state;
    if (pTIMER->usElapsed(run_oldstate)<1000) return true;
    run_oldstate	= pTIMER->state;

    if (mainfunction) {
    QString s = mainfunction->call(QScriptValue()).toString();
    }

    pSavedCONNECTOR->Set_values(pCONNECTOR->Get_values());
    pCONNECTOR_value = pCONNECTOR->Get_values();
}

void Ccesimu::paintEvent(QPaintEvent *event)
{
    CPObject::paintEvent(event);
    //dialogconsole->refresh();
}

void Ccesimu::ScriptLog(QString s) {
    AddLog(LOG_SIMULATOR,s)
}

bool Ccesimu::GoDown(int pin) {

    return (( pCONNECTOR->Get_pin(pin) == DOWN ) && (pSavedCONNECTOR->Get_pin(pin) == UP)) ? true:false;
}
bool Ccesimu::GoUp(int pin) {

    return (( pCONNECTOR->Get_pin(pin) == UP ) && (pSavedCONNECTOR->Get_pin(pin) == DOWN)) ? true:false;
}
bool Ccesimu::Change(int pin) {
    return (pCONNECTOR->Get_pin(pin) != pSavedCONNECTOR->Get_pin(pin) ) ? true:false;
}

void Ccesimu::timerInit(int id) {
    states[id] = pTIMER->state;
}

int Ccesimu::timerMsElapsed(int id) {
    return pTIMER->msElapsed(states[id]);
}
int Ccesimu::timerUsElapsed(int id) {
    return pTIMER->msElapsed(states[id]);
}
void Ccesimu::setMarker(int markId){
    if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = markId;
}
