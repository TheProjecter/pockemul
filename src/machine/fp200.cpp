#include <QtGui>
#include <QString>

#include "common.h"
#include "fp200.h"
#include "Lcdc_fp200.h"
#include "i8085.h"

#include "Inter.h"
#include "Keyb.h"

#include "Log.h"



Cfp200::Cfp200(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 6144000/8);
    setcfgfname(QString("fp200"));

    SessionHeader	= "FP200PKM";
    Initial_Session_Fname ="fp200.pkm";

    BackGroundFname	= ":/fp200/fp200.png";
    LcdFname		= ":/fp200/fp200lcd.png";
    SymbFname		= "";

    memsize		= 0x10000;
    InitMemValue	= 0x00;

    SlotList.clear();
    SlotList.append(CSlot(32 , 0x0000 ,	":/fp200/fp200rom.bin"   , ""	, ROM , "ROM"));
    SlotList.append(CSlot(8  , 0x8000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8  , 0xa000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8  , 0xc000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8  , 0xe000 ,	""                  , ""	, RAM , "RAM"));



//    KeyMap		= KeyMapfp200;
//    KeyMapLenght= KeyMapfp200Lenght;


    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(320);
    setDYmm(220);
    setDZmm(55);

    setDX(1034);
    setDY(705);

    Lcd_X		= 158;
    Lcd_Y		= 57;
    Lcd_DX		= 160;//168;//144 ;
    Lcd_DY		= 64;
    Lcd_ratio_X	= 2;// * 1.18;
    Lcd_ratio_Y	= 2;// * 1.18;

    pLCDC		= new Clcdc_fp200(this);
    pCPU		= new Ci8085(this);
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"z1.map");

//    lastKeyBufSize = 0;
//    newKey = false;

//    ioFreq = 0;
    i85cpu = (Ci8085*)pCPU;
}

Cfp200::~Cfp200() {

}



bool Cfp200::Chk_Adr(DWORD *d, DWORD data)
{
    if ( (*d>=0x0000) && (*d<=0x7FFF) )	return(false);		// ROM area(0000-7FFF)
    if ( (*d>=0x8000) && (*d<=0xFFFF) )	{ return(true);	}
}

bool Cfp200::Chk_Adr_R(DWORD *d, DWORD data)
{
    return true;
}


UINT8 Cfp200::in(UINT8 Port)
{
    UINT8 Value=0;
    Clcdc_fp200 *pLcd = (Clcdc_fp200*)pLCDC;

    switch (Port)
     {
      case 0x01 : /* Read 8bits data to LCD left-half */
                Value = pLcd->Read(1);
                AddLog(LOG_CONSOLE,tr("IN [01]=[%1]=%2\n").arg(Value,2,16,QChar('0')).arg(QChar(Value).toAscii()!=0?QChar(Value):QChar(' ')));
                break;
      case 0x02 : /* Read 8bits data to LCD right-half */
                Value = pLcd->Read(2);
                AddLog(LOG_CONSOLE,tr("IN [02]=[%1]=%2\n").arg(Value,2,16,QChar('0')).arg(QChar(Value).toAscii()!=0?QChar(Value):QChar(' ')));
        break;
      case 0x08 : /* Read 6 bits data : */
                Value = (pLcd->Status << 4) | ((pLcd->Y >> 4) & 0x03);
                AddLog(LOG_CONSOLE,tr("IN [08]=[%1]\n").arg(Value,2,16,QChar('0')));
                  break;
      case 0x09: /* D0-D3 for X, D4-D7 for part of Y */
                Value = (pLcd->X & 0x0f) | ((pLcd->Y & 0x0f) <<4);
                AddLog(LOG_CONSOLE,tr("IN [09]=[%1]\n").arg(Value,2,16,QChar('0')));
                break;
    case 0x20: Value = getKey();

        if (pCPU->fp_log) fprintf(pCPU->fp_log,"KS=%02x Val=%02X\n",ks,Value);
        if (Value!=0x00) {
//            if (pCPU->fp_log) fprintf(pCPU->fp_log,"RST7.5");
//            pKEYB->LastKey=0;
//            i85cpu->i8085_set_irq_line(I8085_RST75_LINE,1);
//            AddLog(LOG_CONSOLE,tr("Read Kbd=[%1]   KS=%2\n").arg(Value,2,16,QChar('0')).arg(ks,2,16,QChar('0')));
        }
        else {
//            i85cpu->i8085_set_irq_line(I8085_RST75_LINE,0);
        }
        break;
    }

//     AddLog(LOG_SIO,tr("(%1) In %2,%3").arg(((CZ80*)pCPU)->z80.r16.pc,4,16,QChar('0')).arg(Port,2,16,QChar('0')).arg(Value,2,16,QChar('0')));

     pCPU->imem[Port] = Value;
     return (Value);
}

