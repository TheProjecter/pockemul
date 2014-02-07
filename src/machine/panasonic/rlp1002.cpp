#include <QtGui>
#include <QTime>
#include <QFileDialog>
#include <QThread>
#include <QDebug>

#include "common.h"
#include "fluidlauncher.h"

#include "rlp1002.h"
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


Crlp1002::Crlp1002(CPObject *parent):Cprinter(this)
{ //[constructor]
    Q_UNUSED(parent)

    setfrequency( 0);
    paperbuf    = 0;
    paperdisplay= 0;
    //bells             = 0;
    charTable = 0;
    margin = 25;
    BackGroundFname     = P_RES(":/rlh1000/rlp1002.png");
    setcfgfname("rlp1002");

    settop(10);
    setposX(0);

    pTIMER              = new Ctimer(this);
//    pKEYB               = new Ckeyb(this,"rlp1002.map");
    setDXmm(113);
    setDYmm(95);
    setDZmm(51);
 // Ratio = 3,57
    setDX(848);//Pc_DX  = 75;
    setDY(340);//Pc_DY  = 20;

//    setPaperPos(QRect(53,0,291,216));
    setPaperPos(QRect(57,-20,318,236));

    ctrl_char = false;
    t = 0;
    c = 0;
    rmtSwitch = false;

    rotate = false;
    INTrequest = false;
    printing = false;
    receiveMode = false;
    CRLFPending = false;

    memsize             = 0x2000;
    InitMemValue        = 0x7f;
    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,  P_RES(":/rlh1000/rlp1004a.bin")    , "" , CSlot::ROM , "Printer ROM"));

}

Crlp1002::~Crlp1002() {
    delete paperbuf;
    delete paperdisplay;
    delete pCONNECTOR;
    delete charTable;
//    delete bells;
}


#define LATENCY (pTIMER->pPC->getfrequency()/3200)
bool Crlp1002::run(void)
{

    static quint64 _state=0;

     Cbus bus;

    bus.fromUInt64(pCONNECTOR->Get_values());

    if (printing) {
        if (!buffer.isEmpty()) {
            // Print char column one by one from buffer
            // Wait 20char/sec -> 140 cols / Sec -> 7,142ms / col
            if (pTIMER->usElapsed(_state)>=7142)
            {
                _state = pTIMER->state;
                drawGraph(buffer.at(0));
//                qWarning()<<"Printing data:"<<buffer.at(0)<<":"<<QChar(buffer.at(0));
                buffer.remove(0,1);
            }
        }
        else {
            printing = false;
        }
    }
    else {
        if (CRLFPending) {
//            qWarning()<<"Send CRLF";
            CRLFPending = false;
            top+=10;
            posX=0;
//                bus.setINT(true);
            INTrequest = true;
            pCONNECTOR->Set_values(bus.toUInt64());
            Refresh();
        }
    }

    if (bus.getFunc()==BUS_SLEEP) return true;

    if (bus.getDest()!=0) return true;
    bus.setDest(0);

//    qWarning()<<"PRINTER:"<<bus.toLog();
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
            bus.setData(0x01);
            pCONNECTOR->Set_values(bus.toUInt64());
        }
        return true;
    }

    switch (bus.getFunc()) {
    case BUS_LINE3: // Print buffer
        switch(bus.getData()) {
        case 0: // Print
//            qWarning()<<"BUS_TOUCH:"<<bus.getData()<<  "PRINTING "<<buffer.size()<<" chars";
//            Refresh(0);
            printing = true;
//            buffer.clear();
            INTrequest = false;
            break;
        case 5: //
//            qWarning()<<"BUS_TOUCH:"<<bus.getData();
            buffer.clear();
            receiveMode = true;
            INTrequest = true;
//            receiveMode = true;
            break;
        case 4: // CR/LF
//            Refresh(0x0d);
//            qWarning()<<"BUS_TOUCH:"<<bus.getData();

//            printing = true;
            CRLFPending = true;
//            INTrequest = true;
            break;
        default: qWarning()<<"BUS_TOUCH:"<<bus.getData();
            break;
        }
        bus.setFunc(BUS_ACK);
        break;
    case BUS_INTREQUEST:
        if (INTrequest) {
//            qWarning()<<"INTREQUEST:true";
            bus.setINT(true);
            bus.setData(0x00);
            INTrequest = false;
        }
        else {
//            qWarning()<<"INTREQUEST:false";
            bus.setData(0xff);
        }
        bus.setFunc(BUS_READDATA);
        pCONNECTOR->Set_values(bus.toUInt64());
        return true;
        break;
    case BUS_LINE1:
        if (receiveMode) {
            buffer.append(bus.getData());
//            qWarning()<<"Receive data:"<<bus.getData();

            INTrequest = true;
        }
        bus.setFunc(BUS_ACK);
        break;
    default: break;
    }


    if (!Power) return true;

    quint32 adr = bus.getAddr();
