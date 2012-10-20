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
    LcdFname		= ":/pc1211/pc-1211lcd.png";
    SymbFname		= ":/pc1211/pc-1211symb.png";

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

    Lcd_X		= 46;
    Lcd_Y		= 50;
    Lcd_DX		= 144;//168;//144 ;
    Lcd_DY		= 8;
    Lcd_ratio_X	= 2 * 1.375;
    Lcd_ratio_Y	= 2 * 1.375;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 380;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    pLCDC		= new Clcdc_pc1211(this);
    pKEYB		= new Ckeyb(this,"pc1211.map");
    pCPU = new CTinyBasic(this);
    pTIMER		= new Ctimer(this);
    pBASIC = (CTinyBasic *)pCPU;
    DisplayWaitForRTN = false;
    pLCDC1211 = (Clcdc_pc1211*)pLCDC;

    cursorPos =0;
}

Cpc1211::~Cpc1211()
{
}

bool Cpc1211::init()
{
//    pCPU->init();
    CpcXXXX::init();
    CpcXXXX::TurnON();
    return true;
}

bool Cpc1211::run()
{
//    qWarning("RUN");
    CTinyBasic *pBASIC = (CTinyBasic *)pCPU;

    pTIMER->state+=1000;

    if (pKEYB->LastKey==K_BRK) {
        pBASIC->breakFlag = true;
        pKEYB->LastKey = 0;
    }


    if (pBASIC->pauseFlag) {
//        qWarning()<<pTIMER->msElapsed(pBASIC->waitState);
        if (pTIMER->msElapsed(pBASIC->waitState)<1300)
            return true;
        qWarning()<<"end PAUSE";
        pBASIC->pauseFlag = false;
        pBASIC->waitForRTN = false;
        inputBuffer.clear();
        pBASIC->inputMode = false;
        pBASIC->outputBuffer.clear();
    }

    Refresh_Display = true;

    if (pBASIC->waitForRTN) {
//        pBASIC->inLIST = false;

//        if (pKEYB->LastKey==K_SHT) pKEYB->LastKey=0; return true;
        switch (pKEYB->LastKey) {
        case 0:
        case K_SHT:return true;
        case '+':
        case '-':
        case '/':
        case '*':
            inputBuffer.clear();
            inputBuffer.append(pBASIC->outputBuffer.left(pBASIC->outputBuffer.indexOf('\n')).trimmed());
            pBASIC->outputBuffer.clear();
//            pBASIC->backupCommandBuffer.clear();
            pBASIC->inputMode = true;
            break;
        case K_LA:
        case K_RA:
            qWarning()<<"EDITOR"<< inputBuffer<<"inList:"<<pBASIC->inLIST;
            if (pBASIC->inLIST) {
                inputBuffer.clear();
                inputBuffer.append(pBASIC->outputBuffer.left(pBASIC->outputBuffer.indexOf('\n')).trimmed());
                pBASIC->outputBuffer.clear();
                if (inputBuffer.indexOf(':')<4) {
                    inputBuffer[inputBuffer.indexOf(':')]=' ';
                }
            }

            pBASIC->inputMode=true;
            pBASIC->outputBuffer.clear();
            break;
        case K_DA:
        case K_UA: if (pBASIC->runMode==CTinyBasic::PRO) pBASIC->inLIST = true;
            else {
                inputBuffer.clear();
                pBASIC->inputMode = true;
                pBASIC->outputBuffer.clear();
            }
            break;
        default: inputBuffer.clear();
            pBASIC->inputMode = true;
            pBASIC->outputBuffer.clear();
            break;
        }

        pBASIC->waitForRTN = false;
        Refresh_Display = true;

    }



    if (pBASIC->inputMode) Editor();

    CpcXXXX::run();

    return true;
}

void Cpc1211::Editor() {

    switch (pKEYB->LastKey) {
    case 0:
    case K_SHT:
    case K_POW_ON:
        pKEYB->LastKey = 0;
        break;
    case K_MOD: pBASIC->switchMode();
        pKEYB->LastKey=0;
        return;
    case K_CLR: inputBuffer.clear();
        pKEYB->LastKey=0;
        break;
    case K_DA:
        if (pBASIC->runMode==CTinyBasic::PRO) {
            inputBuffer.clear();
            if (!pBASIC->inLIST) pBASIC->linenum=0;
            pBASIC->nextStep = CTinyBasic::LIST_NEXT;
            pBASIC->inputMode = false;
            pBASIC->outputBuffer.clear();
        }
        pKEYB->LastKey=0;
        break;
    case K_UA:
        if (pBASIC->runMode==CTinyBasic::PRO) {
            inputBuffer.clear();
            if (!pBASIC->inLIST) pBASIC->linenum=999;
            pBASIC->nextStep = CTinyBasic::LIST_PREV;
            pBASIC->inputMode = false;
            pBASIC->outputBuffer.clear();
        }
        pKEYB->LastKey=0;
        break;
    case K_LA:
        cursorPos--;
        if (cursorPos<0) cursorPos=0;
        pKEYB->LastKey = 0;
        break;
    case K_RA: cursorPos++;
        pKEYB->LastKey = 0;
        break;
    case K_BRK: pBASIC->breakFlag = true; break;
    case K_RET: pBASIC->commandBuffer.clear();
        pBASIC->commandBuffer.append(inputBuffer).append("\n");
        pBASIC->inputMode = false;
//        inputBuffer.clear();
        pKEYB->LastKey = 0;
        break;
    default:

        inputBuffer.append(pKEYB->LastKey);
        qWarning()<<"Input:"<<inputBuffer;
        cursorPos++;
        pKEYB->LastKey = 0;
    }
}

void Cpc1211::afficheChar(quint8 c) {

}

bool Cpc1211::exit()
{
    CpcXXXX::exit();

    return true;
}