UINT8 Cfp200::out(UINT8 Port, UINT8 Value)
{
    Clcdc_fp200 *pLcd = (Clcdc_fp200*)pLCDC;

    switch (Port)
     {
      case 0x01 : /* Write 8bits data to LCD left-half */
                pLcd->Write(1,Value);
                AddLog(LOG_CONSOLE,tr("OUT[01]=[%1]=%2\n").arg(Value,2,16,QChar('0')).arg(QChar(Value).toAscii()!=0?QChar(Value):QChar(' ')));
                break;
      case 0x02 : /* Write 8bits data to LCD right-half */
                pLcd->Write(2,Value);
                AddLog(LOG_CONSOLE,tr("OUT[02]=[%1]=%2\n").arg(Value,2,16,QChar('0')).arg(QChar(Value).toAscii()!=0?QChar(Value):QChar(' ')));
                break;
      case 0x08 : /* write 6 bits data : */

                pLcd->Status = (Value >>4) & 0x0f;
                if (pLcd->Status==0x0b) pLcd->Y = (pLcd->Y & 0x0f) | ((Value & 0x03) << 4);
                AddLog(LOG_CONSOLE,tr("OUT[08]=[%1] Status=%2 Y=%3\n").
                       arg(Value,2,16,QChar('0')).
                       arg(pLcd->Status,2,16,QChar('0')).
                       arg(pLcd->Y,2,16,QChar('0')));

                break;
      case 0x09: /* D0-D3 for X, D4-D7 for part of Y */
                    pLcd->X = Value & 0x0f;
                    pLcd->Y = (pLcd->Y & 0x30) | (Value >> 4);
                    AddLog(LOG_CONSOLE,tr("OUT[09]=[%1] X=%2 Y=%3\n").arg(Value,2,16,QChar('0')).
                           arg(pLcd->X,2,16,QChar('0')).
                           arg(pLcd->Y,2,16,QChar('0')));
                    break;
//    case 0x20: i85cpu->i8085_set_irq_line(I8085_RST75_LINE,0);
//        break;
     case 0x21: ks = Value & 0x0f;
//        AddLog(LOG_CONSOLE,tr("KS=[%1]\n").arg(Value,2,16,QChar('0')));
        break;
    }

 return 0;
}

UINT8 Cfp200::in8(UINT16 Port)
{
}



UINT8 Cfp200::out8(UINT16 Port, UINT8 x)
{
}

UINT16 Cfp200::in16(UINT16 Port)
{
}

UINT16 Cfp200::out16(UINT16 Port, UINT16 x)
{
}

bool Cfp200::init()
{

#ifndef QT_NO_DEBUG
    pCPU->logsw = true;

#endif
    CpcXXXX::init();
    off = 0;
    Reset();
    return true;
}

bool Cfp200::run()
{
    if (ks==4) {  // K SWITCH
//        i85cpu->i8085_set_SID(1);
    }
    if (ks==5) {  // CETL BASIC SWITCH
        i85cpu->i8085_set_SID(1);
    }

//    // Check if keybuffer size change
//    if (lastKeyBufSize != pKEYB->keyPressedList.size()) {
//        newKey = true;
//        lastKeyBufSize = pKEYB->keyPressedList.size();
//    }

    if (pKEYB->LastKey>0) {
        i85cpu->i8085_set_irq_line(I8085_RST75_LINE,1);
//        AddLog(LOG_KEYBOARD,"KEY PRESSED");
//        i85cpu->i8085_set_RST75(1);
    }
    else {
        i85cpu->i8085_set_irq_line(I8085_RST75_LINE,0);
    }
    CpcXXXX::run();
}

void Cfp200::Reset()
{
    CpcXXXX::Reset();

//    ((Ci8085 *) pCPU)->i8085_set_pc(0x0000);
}

void Cfp200::TurnON()
{
}

void Cfp200::TurnOFF()
{
}

bool Cfp200::SaveConfig(QXmlStreamWriter *xmlOut)
{
}

bool Cfp200::LoadConfig(QXmlStreamReader *xmlIn)
{
}

