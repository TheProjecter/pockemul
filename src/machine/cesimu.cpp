#include "common.h"
#include "Log.h"
#include "cesimu.h"
#include "dialogsimulator.h"

Ccesimu::Ccesimu(CPObject *parent): CPObject(this)
{							//[constructor]

    //ToDestroy = false;

    pCONNECTOR = new Cconnector(this,11,0,"Connector 11 pins",true,QPoint(130,7)); publish(pCONNECTOR);
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

    menu.exec(event->globalPos () );
}

void Ccesimu::ShowDialog(void) {
    dialogconsole->show();
}
void Ccesimu::HideDialog(void) {
    dialogconsole->hide();
}

bool Ccesimu::init(void){
    dialogconsole = new DialogSimulator(this);
    dialogconsole->show();
}

bool Ccesimu::exit(void){

}
bool Ccesimu::run(void){

}

void Ccesimu::paintEvent(QPaintEvent *event)
{
    CPObject::paintEvent(event);
    //dialogconsole->refresh();
}
