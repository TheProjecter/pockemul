
#include <QPainter>

#include "fp100.h"
#include "Connect.h"
#include "Keyb.h"
#include "dialogconsole.h"
#include "init.h"
#include "Inter.h"
#include "Log.h"
#include "paperwidget.h"

#define DOWN	0
#define UP		1

/*
TransMap KeyMapce140p[]={
    {1,	"FEED  ",	K_PFEED,34,234,	9},
    {2, "PRINTER ON",K_PRINT_ON,34,200,9},
    {3, "SIO ON",K_PRINT_OFF,35,150,9}
};
 int KeyMapce140pLenght = 3;
*/

Cfp100::Cfp100(CPObject *parent):Cce515p(this) {
    //setfrequency( 0);
    setcfgfname(QString("fp100"));
    BackGroundFname	= ":/EXT/ext/fp100.png";

    pCONNECTOR = new Cconnector(this,36,0,Cconnector::Centronics_36,"Parrallel Connector",false,QPoint(631,468)); publish(pCONNECTOR);
    pSavedCONNECTOR = new Cconnector(this,36,0,Cconnector::Canon_15,"Saved Parrallel Connector",true,QPoint(631,468));


    delete pKEYB; pKEYB		= new Ckeyb(this,"x710.map");

    setDXmm(302);//Pc_DX_mm = 256;
    setDYmm(120);//Pc_DY_mm = 185;
    setDZmm(36);//Pc_DZ_mm = 42;

    setDX(1078);//Pc_DX	= 895;
    setDY(817);//Pc_DY	= 615;

    setPaperPos(QRect(154,26,731,300));

    printerACK = false;
    Paper_DX = 960;

    capot = LoadImage(QSize(849,274),":/EXT/ext/fp100-capot.png");
    head = LoadImage(QSize(79,161),":/EXT/ext/fp100head.png");
}

Cfp100::~Cfp100() {
    delete pCONNECTOR;
    delete pSavedCONNECTOR;
}

bool Cfp100::init(void) {

    Cce515p::init();

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

    paperWidget->hide();
    return true;
}

bool Cfp100::run(void) {

    if (pKEYB->LastKey == K_PFEED) { PaperFeed();}

    // Si strobe change de status et passe bas vers haut, alors prelever les data
    // mettre le busy
    //
    if (GoUp(1)) {
        UINT8 car = (pCONNECTOR->Get_values() >> 1) & 0xFF;
        AddLog(LOG_PRINTER,tr("RECIEVED CHAR : %1").arg(car,2,16,QChar('0')));
        if (car != 0xff && car !=0x0a) Command( car );
        printerACK = true;
        pTIMER->resetTimer(6);
    }


    if (printerACK && (pTIMER->nsElapsedId(6) > 500)) {
        AddLog(LOG_PRINTER,tr("PRINTER printerStatusPort ACK OFF, BUSY OFF"));
        printerACK = false;
    }

    pCONNECTOR->Set_pin(10,printerACK);
    pCONNECTOR->Set_pin(32,true);

    pSavedCONNECTOR->Set_values(pCONNECTOR->Get_values());
    pCONNECTOR_value = pCONNECTOR->Get_values();

    Draw();

    return true;
}

bool Cfp100::exit(void) {

    Cce515p::exit();

    return true;
}

bool Cfp100::GoDown(int pin) {

    return (( pCONNECTOR->Get_pin(pin) == DOWN ) && (pSavedCONNECTOR->Get_pin(pin) == UP)) ? true:false;
}
bool Cfp100::GoUp(int pin) {

    return (( pCONNECTOR->Get_pin(pin) == UP ) && (pSavedCONNECTOR->Get_pin(pin) == DOWN)) ? true:false;
}
bool Cfp100::Change(int pin) {
    return (pCONNECTOR->Get_pin(pin) != pSavedCONNECTOR->Get_pin(pin) ) ? true:false;
}

void Cfp100::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    BuildContextMenu(&menu);

//    menu.addSeparator();

//    menu.addAction(tr("Show console"),pSIO,SLOT(ShowConsole()));
//    menu.addAction(tr("Hide console"),pSIO,SLOT(HideConsole()));

    menu.exec(event->globalPos () );
}

void Cfp100::ComputeKey(void)
{

//    if (pKEYB->LastKey == K_PRINT_ON) {
//        printerSwitch = true;
//    }
//    if (pKEYB->LastKey == K_PRINT_OFF) {
//        printerSwitch = false;
//    }
}

bool Cfp100::UpdateFinalImage(void) {
//    if (Refresh_Display) {
    AddLog(LOG_PRINTER,"UPDATE DISPLAY");
    Cce515p::UpdateFinalImage();


    QPainter painter;
    painter.begin(FinalImage);

    float ratio = ( (float) paperWidget->width() ) / ( paperWidget->bufferImage->width() - paperWidget->getOffset().x() );
    QRect source = QRect( QPoint(paperWidget->getOffset().x() , paperWidget->getOffset().y()  - paperWidget->height() / ratio ) , QPoint( paperWidget->bufferImage->width() , paperWidget->getOffset().y() ) );
    painter.drawImage(PaperPos(),paperWidget->bufferImage->copy(source).scaled(PaperPos().size(), Qt::IgnoreAspectRatio));//, Qt::SmoothTransformation ));



    // 112,150

    painter.drawImage(112,145,*capot);

    // Draw head
    painter.drawImage(152+lastX*ratio,178,*head);

//    // Draw switch by 180° rotation
//    QPainter painter;

//    // PRINTER SWITCH
//    painter.begin(FinalImage);
//    painter.drawImage(800,430,FinalImage->copy(800,430,22,14).mirrored(!printerSwitch,false));

    painter.end();

//    Refresh_Display = false;
//    }
}

void Cfp100::paintEvent(QPaintEvent *event)
{
    Cce515p::paintEvent(event);
}

void Cfp100::CommandSlot(qint8 data) {
    Command(data);
}


