#include <QtGui>

#include "common.h"
#include "z1.h"
#include "Inter.h"
#include "Keyb.h"
#include "Log.h"



Cz1::Cz1(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 3840000*3/8);
    setcfgfname(QString("z1"));

    SessionHeader	= "Z1PKM";
    SessionHeaderLen= 5;
    Initial_Session_Fname ="z1.pkm";

    BackGroundFname	= ":/z1/z1gr.png";
    LcdFname		= ":/z1/z1grlcd.png";
    SymbFname		= "";

    memsize		= 0x10000;
    InitMemValue	= 0xff;

    SlotList.clear();
//    SlotList.append(CSlot(8 , 0x0000 ,	""                  , ""	, RAM , "RAM"));
//    SlotList.append(CSlot(8 , 0x2000 ,	""					, ""	, RAM , "Carte RAM 8kb"));
//    SlotList.append(CSlot(8 , 0x4000 ,	""					, ""	, ROM , "Prise ROM 8kb"));
//    SlotList.append(CSlot(8 , 0x6000 ,	""					, ""	, ROM , "ROM Carte 8kb"));
//    SlotList.append(CSlot(6 , 0x8000 ,	""					, ""	, RAM , "Carte RAM 8kb"));
//    SlotList.append(CSlot(4 , 0xA000 ,	""                  , ""	, ROM , "ROM TV"));
//    SlotList.append(CSlot(20, 0xB000 ,	":/x07/rom_xo7.bin" , "x07/rom_xo7.bin" 	, ROM , "BASIC ROM"));

    KeyMap		= KeyMap1250;
    KeyMapLenght= KeyMap1250Lenght;

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(206);//Pc_DX_mm =200 ;
    setDYmm(83);//Pc_DY_mm =130;
    setDZmm(30);//Pc_DZ_mm = 10;

    setDX(736);//Pc_DX		= 483;//409;
    setDY(297);//Pc_DY		= 252;//213;

    Lcd_X		= 67;
    Lcd_Y		= 63;
    Lcd_DX		= 120;//168;//144 ;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 2.4;// * 1.18;
    Lcd_ratio_Y	= 2.8;// * 1.18;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    PowerSwitch = 0;

//    pLCDC		= new Clcdc_x07(this);
//    pCPU		= new CZ80(this);
    pTIMER		= new Ctimer(this);

    pKEYB		= new Ckeyb(this,"x07.map");



}

Cz1::~Cz1() {

}

bool Cz1::init(void)				// initialize
{

#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();



    return true;
}



bool Cz1::run() {


    CpcXXXX::run();

    return true;
}

bool Cz1::Chk_Adr(DWORD *d, DWORD data)
{


    return false;
}

bool Cz1::Chk_Adr_R(DWORD *d, DWORD data)
{
    return true;
}

UINT8 Cz1::in(UINT8 Port)
{

     return (0);
}

UINT8 Cz1::out(UINT8 Port, UINT8 Value)
{



 return 0;
}



void Cz1::TurnOFF(void) {
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
    AddLog(LOG_TEMP,"TURN OFF");

}

void Cz1::TurnON(void){
    CpcXXXX::TurnON();


    AddLog(LOG_CANON,"TURN ON");

}


void Cz1::Reset()
{
    CpcXXXX::Reset();

}

bool Cz1::LoadConfig(QXmlStreamReader *xmlIn)
{

    return true;
}

bool Cz1::SaveConfig(QXmlStreamWriter *xmlOut)
{

    return true;
}

