

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

Cmd100::~Cmd100() {
    delete pCONNECTOR;
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
    CPObject::exit();
    return true;
}


bool Cmd100::Get_Connector(void) {


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

BYTE Cmd100::SwitchCmd(BYTE x) {}

BYTE Cmd100::ExecDir(BYTE x) {}

BYTE Cmd100::ReturnCountHi(BYTE x) {
    Q_UNUSED(x);
    index++;
    return (count>>8)&0xff;
}

BYTE Cmd100::ReturnCountLo(BYTE x) {
    Q_UNUSED(x);
    index++;
    bufindex = 0;
    return (count&0xff);
}

BYTE Cmd100::ReturnBlock(BYTE x) {
    Q_UNUSED(x);
    BYTE ret=buffer[bufindex];
    if (bufindex < (BUFSIZE - 1)) bufindex++;
    count--;
    if (count <= 0) index++;
    return ret;
}

BYTE Cmd100::ExecCloseFile(BYTE x) {}
BYTE Cmd100::AcceptCountLo(BYTE x) {}
BYTE Cmd100::AcceptCountHi(BYTE x) {}
BYTE Cmd100::AcceptBlock(BYTE x) {}
BYTE Cmd100::ExecOpenFile(BYTE x) {}		// + return the number of data bytes, LSB }
BYTE Cmd100::ExecReadFile(BYTE x) {}		// + return the number of data bytes, LSB }
BYTE Cmd100::ExecReadSector(BYTE x) {}	// + get the sector number }
BYTE Cmd100::ExecKillFile(BYTE x) {}		// + return the number of data bytes, LSB }
BYTE Cmd100::ExecRenameFile(BYTE x) {}	// + return the number of data bytes, LSB }
BYTE Cmd100::ExecWriteSector(BYTE x) {}
BYTE Cmd100::ExecWriteFile(BYTE x) {}
BYTE Cmd100::ExecGetSize(BYTE x) {}

const Cmd100::funcPtr Cmd100::cmdtab[55] = {
// index 0: entry point }
&Cmd100::SwitchCmd,
// index 1: read directory entry }
&Cmd100::ExecDir,		// + return the number of data bytes, LSB }
&Cmd100::ReturnCountHi,
&Cmd100::ReturnBlock,
&Cmd100::SwitchCmd,
// index 5: close file }
&Cmd100::ExecCloseFile,
&Cmd100::SwitchCmd,
// index 7: open file }
&Cmd100::AcceptCountLo,
&Cmd100::AcceptCountHi,
&Cmd100::AcceptBlock,
&Cmd100::ExecOpenFile,		// + return the number of data bytes, LSB }
&Cmd100::ReturnCountHi,
&Cmd100::ReturnBlock,
&Cmd100::SwitchCmd,
// index 14: read from file }
&Cmd100::AcceptCountLo,
&Cmd100::AcceptCountHi,
&Cmd100::AcceptBlock,
&Cmd100::ExecReadFile,		// + return the number of data bytes, LSB }
&Cmd100::ReturnCountHi,
&Cmd100::ReturnBlock,
&Cmd100::SwitchCmd,
// index 21: read sector }
&Cmd100::AcceptCountLo,		// get the track number }
&Cmd100::ExecReadSector,	// + get the sector number }
&Cmd100::ReturnBlock,
&Cmd100::SwitchCmd,
// index 25: delete file }
&Cmd100::AcceptCountLo,
&Cmd100::AcceptCountHi,
&Cmd100::AcceptBlock,
&Cmd100::ExecKillFile,		// + return the number of data bytes, LSB }
&Cmd100::ReturnCountHi,
&Cmd100::ReturnBlock,
&Cmd100::SwitchCmd,
// index 32: rename file }
&Cmd100::AcceptCountLo,
&Cmd100::AcceptCountHi,
&Cmd100::AcceptBlock,
&Cmd100::ExecRenameFile,	// + return the number of data bytes, LSB }
&Cmd100::ReturnCountHi,
&Cmd100::ReturnBlock,
&Cmd100::SwitchCmd,
// index 39: write sector }
&Cmd100::AcceptCountLo,
&Cmd100::AcceptCountHi,
&Cmd100::ExecWriteSector,
&Cmd100::SwitchCmd,
// index 43: write to file }
&Cmd100::AcceptCountLo,
&Cmd100::AcceptCountHi,
&Cmd100::ExecWriteFile,
&Cmd100::ReturnCountLo,
&Cmd100::ReturnCountHi,
&Cmd100::ReturnBlock,
&Cmd100::SwitchCmd,
// index 50: get file size }
&Cmd100::AcceptCountLo,		// get the file handle }
&Cmd100::ExecGetSize,
&Cmd100::ReturnCountLo,
&Cmd100::ReturnCountHi,
&Cmd100::SwitchCmd	};


