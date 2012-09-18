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
    setfrequency( (int) 6144000);
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
    CpcXXXX::run();
}

void Cfp200::Reset()
{
    CpcXXXX::Reset();
    ((Ci8085 *) pCPU)->i8085_set_pc(0x0000);
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

