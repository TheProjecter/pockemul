/********************************************************************************************************
 * PROGRAM      : test
 * DATE - TIME  : samedi 28 octobre 2006 - 12h40
 * AUTHOR       :  (  )
 * FILENAME     : Ce126.cpp
 * LICENSE      : GPL
 * COMMENTARY   :
 ********************************************************************************************************/
#include <QPainter>
#include <QTime>
#include <QSound>
#include <QFileDialog>
#include <QThread>

#include "common.h"

#include "ce140f.h"
#include "pcxxxx.h"
#include "Log.h"
#include "dialoganalog.h"
#include "paperwidget.h"
#include "Keyb.h"

#define DOWN	0
#define UP		1

TransMap KeyMapce140f[]={
    {1,	"FEED  ",	K_PFEED,34,234,	9},
    {2,	"RMT ON",	K_RMT_ON,34,234,9},
    {3,	"RMT OFF",	K_RMT_OFF,34,234,9},
    {4,	"POWER ON",	K_POW_ON,34,234,	9},
    {5,	"POWER OFF",K_POW_OFF,34,234,	9}
};
int KeyMapce140fLenght = 5;

Cce140f::Cce140f(CPObject *parent):Cprinter(this)
{								//[constructor]
    setfrequency( 0);
    ce140fbuf	= 0;
    ce140fdisplay= 0;
    bells		= 0;
    charTable = 0;
    ToDestroy	= false;
    BackGroundFname	= ":/EXT/ext/ce-140f.png";
    setcfgfname("ce140f");

    settop(10);
    setposX(0);
    pCONNECTOR	= new Cconnector(this,11,0,"Connector 11 pins",true,QPoint(594,238));	publish(pCONNECTOR);
    pTAPECONNECTOR	= new Cconnector(this,3,1,"Line in / Rec / Rmt",false);	publish(pTAPECONNECTOR);
    pTIMER		= new Ctimer(this);
    KeyMap      = KeyMapce140f;
    KeyMapLenght= KeyMapce140fLenght;
    pKEYB		= new Ckeyb(this,"ce140f.map");
    Pc_DX	= 620;
    Pc_DY	= 488;
    SnapPts = QPoint(594,145);

    setPaperPos(QRect(150,-3,207,149));

    stackBehind = true;

    ctrl_char = false;
    t = 0;
    c = 0;
    rmtSwitch = false;
};

void Cce140f::ComputeKey(void)
{
    if (pKEYB->LastKey == K_PFEED) {
        RefreshCe140f(0x0d);
    }
    if (pKEYB->LastKey == K_RMT_ON) {
        rmtSwitch = true;
    }
    if (pKEYB->LastKey == K_RMT_OFF) {
        rmtSwitch = false;
    }
}

//void Cce140f::resizeEvent ( QResizeEvent * ) {
//    float ratio = (float)this->width()/this->Pc_DX ;

//    QRect rect = paperWidget->baseRect;
//    paperWidget->setGeometry( rect.x()*ratio,
//                              rect.y()*ratio,
//                              rect.width()*ratio,
//                              rect.height()*ratio);
//}


void Cce140f::SaveAsText(void)
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

void Cce140f::RefreshCe140f(qint8 data)
{

    QPainter painter;

//if (posX==0) bells->play();

// copy ce126buf to ce126display
// The final paper image is 207 x 149 at (277,0) for the ce125

// grab data char to byteArray
    TextBuffer += data;

    if (data == 0x0d){
        top+=10;
        posX=0;
    }
    else
    {
        painter.begin(ce140fbuf);
        int x = ((data>>4) & 0x0F)*6;
        int y = (data & 0x0F) * 8;
        painter.drawImage(	QPointF( 25 + (7 * posX),top),
                            *charTable,
                            QRectF( x , y , 5,7));
        posX++;
        painter.end();
    }


    painter.begin(ce140fdisplay);

    painter.drawImage(QRectF(0,MAX(149-top,0),207,MIN(top,149)),*ce140fbuf,QRectF(0,MAX(0,top-149),207,MIN(top,149)));

// Draw printer head
    painter.fillRect(QRect(0 , 147,207,2),QBrush(QColor(0,0,0)));
    painter.fillRect(QRect(21 + (7 * posX) , 147,14,2),QBrush(QColor(255,255,255)));

    painter.end();

    Refresh_Display = true;

    paperWidget->setOffset(QPoint(0,top));

}


