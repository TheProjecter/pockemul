

#include <QPainter>
#include <QTime>
#include <QSound>
#include <QFileDialog>
#include <QThread>
#include <QPixmap>
#include <QBitmap>

#include "common.h"

#include "md100.h"
#include "Connect.h"
#include "Inter.h"
#include "init.h"
#include "Log.h"


Cmd100::Cmd100(CPObject *parent):CPObject(parent)
{								//[constructor]
    setfrequency( 0);

    BackGroundFname	= ":/EXT/ext/md-100.png";
    setcfgfname("md100");


    pCONNECTOR	   = new Cconnector(this,30,0,Cconnector::Casio_30,"Connector 30 pins",true,QPoint(666,540));	publish(pCONNECTOR);
//    pCONNECTOR_Ext = new Cconnector(this,11,1,Cconnector::Sharp_11,"Connector 11 pins Ext.",false,QPoint(6,295));	publish(pCONNECTOR_Ext);

    pTIMER		= new Ctimer(this);

    setDX(1203);//Pc_DX	= 620;//480;
    setDY(779);//Pc_DY	= 488;//420;

    setDXmm(337);
    setDYmm(218);
    setDZmm(46);


}

bool Cmd100::UpdateFinalImage(void) {
    CPObject::UpdateFinalImage();

    return true;

}







bool Cmd100::init(void)
{
    CPObject::init();

    setfrequency( 0);

    WatchPoint.add(&pCONNECTOR_value,64,30,this,"Standard 30pins connector");
//    WatchPoint.add(&pCONNECTOR_Ext_value,64,11,this,"Ext 11pins connector");


    AddLog(LOG_PRINTER,tr("MD-100 initializing..."));

    if(pTIMER)	pTIMER->init();

    return true;
}

void Cmd100::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    BuildContextMenu(&menu);

    menu.addSeparator();

    menu.addAction(tr("Define Directory"),this,SLOT(definePath()));

    menu.exec(event->globalPos () );
}

void Cmd100::definePath(void){
    MSG_ERROR("test")
    QString path = QFileDialog::getExistingDirectory (this, tr("Directory"));
    if ( path.isNull() == false )
    {
        directory.setPath(path);
    }
}

/*****************************************************/
/* Exit PRINTER										 */
/*****************************************************/
bool Cmd100::exit(void)
{
    AddLog(LOG_PRINTER,"MD-100 Closing...");
    AddLog(LOG_PRINTER,"done.");
    CPObject::exit();//Cprinter::exit();
    return true;
}


bool Cmd100::Get_Connector(void) {
//    MT_OUT2	= GET_PIN(PIN_MT_OUT2);
//    BUSY    = GET_PIN(PIN_BUSY);
//    D_OUT	= GET_PIN(PIN_D_OUT);
//    MT_IN	= GET_PIN(PIN_MT_IN);
//    MT_OUT1	= GET_PIN(PIN_MT_OUT1);
//    D_IN	= GET_PIN(PIN_D_IN);
//    //ACK		= GET_PIN(PIN_ACK);
//    SEL2	= GET_PIN(PIN_SEL2);
//    SEL1	= GET_PIN(PIN_SEL1);

    return true;
}

bool Cmd100::Set_Connector(void) {



    return true;
}


bool Cmd100::run(void)
{

    Get_Connector();

    pCONNECTOR_value = pCONNECTOR->Get_values();


    Set_Connector();
    pCONNECTOR_value = pCONNECTOR->Get_values();

    return true;
}

