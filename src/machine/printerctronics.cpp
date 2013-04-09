


#include <QPainter>
#include <QTime>
#include <QSound>
#include <QFileDialog>
#include <QThread>
#include <QDebug>

#include "common.h"

#include "printerctronics.h"
#include "pcxxxx.h"
#include "Log.h"
#include "dialoganalog.h"
#include "paperwidget.h"
#include "Keyb.h"
#include "Connect.h"

#define DOWN	0
#define UP		1


CprinterCtronics::CprinterCtronics(CPObject *parent):Cprinter(this)
{								//[constructor]
}

CprinterCtronics::~CprinterCtronics() {
    delete pc2021buf;
    delete pc2021display;
    delete pCONNECTOR;
    delete charTable;
}

void CprinterCtronics::ComputeKey(void)
{

}


void CprinterCtronics::SaveAsText(void)
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



/*****************************************************/
/* Initialize PRINTER								 */
/*****************************************************/
void CprinterCtronics::clearPaper(void)
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


bool CprinterCtronics::init(void)
{
    CPObject::init();

    setfrequency( 0);

    pCONNECTOR = new Cconnector(this,36,0,Cconnector::Centronics_36,"Parrallel Connector",false,QPoint(631,468)); publish(pCONNECTOR);
    pSavedCONNECTOR = new Cconnector(this,36,0,Cconnector::Canon_15,"Saved Parrallel Connector",true,QPoint(631,468));

    QHash<int,QString> lbl;
    lbl[1] = "STROBE";
    lbl[2] = "D1";
    lbl[3] = "D2";
    lbl[4] = "D3";
    lbl[5] = "D4";
    lbl[6] = "D5";
    lbl[7] = "D6";
    lbl[8] = "D7";
    lbl[9] = "D8";
    lbl[10]= "ACK";
    lbl[11]= "BUSY";
    lbl[31]= "INIT";
    lbl[32]= "ERROR";
    WatchPoint.add(&pCONNECTOR_value,64,36,this,"// 36pins connector",lbl);
    AddLog(LOG_PRINTER,tr("PRT initializing..."));

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();

    // Create CE-126 Paper Image
    // The final paper image is 207 x 149 at (277,0) for the ce125
    pc2021buf	= new QImage(QSize(170, 3000),QImage::Format_ARGB32);
    pc2021display= new QImage(QSize(170, 149),QImage::Format_ARGB32);


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
bool CprinterCtronics::exit(void)
{
    AddLog(LOG_PRINTER,"PRT Closing...");
    AddLog(LOG_PRINTER,"done.");
    Cprinter::exit();
    return true;
}





bool CprinterCtronics::Get_Connector(void) {
    return true;
}

bool CprinterCtronics::Set_Connector(void) {
    return true;
}

bool CprinterCtronics::run(void)
{
//    if (pKEYB->LastKey == K_PFEED) { PaperFeed();}

    // Si strobe change de status et passe bas vers haut, alors prelever les data
    // mettre le busy
    //
    if (GoUp(1)) {
        UINT8 car = (pCONNECTOR->Get_values() >> 1) & 0xFF;
        AddLog(LOG_PRINTER,tr("RECIEVED CHAR : %1").arg(car,2,16,QChar('0')));
        if (car != 0xff && car !=0x0a) Printer(car);
        printerACK = true;
        pTIMER->resetTimer(6);
    }


    if (printerACK && (pTIMER->nsElapsedId(6) > 500)) {
        AddLog(LOG_PRINTER,tr("PRINTER printerStatusPort ACK OFF, BUSY OFF"));
        printerACK = false;
    }

//    printerBUSY = (moveBuffer.size()>100) ? true:false;
    pCONNECTOR->Set_pin(10,printerACK);
    pCONNECTOR->Set_pin(11,printerBUSY);
    pCONNECTOR->Set_pin(32,true);

    pSavedCONNECTOR->Set_values(pCONNECTOR->Get_values());
    pCONNECTOR_value = pCONNECTOR->Get_values();


    return true;

}

bool CprinterCtronics::GoDown(int pin) {

    return (( pCONNECTOR->Get_pin(pin) == DOWN ) && (pSavedCONNECTOR->Get_pin(pin) == UP)) ? true:false;
}
bool CprinterCtronics::GoUp(int pin) {

    return (( pCONNECTOR->Get_pin(pin) == UP ) && (pSavedCONNECTOR->Get_pin(pin) == DOWN)) ? true:false;
}
bool CprinterCtronics::Change(int pin) {
    return (pCONNECTOR->Get_pin(pin) != pSavedCONNECTOR->Get_pin(pin) ) ? true:false;
}

void CprinterCtronics::Printer(qint8 d)
{

}