/*****************************************************/
/* Initialize PRINTER								 */
/*****************************************************/
void Cce140f::clearPaper(void)
{
    // Fill it blank
    ce140fbuf->fill(PaperColor.rgba());
    ce140fdisplay->fill(QColor(255,255,255,0).rgba());
    settop(10);
    setposX(0);
    // empty TextBuffer
    TextBuffer.clear();
}


bool Cce140f::init(void)
{
    CPObject::init();

    setfrequency( 0);

    WatchPoint.add(&pCONNECTOR_value,64,11,this,"Standard 11pins connector");
    WatchPoint.add(&pTAPECONNECTOR_value,64,2,this,"Line In / Rec");


    AddLog(LOG_PRINTER,tr("PRT initializing..."));

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();



    // Create CE-126 Paper Image
    // The final paper image is 207 x 149 at (277,0) for the ce125
    ce140fbuf	= new QImage(QSize(207, 3000),QImage::Format_ARGB32);
    ce140fdisplay= new QImage(QSize(207, 149),QImage::Format_ARGB32);

    // Fill it blank
    clearPaper();

    charTable = new QImage(":/EXT/ext/ce126ptable.bmp");

//	bells	 = new QSound("ce.wav");

// Create a paper widget

    paperWidget = new CpaperWidget(PaperPos(),ce140fbuf,this);
    paperWidget->show();

//	SET_PIN(PIN_ACK,DOWN);
AddLog(LOG_PRINTER,tr("Initial value for PIN_BUSY %1").arg(GET_PIN(PIN_BUSY)?"1":"0"));
    Previous_PIN_BUSY = GET_PIN(PIN_BUSY);
    Previous_PIN_MT_OUT1 = GET_PIN(PIN_MT_OUT1);
    code_transfer_step = 0;
    time.start();

    run_oldstate = -1;
    return true;
}



/*****************************************************/
/* Exit PRINTER										 */
/*****************************************************/
bool Cce140f::exit(void)
{
    AddLog(LOG_PRINTER,"PRT Closing...");
    AddLog(LOG_PRINTER,"done.");
    Cprinter::exit();
    return true;
}


/*****************************************************/
/* CE-126P PRINTER emulation						 */
/*****************************************************/
void Cce140f::Printer(qint8 d)
{
    if(ctrl_char && d==0x20)
        ctrl_char=false;
    else
    {
        if(d==0xf || d==0xe)
            ctrl_char=true;
        else
        {
            RefreshCe140f(d);
        }
    }
}

//********************************************************/
//* Check for E-PORT and Get data						 */
//********************************************************/
// PIN_MT_OUT2	1
// PIN_GND		2
// PIN_VGG		3
// PIN_BUSY		4
// PIN_D_OUT	5
// PIN_MT_IN	6
// PIN_MT_OUT1	7
// PIN_D_IN		8
// PIN_ACK		9
// PIN_SEL2		10
// PIN_SEL1		11
//********************************************************/



#define		WAIT ( pPC->frequency / 10000*6)

#define RECEIVE_MODE	1
#define SEND_MODE		2
#define TEST_MODE		3

extern int LogLevel;
//
// no bit start
// no bit stop
void Cce140f::pulldownsignal(void)
{

#if 0
    SET_PIN(PIN_BUSY,DOWN);
    SET_PIN(PIN_D_OUT,DOWN);
    SET_PIN(PIN_D_IN,DOWN);
    SET_PIN(PIN_SEL2,DOWN);
    SET_PIN(PIN_SEL1,DOWN);
#endif
}

