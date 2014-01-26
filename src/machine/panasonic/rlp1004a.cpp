#include <QtGui>
#include <QTime>
#include <QFileDialog>
#include <QThread>
#include <QDebug>

#include "common.h"

#include "rlp1004a.h"
#include "rlh1000.h"
#include "pcxxxx.h"
#include "Log.h"
#include "dialoganalog.h"
#include "paperwidget.h"
#include "Keyb.h"
#include "Connect.h"
#include "bus.h"

#define DOWN    0
#define UP              1

TransMap KeyMaprlp1004a[]={
    {1, "FEED  ",       K_PFEED,34,234, 9},
    {2, "POWER ON",     K_POW_ON,34,234,        9},
    {3, "POWER OFF",K_POW_OFF,34,234,   9}
};
int KeyMaprlp1004aLenght = 3;

Crlp1004a::Crlp1004a(CPObject *parent):Cprinter(this)
{                                                               //[constructor]
    setfrequency( 0);
    paperbuf    = 0;
    paperdisplay= 0;
    //bells             = 0;
    charTable = 0;
    margin = 25;
    BackGroundFname     = P_RES(":/rlh1000/rlp1004a.png");
    setcfgfname("rlp1004a");

    settop(10);
    setposX(0);

    pTIMER              = new Ctimer(this);
    KeyMap      = KeyMaprlp1004a;
    KeyMapLenght= KeyMaprlp1004aLenght;
    pKEYB               = new Ckeyb(this,"rlp1004a.map");
    setDXmm(113);
    setDYmm(95);
    setDZmm(51);
 // Ratio = 3,57
    setDX(440);//Pc_DX  = 75;
    setDY(340);//Pc_DY  = 20;

    setPaperPos(QRect(53,0,291,216));

    ctrl_char = false;
    t = 0;
    c = 0;
    rmtSwitch = false;

    rotate = false;
    INTrequest = false;

    memsize             = 0x2000;
    InitMemValue        = 0x7f;
    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,  P_RES(":/rlh1000/rlp1004a.bin")    , "" , CSlot::ROM , "Printer ROM"));

}

Crlp1004a::~Crlp1004a() {
    delete paperbuf;
    delete paperdisplay;
    delete pCONNECTOR;
    delete pTAPECONNECTOR;
    delete charTable;
//    delete bells;
}


#define LATENCY (pTIMER->pPC->getfrequency()/3200)
bool Crlp1004a::run(void)
{

    pTAPECONNECTOR->Set_pin(3,true);       // RMT
    pTAPECONNECTOR->Set_pin(2,tapeOutput);    // Out
    tapeInput = pTAPECONNECTOR->Get_pin(1);      // In

    pTAPECONNECTOR_value = pTAPECONNECTOR->Get_values();
    Cbus bus;

    bus.fromUInt64(pCONNECTOR->Get_values());
    if (bus.getFunc()==BUS_SLEEP) return true;

    if (bus.getDest()!=0) return true;
    bus.setDest(0);

    if (bus.getFunc()==BUS_QUERY) {
        bus.setData(0x00);
        bus.setFunc(BUS_READDATA);
        pCONNECTOR->Set_values(bus.toUInt64());
        if (pPC->pTIMER->pPC->fp_log) fprintf(pPC->pTIMER->pPC->fp_log,"RL-P1004A BUS_QUERY\n");
        return true;
    }

    if (bus.getFunc()==BUS_SELECT) {
//        qWarning()<<"1004A BUS SELECT:"<<bus.getData();

        switch (bus.getData()) {
        case 1: Power = true; break;
        default: Power = false; break;
        }
        if (Power)
        {
            bus.setFunc(BUS_READDATA);
            pCONNECTOR->Set_values(bus.toUInt64());
        }
        return true;
    }

    if (!Power) return true;

    quint32 adr = bus.getAddr();
    quint8 data = bus.getData();

    switch (bus.getFunc()) {
    case BUS_SLEEP: break;
    case BUS_WRITEDATA:
        switch (adr) {
        case 0x3020: // flip flop K7 output
            tapeOutput = !tapeOutput;
//            qWarning()<<pTIMER->state<<" - "<<tapeOutput;
            bus.setData(0x00);
            bus.setFunc(BUS_READDATA);
            break;
        case 0x4e:
            if (data!=0) {
                Printer(data);
                if (data == 0x0d) {
                    bus.setINT(true);
                    INTrequest = true;
                }
            }
            bus.setData(0x00);
            bus.setFunc(BUS_READDATA);
            break;
        }
        break;
    case BUS_INTREQUEST:
        if (INTrequest) {
            bus.setData(0x00);
        }
        else {
            bus.setData(0xff);
        }
        bus.setFunc(BUS_READDATA);
        INTrequest = false;
        break;
    case BUS_READDATA:
        if ( (adr>=0x2000) && (adr<0x3000) ) bus.setData(mem[adr-0x2000]);
        else if (adr == 0x3060){
            bus.setData(tapeInput? 0x80 : 0x00);
        }

        break;

    }

    pCONNECTOR->Set_values(bus.toUInt64());
    return true;

}

