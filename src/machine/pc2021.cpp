


#include <QPainter>
#include <QTime>
#include <QSound>
#include <QFileDialog>
#include <QThread>
#include <QDebug>

#include "common.h"

#include "pc2021.h"
#include "pcxxxx.h"
#include "Log.h"
#include "dialoganalog.h"
#include "paperwidget.h"
#include "Keyb.h"
#include "Connect.h"

#define DOWN	0
#define UP		1

TransMap KeyMappc2021[]={
    {1,	"FEED  ",	K_PFEED,34,234,	9},
    {2,	"POWER ON",	K_POW_ON,34,234,	9},
    {3,	"POWER OFF",K_POW_OFF,34,234,	9}
};
int KeyMappc2021Lenght = 3;

Cpc2021::Cpc2021(CPObject *parent):Cprinter(this)
{								//[constructor]
    setfrequency( 0);
    pc2021buf	= 0;
    pc2021display= 0;
    //bells		= 0;
    charTable = 0;
    margin = 25;
    ToDestroy	= false;
    BackGroundFname	= P_RES(":/ext/pc-2021.png");
    setcfgfname("pc2021");

    settop(10);
    setposX(0);

    pTIMER		= new Ctimer(this);
    KeyMap      = KeyMappc2021;
    KeyMapLenght= KeyMappc2021Lenght;
    pKEYB		= new Ckeyb(this,"pc2021.map");
    setDXmm(108);
    setDYmm(130);
    setDZmm(43);

    setDX(386);//Pc_DX	= 620;
    setDY(464);//Pc_DY	= 488;
    SnapPts = QPoint(594,145);

    setPaperPos(QRect(70,-3,275,149));

    ctrl_char = false;
    t = 0;
    c = 0;
    rmtSwitch = false;

    internal_device_code = 0x0f;
}

Cpc2021::~Cpc2021() {
    delete pc2021buf;
    delete pc2021display;
    delete pCONNECTOR;
    delete charTable;
//    delete bells;
}

void Cpc2021::ComputeKey(void)
{
    if (pKEYB->LastKey == K_PFEED) {
        Refreshpc2021(0x0d);
    }
}


void Cpc2021::SaveAsText(void)
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

void Cpc2021::Refreshpc2021(qint8 data)
{

    QPainter painter;

//if (posX==0) bells->play();

// copy ce126buf to ce126display
// The final paper image is 207 x 149 at (277,0) for the ce125

// grab data char to byteArray
    if ( (data == 0xff) || (data==0x0a)) return;

    TextBuffer += data;

    if (data == 0x0d){
        top+=10;
        posX=0;
//        qWarning()<<"CR PRINTED";
    }
    else
    {
        painter.begin(pc2021buf);
        int x = ((data>>4) & 0x0F)*6;
        int y = (data & 0x0F) * 8;
        painter.drawImage(	QPointF( margin + (7 * posX),top),
                            *charTable,
                            QRectF( x , y , 5,7));
        posX++;
        painter.end();
    }

    if (posX >= 20) {
        posX=0;
        top+=10;
    }

    painter.begin(pc2021display);

    painter.drawImage(QRectF(0,MAX(149-top,0),165,MIN(top,149)),*pc2021buf,QRectF(0,MAX(0,top-149),170,MIN(top,149)));

// Draw printer head
//    painter.fillRect(QRect(0 , 147,407,2),QBrush(QColor(0,0,0)));
//    painter.fillRect(QRect(21 + (7 * posX) , 147,14,2),QBrush(QColor(255,255,255)));

    painter.end();

    Refresh_Display = true;

    paperWidget->setOffset(QPoint(0,top));
    paperWidget->updated = true;

}


/*****************************************************/
/* Initialize PRINTER								 */
/*****************************************************/
void Cpc2021::clearPaper(void)
{
    // Fill it blank
    pc2021buf->fill(PaperColor.rgba());
    pc2021display->fill(QColor(255,255,255,0).rgba());
    settop(10);
    setposX(0);
    // empty TextBuffer
    TextBuffer.clear();
    paperWidget->updated = true;
}


bool Cpc2021::init(void)
{
    CPObject::init();

    setfrequency( 0);

    pCONNECTOR	= new Cconnector(this,
                                 9,
                                 0,
                                 Cconnector::DIN_8,
                                 "Printer connector",
                                 true,
                                 QPoint(386,238),
                                 Cconnector::EAST);	publish(pCONNECTOR);
    WatchPoint.add(&pCONNECTOR_value,64,11,this,"Printer connector");
    AddLog(LOG_PRINTER,tr("PRT initializing..."));

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();

    // Create CE-126 Paper Image
    // The final paper image is 207 x 149 at (277,0) for the ce125
    pc2021buf	= new QImage(QSize(170, 3000),QImage::Format_ARGB32);
    pc2021display= new QImage(QSize(170, 149),QImage::Format_ARGB32);


//TODO Update the chartable with upd16343 char table
    charTable = new QImage(P_RES(":/ext/ce126ptable.bmp"));

//	bells	 = new QSound("ce.wav");

// Create a paper widget

    paperWidget = new CpaperWidget(PaperPos(),pc2021buf,this);
    paperWidget->updated = true;
    paperWidget->show();

    // Fill it blank
    clearPaper();

    run_oldstate = -1;

    return true;
}



/*****************************************************/
/* Exit PRINTER										 */
/*****************************************************/
bool Cpc2021::exit(void)
{
    AddLog(LOG_PRINTER,"PRT Closing...");
    AddLog(LOG_PRINTER,"done.");
    Cprinter::exit();
    return true;
}


/*****************************************************/
/* CE-126P PRINTER emulation						 */
/*****************************************************/

void Cpc2021::Printer(qint8 d)
{
    if(ctrl_char && d==0x20) {
        ctrl_char=false;
        Refreshpc2021(d);
    }
    else
    {
        if(d==0xf || d==0xe || d==0x03)
            ctrl_char=true;
        else
        {
            Refreshpc2021(d);
        }
    }
}


#define		WAIT ( pPC->frequency / 10000*6)

#define RECEIVE_MODE	1
#define SEND_MODE		2
#define TEST_MODE		3



bool Cpc2021::Get_Connector(void) {
    return true;
}

bool Cpc2021::Set_Connector(void) {
    return true;
}


#define PC2021LATENCY (pTIMER->pPC->getfrequency()/3200)

bool Cpc2021::run(void)
{

    Get_Connector();

#if 1
// Try to introduce a latency
    quint64	deltastate = 0;

    if (run_oldstate == -1) run_oldstate = pTIMER->state;
    deltastate = pTIMER->state - run_oldstate;
    if (deltastate < PC2021LATENCY ) return true;
    run_oldstate	= pTIMER->state;
#endif

    quint8 c = pCONNECTOR->Get_values();

    if (c>0)
    {
        AddLog(LOG_PRINTER,QString("Recieve:%1 = (%2)").arg(c,2,16,QChar('0')).arg(QChar(c)));
        SET_PIN(9,1);
        Printer(c);
    }

    pCONNECTOR_value = pCONNECTOR->Get_values();




    Set_Connector();

    return true;
}
