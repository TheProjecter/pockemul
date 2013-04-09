
//TODO: Key management

#include <QPainter>

#include "fp40.h"
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

Cfp40::Cfp40(CPObject *parent):CprinterCtronics(this) {
    //setfrequency( 0);
    setcfgfname(QString("fp40"));
    BackGroundFname	= ":/EXT/ext/fp40.png";



    delete pKEYB; pKEYB		= new Ckeyb(this,"x710.map");

    setDXmm(210);//Pc_DX_mm = 256;
    setDYmm(145);//Pc_DY_mm = 185;
    setDZmm(42);//Pc_DZ_mm = 42;

    setDX(750);//Pc_DX	= 895;
    setDY(518);//Pc_DY	= 615;


    margin = 40;
    paperWidth = 500;
    setPaperPos(QRect(54,26,500,300));
}

Cfp40::~Cfp40() {
}

bool Cfp40::init(void) {

    CprinterCtronics::init();

    charTable = new QImage(":/EXT/ext/ce126ptable.bmp");

    return true;
}

bool Cfp40::run(void) {

    CprinterCtronics::run();

    return true;
}

bool Cfp40::exit(void) {

    CprinterCtronics::exit();

    return true;
}


void Cfp40::ComputeKey(void)
{

//    if (pKEYB->LastKey == K_PRINT_ON) {
//        printerSwitch = true;
//    }
//    if (pKEYB->LastKey == K_PRINT_OFF) {
//        printerSwitch = false;
//    }
}

void Cfp40::Printer(quint8 data) {
    QPainter painter;


    TextBuffer += data;

    if (data == 0x0d){
        top+=10;
        setposX(0);
//        qWarning()<<"CR PRINTED";
    }
    else
    {
        painter.begin(printerbuf);
        int x = ((data>>4) & 0x0F)*6;
        int y = (data & 0x0F) * 8;
        painter.drawImage(	QPointF( margin + (7 * posX),top),
                            *charTable,
                            QRectF( x , y , 5,7));
        posX++;
        painter.end();
    }


    painter.begin(printerdisplay);

    painter.drawImage(QRectF(0,MAX(149-top,0),paperWidth,MIN(top,149)),*printerbuf,QRectF(0,MAX(0,top-149),paperWidth,MIN(top,149)));

// Draw printer head
//    painter.fillRect(QRect(0 , 147,207,2),QBrush(QColor(0,0,0)));
//    painter.fillRect(QRect(21 + (7 * posX) , 147,14,2),QBrush(QColor(255,255,255)));

    painter.end();

    Refresh_Display = true;

    paperWidget->setOffset(QPoint(0,top));
    paperWidget->updated = true;


}

bool Cfp40::UpdateFinalImage(void) {

    CprinterCtronics::UpdateFinalImage();

    return true;
}