void Crlp1004a::ComputeKey(void)
{
    if (pKEYB->LastKey == K_PFEED) {
        Refresh(0x0d);
    }
}


void Crlp1004a::SaveAsText(void)
{
    mainwindow->releaseKeyboard();

    QString s = QFileDialog::getSaveFileName(
                    mainwindow,
                    tr("Choose a filename to save under"),
                    ".",
                   tr("Text File (*.txt)"));

    QFile file(s);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    file.write(TextBuffer);

    mainwindow->grabKeyboard();
        AddLog(LOG_PRINTER,TextBuffer.data());
}

void Crlp1004a::Refresh(qint8 data)
{

    QPainter painter;

//if (posX==0) bells->play();

// copy ce126buf to ce126display
// The final paper image is 207 x 149 at (277,0) for the ce125

// grab data char to byteArray
    if ( (data == 0xff) || (data==0x0a)) return;

    if ( (data == 0x81) || (data == 0x82)) return;

    TextBuffer += data;

    if (data == 0x0d){
        top+=10;
        posX=0;
//        qWarning()<<"CR PRINTED";
    }
    else
    {
        painter.begin(paperbuf);
        int x = ((data>>4) & 0x0F)*7;
        int y = (data & 0x0F) * 8;
        painter.drawImage(      QPointF( margin + (7 * posX),top),
                            *charTable,
                            QRectF( x , y , 7,7));
        posX++;
        painter.end();
    }

    if (posX >= 40) {
        posX=0;
        top+=10;
    }

    painter.begin(paperdisplay);

    painter.drawImage(QRectF(0,MAX(149-top,0),165,MIN(top,149)),*paperbuf,QRectF(0,MAX(0,top-149),170,MIN(top,149)));

// Draw printer head
//    painter.fillRect(QRect(0 , 147,407,2),QBrush(QColor(0,0,0)));
//    painter.fillRect(QRect(21 + (7 * posX) , 147,14,2),QBrush(QColor(255,255,255)));

    painter.end();

    Refresh_Display = true;

    paperWidget->setOffset(QPoint(0,top));
    paperWidget->updated = true;

}


/*****************************************************/
/* Initialize PRINTER                                                            */
/*****************************************************/
void Crlp1004a::clearPaper(void)
{
    // Fill it blank
    paperbuf->fill(PaperColor.rgba());
    paperdisplay->fill(QColor(255,255,255,0).rgba());
    settop(10);
    setposX(0);
    // empty TextBuffer
    TextBuffer.clear();
    paperWidget->updated = true;
}