//    quint8 data = bus.getData();

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
        }
        break;


    case BUS_READDATA:
        if ( (adr>=0x2000) && (adr<0x3000) ) bus.setData(mem[adr-0x2000]);
        else if (adr == 0x3060){
            bus.setData(tapeInput? 0x80 : 0x00);
        }
        else bus.setData(0x7f);
        break;
    default: break;

    }

    pCONNECTOR->Set_values(bus.toUInt64());
    return true;

}


void Crlp1002::SaveAsText(void)
{
    QMessageBox::warning(mainwindow, "PockEmul",
                          tr("This printer is a pure graphic printer (yes it is!!!)\n") +
                          tr("Saving output as text is irrelevant") );
}

void Crlp1002::drawGraph(quint8 data) {
    QPainter painter;

    painter.begin(paperbuf);
    for (int b=0; b<8;b++)
    {
        if ((data>>b)&0x01) painter.drawPoint( posX, top+b);
    }
    posX++;
    painter.end();

    Refresh();
}

void Crlp1002::Refresh()
{
    QPainter painter;

    painter.begin(paperdisplay);
    painter.drawImage(QRectF(0,MAX(149-top,0),165,MIN(top,159)),*paperbuf,QRectF(0,MAX(0,top-149),170,MIN(top,149)));

    // Draw printer head
    //    painter.fillRect(QRect(0 , 147,407,2),QBrush(QColor(0,0,0)));
    //    painter.fillRect(QRect(21 + (7 * posX) , 137,14,2),QBrush(QColor(128,0,0)));

    painter.end();

    Refresh_Display = true;

    paperWidget->setOffset(QPoint(0,top));
    paperWidget->updated = true;

}


/*****************************************************/
/* Initialize PRINTER                                                            */
/*****************************************************/
void Crlp1002::clearPaper(void)
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


bool Crlp1002::init(void)
{
    CPObject::init();

    setfrequency( 0);

    pCONNECTOR = new Cconnector(this,44,0,
                                    Cconnector::Panasonic_44,
                                    "44 pins conector",
                                    true,
                                    QPoint(37,72),
                                    Cconnector::WEST);
    publish(pCONNECTOR);
    WatchPoint.add(&pCONNECTOR_value,64,44,this,"Printer connector");
    AddLog(LOG_PRINTER,tr("PRT initializing..."));

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
bool Crlp1002::exit(void)
{
    AddLog(LOG_PRINTER,"PRT Closing...");
    AddLog(LOG_PRINTER,"done.");
    Cprinter::exit();
    return true;
}


/*****************************************************/
/* CE-126P PRINTER emulation                                             */
/*****************************************************/



#define         WAIT ( pPC->frequency / 10000*6)

#define RECEIVE_MODE    1
#define SEND_MODE               2
#define TEST_MODE               3



bool Crlp1002::Get_Connector(void) {
    return true;
}

bool Crlp1002::Set_Connector(void) {
    return true;
}





void Crlp1002::paintEvent(QPaintEvent *event)
{
    CPObject::paintEvent(event);
}

void Crlp1002::contextMenuEvent ( QContextMenuEvent * event )
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

void Crlp1002::Rotate()
{
    rotate = ! rotate;

    delete BackgroundImageBackup;
    BackgroundImageBackup = CreateImage(QSize(getDX(), getDY()),BackGroundFname,false,false,rotate?180:0);
    delete BackgroundImage;
    BackgroundImage = new QImage(*BackgroundImageBackup);
    delete FinalImage;
    FinalImage = new QImage(*BackgroundImageBackup);

    pCONNECTOR->setSnap(rotate?QPoint(811,72):QPoint(37,72));

    pCONNECTOR->setDir(rotate?Cconnector::EAST:Cconnector::WEST);
    mask = QPixmap::fromImage(*BackgroundImageBackup).scaled(getDX()*mainwindow->zoom/100,getDY()*mainwindow->zoom/100);
    setMask(mask.mask());

    update();

        // adapt SNAP connector
}


extern int ask(QWidget *parent,QString msg,int nbButton);
#define KEY(c)	((pKEYB->keyPressedList.contains(TOUPPER(c)) || \
                  pKEYB->keyPressedList.contains(c) || \
                  pKEYB->keyPressedList.contains(TOLOWER(c)))?1:0)
void Crlp1002::ComputeKey()
{
    if (pKEYB->LastKey == K_PFEED) {
        top+=10;
        Refresh();
    }

    // Manage left connector click
    if (KEY(0x240) && (currentView==FRONTview)) {
        pKEYB->keyPressedList.removeAll(0x240);
        FluidLauncher *launcher = new FluidLauncher(mainwindow,
                                     QStringList()<<P_RES(":/pockemul/configExt.xml"),
                                     FluidLauncher::PictureFlowType,
                                     "Jack_3");
        launcher->show();
    }

}
