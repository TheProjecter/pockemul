#include <QtGui>
#include <QString>

#include "common.h"
#include "fp200.h"
#include "i8085.h"

#include "Inter.h"



Cfp200::Cfp200(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 3865000);
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

//    pLCDC		= new Clcdc_z1(this);
    pCPU		= new Ci8085(this);
    pTIMER		= new Ctimer(this);
//    pHD66108    = new CHD66108(this);
//    pKEYB		= new Ckeyb(this,"z1.map");

//    lastKeyBufSize = 0;
//    newKey = false;

    i85cpu = (Ci8085*)pCPU;
}

Cfp200::~Cfp200() {

}

bool Cfp200::Chk_Adr(DWORD *d, DWORD data)
{
}

bool Cfp200::Chk_Adr_R(DWORD *d, DWORD data)
{
}

UINT8 Cfp200::in(UINT8 address)
{
}

UINT8 Cfp200::out(UINT8 address, UINT8 value)
{
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
    pCPU->logsw = true;
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;

#endif
    return CpcXXXX::init();
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