bool Crlp1004a::init(void)
{
    CPObject::init();

    setfrequency( 0);

    pCONNECTOR  = new Cconnector(this,
                                 44,
                                 0,
                                 Cconnector::Panasonic_44,
                                 "Printer connector",
                                 true,
                                 QPoint(406,72),
                                 Cconnector::EAST);     publish(pCONNECTOR);
    WatchPoint.add(&pCONNECTOR_value,64,44,this,"Printer connector");
    AddLog(LOG_PRINTER,tr("PRT initializing..."));

    pTAPECONNECTOR	= new Cconnector(this,
                                     3,
                                     1,
                                     Cconnector::Jack,
                                     "Line in / Rec",false);	publish(pTAPECONNECTOR);
    WatchPoint.add(&pTAPECONNECTOR_value,64,2,this,"Line In / Rec");

    if(pKEYB)   pKEYB->init();
    if(pTIMER)  pTIMER->init();

    // Create CE-126 Paper Image
    // The final paper image is 207 x 149 at (277,0) for the ce125
    paperbuf    = new QImage(QSize(340, 3000),QImage::Format_ARGB32);
    paperdisplay= new QImage(QSize(200, 149),QImage::Format_ARGB32);


//TODO Update the chartable with upd16343 char table
    charTable = new QImage(P_RES(":/rlh1000/rlp1004atable.bmp"));

//      bells    = new QSound("ce.wav");

// Create a paper widget

    paperWidget = new CpaperWidget(PaperPos(),paperbuf,this);
    paperWidget->updated = true;
    paperWidget->show();

    // Fill it blank
    clearPaper();

    run_oldstate = -1;


    tapeOutput = tapeInput = false;
    return true;
}



/*****************************************************/
/* Exit PRINTER                                                                          */
/*****************************************************/
bool Crlp1004a::exit(void)
{
    AddLog(LOG_PRINTER,"PRT Closing...");
    AddLog(LOG_PRINTER,"done.");
    Cprinter::exit();
    return true;
}


/*****************************************************/
/* CE-126P PRINTER emulation                                             */
/*****************************************************/

void Crlp1004a::Printer(qint8 d)
{
    if(ctrl_char && d==0x20) {
        ctrl_char=false;
        Refresh(d);
    }
    else
    {
        if(d==0xf || d==0xe || d==0x03)
            ctrl_char=true;
        else
        {
            Refresh(d);
        }
    }
}


#define         WAIT ( pPC->frequency / 10000*6)

#define RECEIVE_MODE    1
#define SEND_MODE               2
#define TEST_MODE               3



bool Crlp1004a::Get_Connector(void) {
    return true;
}

bool Crlp1004a::Set_Connector(void) {
    return true;
}





void Crlp1004a::paintEvent(QPaintEvent *event)
{
    CPObject::paintEvent(event);
}

void Crlp1004a::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu *menu= new QMenu(this);

    BuildContextMenu(menu);


    menu->addAction(tr("Dump Memory"),this,SLOT(Dump()));
    menu->addSeparator();

    menu->addAction(tr("Rotate 180"),this,SLOT(Rotate()));
//    menu.addAction(tr("Hide console"),this,SLOT(HideConsole()));

    menu->popup(event->globalPos () );
    event->accept();
}

void Crlp1004a::Rotate()
{
    rotate = ! rotate;

    delete BackgroundImageBackup;
    BackgroundImageBackup = CreateImage(QSize(getDX(), getDY()),BackGroundFname,false,false,rotate?180:0);
    delete BackgroundImage;
    BackgroundImage = new QImage(*BackgroundImageBackup);
    delete FinalImage;
    FinalImage = new QImage(*BackgroundImageBackup);

    pCONNECTOR->setSnap(rotate?QPoint(34,72):QPoint(406,72));

    pCONNECTOR->setDir(rotate?Cconnector::WEST:Cconnector::EAST);
    mask = QPixmap::fromImage(*BackgroundImageBackup).scaled(getDX()*mainwindow->zoom/100,getDY()*mainwindow->zoom/100);
    setMask(mask.mask());

    update();

        // adapt SNAP connector
}

