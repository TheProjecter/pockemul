#include <QDebug>

#include "pc1211.h"
#include "cpu.h"
#include "Keyb.h"
#include "Lcdc_pc1211.h"
#include "tinybasic/tinybasic.h"
#include "Inter.h"

Cpc1211::Cpc1211(CPObject *parent)	: CpcXXXX(parent)
{
    setfrequency( (int) 576000/3);
    setcfgfname(QString("pc1211"));

    SessionHeader	= "PC1211PKM";
    Initial_Session_Fname ="pc1211.pkm";

    BackGroundFname	= ":/pc1211/pc-1211.png";
    LcdFname		= ":/PC1245/pc1245/1245lcd.png";
    SymbFname		= ":/PC1245/pc1245/1245symb.png";

    memsize		= 0x10000;

    KeyMap		= KeyMap1250;
    KeyMapLenght= KeyMap1250Lenght;

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(175);//Pc_DX_mm = 135;
    setDYmm(70);//Pc_DY_mm = 70;
    setDZmm(15);//Pc_DZ_mm = 10;

    setDX(626);//Pc_DX		= 483;//409;
    setDY(252);//Pc_DY		= 252;//213;

    Lcd_X		= 55;
    Lcd_Y		= 49;
    Lcd_DX		= 144;//168;//144 ;
    Lcd_DY		= 8;
    Lcd_ratio_X	= 2 * 1.18;
    Lcd_ratio_Y	= 2 * 1.18;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    pLCDC		= new Clcdc_pc1211(this);
    pKEYB		= new Ckeyb(this,"pc1211.map");
    pCPU = new CTinyBasic(this);
    pTIMER		= new Ctimer(this);
    pBASIC = (CTinyBasic *)pCPU;
    DisplayWaitForRTN = false;
}

Cpc1211::~Cpc1211()
{
}

bool Cpc1211::init()
{
    pCPU->init();
    CpcXXXX::TurnON();
    return true;
}

bool Cpc1211::run()
{
//    qWarning("RUN");
    CTinyBasic *pBASIC = (CTinyBasic *)pCPU;

    if (DisplayWaitForRTN) {
        if (pKEYB->LastKey >0) {
            qWarning()<< "Clear buffer";
            // Remove until CR
            pBASIC->outputBuffer.remove(0,pBASIC->outputBuffer.indexOf('\n')+1);
            if (pBASIC->outputBuffer.right(1).startsWith('\n')) pBASIC->outputBuffer.chop(1);
            if (!pBASIC->outputBuffer.contains('\n')) DisplayWaitForRTN = false;
//            DisplayWaitForRTN = false;
//            pBASIC->outputBuffer.clear();
            Refresh_Display = true;
//            pKEYB->LastKey = 0;
        }
        else
            return true;
    }

    switch (pKEYB->LastKey) {
    case 0: break;
    case K_SHT: break;
    case K_BRK: pBASIC->breakFlag = true; break;
    default:
        pBASIC->commandBuffer.append(pKEYB->LastKey);
        pKEYB->LastKey = 0;
    }

    CpcXXXX::run();

    return true;
}

void Cpc1211::afficheChar(quint8 c) {

//    Clcdc_pc1211 *myLCD = (Clcdc_pc1211*) pLCDC;

//    for (int i=0;i<pBASIC->outputBuffer.size();i++) {
//        quint8 c = pBASIC->outputBuffer.at(i);
//        myLCD->DrawChar(c,i);
//    }
}

bool Cpc1211::exit()
{

    return true;
}