#define KEY(c)	( pKEYB->keyPressedList.contains(toupper(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(tolower(c)))
//#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )
BYTE Cfp200::getKey()
{

    UINT8 data=0;

    if ((pKEYB->LastKey))// && ks )
    {
        if (ks==0) {
            if (KEY('7'))			data|=0x10;
            if (KEY('U'))			data|=0x20;
            if (KEY('J'))			data|=0x40;
            if (KEY('M'))			data|=0x80;
        }
        if (ks==1) {
            if (KEY('8'))			data|=0x10;
            if (KEY('I'))			data|=0x20;
            if (KEY('K'))			data|=0x40;
            if (KEY('?'))			data|=0x80;
        }
        if (ks==2) {
            if (KEY('9'))			data|=0x10;
            if (KEY('O'))			data|=0x20;
            if (KEY('L'))			data|=0x40;
            if (KEY('>'))			data|=0x80;
        }
        if (ks==3) {
            if (KEY('0'))			data|=0x10;
            if (KEY('P'))			data|=0x20;
            if (KEY('<'))			data|=0x40;
            if (KEY('+'))			data|=0x80;
        }
        if (ks==4) {
            if (KEY(K_CLR))			data|=0x04;
            if (KEY(K_F0))			data|=0x08;
            if (KEY('1'))			data|=0x10;
            if (KEY('Q'))			data|=0x20;
            if (KEY('A'))			data|=0x40;
            if (KEY('Z'))			data|=0x80;
        }
        if (ks==5) {
            if (KEY(K_RA))			data|=0x04;
            if (KEY(K_F1))			data|=0x08;
            if (KEY('2'))			data|=0x10;
            if (KEY('W'))			data|=0x20;
            if (KEY('S'))			data|=0x40;
            if (KEY('X'))			data|=0x80;
        }
        if (ks==6) {
            if (KEY(K_LA))			data|=0x04;
            if (KEY(K_F2))			data|=0x08;
            if (KEY('3'))			data|=0x10;
            if (KEY('E'))			data|=0x20;
            if (KEY('D'))			data|=0x40;
            if (KEY('C'))			data|=0x80;
        }
        if (ks==7) {
            if (KEY(K_DA))			data|=0x04;
            if (KEY(K_F3))			data|=0x08;
            if (KEY('4'))			data|=0x10;
            if (KEY('R'))			data|=0x20;
            if (KEY('F'))			data|=0x40;
            if (KEY('V'))			data|=0x80;
        }
        if (ks==8)  {
            if (KEY(K_UA))			data|=0x04;
            if (KEY(K_F4))			data|=0x08;
            if (KEY('5'))			data|=0x10;
            if (KEY('T'))           data|=0x20;
            if (KEY('G'))			data|=0x40;
            if (KEY('B'))			data|=0x80;
        }
        if (ks==9) {
            if (KEY(K_RET))			data|=0x01;
            if (KEY(K_SML))			data|=0x04;
            if (KEY('_'))			data|=0x08;
            if (KEY('6'))			data|=0x10;
            if (KEY('Y'))           data|=0x20;
            if (KEY('H'))			data|=0x40;
            if (KEY('N'))			data|=0x80;
        }

//        if (ks==10) {
//            if (KEY(K_SHT2))		data|=0x01;
//            if (KEY(K_SIN))			data|=0x02;
//            if (KEY(K_COS))			data|=0x04;
//            if (KEY(K_LN))			data|=0x08;
//            if (KEY(K_LOG))			data|=0x10;
//            if (KEY(K_TAN))			data|=0x20;
//            if (KEY(K_FSE))			data|=0x40;
//            if (KEY(K_CCE))			data|=0x80;
//        }
//        if (fp_log) fprintf(fp_log,"Read key [%02x]: strobe=%02x result=%02x\n",pKEYB->LastKey,ks,data^0xff);

    }
//                data|=0x20;
    if (data>0) {
        AddLog(LOG_KEYBOARD,tr("KEY PRESSED=%1").arg(data,2,16,QChar('0')));
    }
    return data;//^0xff;

}

void Cfp200::keyReleaseEvent(QKeyEvent *event)
{
    CPObject::keyReleaseEvent(event);
    if (pCPU->fp_log) fprintf(pCPU->fp_log,"KEY RELEASED");
}


void Cfp200::keyPressEvent(QKeyEvent *event)
{
    CPObject::keyPressEvent(event);
    if (pCPU->fp_log) fprintf(pCPU->fp_log,"KEY PRESSED\n");




}