#define CE126LATENCY (pTIMER->pPC->getfrequency()/3200)
//#define CE126LATENCY (getfrequency()/3200)
bool Cce140f::run(void)
{

    bool bit = false;
    ce140f_Mode=RECEIVE_MODE;

    pTAPECONNECTOR->Set_pin(3,(rmtSwitch ? GET_PIN(PIN_SEL1):true));       // RMT
    pTAPECONNECTOR->Set_pin(2,GET_PIN(PIN_MT_OUT1));    // Out
    SET_PIN(PIN_MT_IN,pTAPECONNECTOR->Get_pin(1));      // In
//SET_PIN(PIN_SEL2,UP);
//SET_PIN(PIN_SEL1,UP);
pCONNECTOR_value = pCONNECTOR->Get_values();
    pTAPECONNECTOR_value = pTAPECONNECTOR->Get_values();

#if 1
// Try to introduce a latency
    qint64			deltastate = 0;

    if (run_oldstate == -1) run_oldstate = pTIMER->state;
    deltastate = pTIMER->state - run_oldstate;
    if (deltastate < CE126LATENCY ) return true;
    run_oldstate	= pTIMER->state;
#endif

#if 0
    char dump1[20]="";
    static char dump2[20]="";

    pCONNECTOR->Dump_pin();
    strcpy(dump1,pCONNECTOR->dump);

    if (strcmp(dump1,dump2) != 0)
    {
        strcpy(dump2 ,dump1);
        AddLog(LOG_PRINTER,tr("input printer %1").arg(dump1));
    }
#endif

//	AddLog(LOG_PRINTER,tr("%1").arg(dump1));

    bool PIN_BUSY_GoDown = ( ( GET_PIN(PIN_BUSY) == Previous_PIN_BUSY ) && (Previous_PIN_BUSY == DOWN)) ? true:false;
    bool PIN_BUSY_Change = (GET_PIN(PIN_BUSY) != Previous_PIN_BUSY ) ? true:false;
    bool PIN_MT_OUT1_GoDown = ( ( GET_PIN(PIN_MT_OUT1) == Previous_PIN_MT_OUT1) &&	(Previous_PIN_MT_OUT1 == DOWN)) ? true:false;
    bool PIN_MT_OUT1_GoUp = ( (Previous_PIN_MT_OUT1 == DOWN) && (GET_PIN(PIN_MT_OUT1) == UP )) ? true:false;


    switch (code_transfer_step) {
    case 0 :    if ((GET_PIN(PIN_MT_OUT1) == UP) && (GET_PIN(PIN_D_OUT)==UP)) {
                    time.restart();
                    code_transfer_step=1;
                    if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 8;
                }
                break;
    case 1 :    if ((GET_PIN(PIN_MT_OUT1) == UP) && (GET_PIN(PIN_D_OUT)==UP)) {
                    if (time.elapsed() > 40) {
                        // Code transfer sequence started
                        // Raise ACK
                        code_transfer_step = 2;
                        SET_PIN(PIN_ACK,UP);
                        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 9;
                    }
                }
                else {
                    code_transfer_step=0;
                    //if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 7;
                }
                break;
    case 2:     if (GET_PIN(PIN_BUSY) == UP )	//check for BUSY
                {
                    if(GET_PIN(PIN_D_OUT) == UP)
                    {
                        bit = true;;
                    } else
                    {
                        bit = false;
                    }
                    t>>=1;
                    if (bit) t|=0x80;
                    if((c=(++c)&7)==0)
                    {
                        AddLog(LOG_PRINTER,tr("send char to printer : %1").arg(t,2,16) );
                        Printer(t);
                        SET_PIN(PIN_ACK,DOWN);
                        code_transfer_step=4;
                        //t=0; c=0;
                    }
                    else {
                        SET_PIN(PIN_ACK,DOWN);
                        code_transfer_step=3;
                    }
                }
                break;
    case 3:     code_transfer_step=2;
                    // wait 2 ms and raise ACK
                    SET_PIN(PIN_ACK,UP);

                break;
    case 4:     if ((GET_PIN(PIN_BUSY) == DOWN)&&(GET_PIN(PIN_MT_OUT1) == DOWN)) {
                    SET_PIN(PIN_ACK,UP);
                    code_transfer_step=5;
                    time.restart();
                    t=0; c=0;
                }
                break;
    case 5:     if (time.elapsed()>9) {
                    SET_PIN(PIN_ACK,DOWN);
                    code_transfer_step=10;

                }
                break;
    case 10:    if (GET_PIN(PIN_BUSY) == UP) {
                    // read the 4 bits

                    t = GET_PIN(PIN_SEL1) + (GET_PIN(PIN_SEL2)<<1) + (GET_PIN(PIN_D_OUT)<<2) + (GET_PIN(PIN_D_IN)<<3);

                    //AddLog(LOG_PRINTER,tr("send first 4bits to floppy : %1").arg(t,2,16) );
                    SET_PIN(PIN_ACK,UP);
                    code_transfer_step=11;
                    if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 10;
                }
                break;
    case 11:    if (GET_PIN(PIN_BUSY) == DOWN) {
                    SET_PIN(PIN_ACK,DOWN);
                    code_transfer_step=12;
                }
                break;
    case 12:    if (GET_PIN(PIN_BUSY) == UP) {
                    // read the 4 bits
                    t += (GET_PIN(PIN_SEL1)<<4) + (GET_PIN(PIN_SEL2)<<5) + (GET_PIN(PIN_D_OUT)<<6) + (GET_PIN(PIN_D_IN)<<7);
                    SET_PIN(PIN_ACK,UP);
                    AddLog(LOG_PRINTER,tr("send char to floppy : %1 - %2").arg(t,2,16).arg(QChar(t)) );
                    code_transfer_step=13;
                    if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 11;
                }
                break;
    case 13:    if (GET_PIN(PIN_BUSY) == DOWN) {
                    SET_PIN(PIN_ACK,DOWN);
                    code_transfer_step=10;
                }
                break;
    }





    switch (ce140f_Mode)
    {
            case RECEIVE_MODE:
#if 0
                if ( PIN_BUSY_GoDown &&
                     PIN_MT_OUT1_GoDown &&
                     (GET_PIN(PIN_ACK) == UP) &&
                     (time.elapsed() > 30 ) )
                {
                    AddLog(LOG_PRINTER,tr("ACK timeout"));
                    time.restart();
                    SET_PIN(PIN_ACK,DOWN);
                    if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 1;
                }

                if ( PIN_MT_OUT1_GoUp)
                {
                    Previous_PIN_MT_OUT1 = GET_PIN(PIN_MT_OUT1);
                    AddLog(LOG_PRINTER,tr("XIN from low to HIGHT"));
                    t=0;
                    c=0;
                    if (GET_PIN(PIN_BUSY) == DOWN )
                    {
                        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 2;
                        //time.restart();
                        SET_PIN(PIN_ACK,UP);
                        AddLog(LOG_PRINTER,tr("CHANGE ACK TO %1").arg(GET_PIN(PIN_ACK)?"1":"0"));
                        Previous_PIN_BUSY = GET_PIN(PIN_BUSY);
//						return;
                    }
                }

                if (GET_PIN(PIN_SEL1) == UP )
                {
                    t=0;
                    c=0;
                }

                if (PIN_BUSY_Change )	//check for BUSY  - F03
                {
                    AddLog(LOG_PRINTER,tr("PIN_BUSY CHANGE from %1 TO %2").arg(Previous_PIN_BUSY?"1":"0").arg(GET_PIN(PIN_BUSY)?"1":"0"));
                    Previous_PIN_BUSY = GET_PIN(PIN_BUSY);
                    if (GET_PIN(PIN_ACK) == UP) //check for ACK  - F03
                    {
                        SET_PIN(PIN_ACK,DOWN);
                        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 3;
                    }
                    else
                    {
                        SET_PIN(PIN_ACK,UP);
                        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 4;
                    }
                    AddLog(LOG_PRINTER,tr("CHANGE ACK TO %1").arg(GET_PIN(PIN_ACK)?"1":"0"));

                    if (GET_PIN(PIN_BUSY) == UP )	//check for BUSY  - F03
                    {
                        if(GET_PIN(PIN_D_OUT) == UP)
                        {
                            bit = true;;
                        } else
                        {
                            bit = false;
                        }
                        t>>=1;
                        if (bit) t|=0x80;
                        if((c=(++c)&7)==0)
                        {
                            AddLog(LOG_PRINTER,tr("send char to printer : %1").arg(t,2,16) );
                            Printer(t);
                            t=0; c=0;
                        }


                    }
                }
#endif
                Previous_PIN_BUSY = GET_PIN(PIN_BUSY);
                Previous_PIN_MT_OUT1 = GET_PIN(PIN_MT_OUT1);
                pulldownsignal();


    }
#if 0
    char dump11[20]="";
    static char dump12[20]="";

    pCONNECTOR->Dump_pin();
    strcpy(dump11,pCONNECTOR->dump);

    if (strcmp(dump11,dump12) != 0)
    {
        strcpy(dump12 ,dump11);
        AddLog(LOG_PRINTER,tr("output printer %1").arg(dump11));
    }
#endif

    pCONNECTOR_value = pCONNECTOR->Get_values();
    pTAPECONNECTOR_value = pTAPECONNECTOR->Get_values();


    return true;
}




