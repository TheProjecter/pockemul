
#include "rlh1000.h"
#include "m6502.h"
#include "Inter.h"
#include "Keyb.h"
#include "Connect.h"
#include "Log.h"
#include "Lcdc.h"

Crlh1000::Crlh1000(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 1000000);
    setcfgfname(QString("rlh1000"));

    SessionHeader	= "RLH1000PKM";
    Initial_Session_Fname ="rlh1000.pkm";

    BackGroundFname	= P_RES(":/rlh1000/rlh1000.png");
    LcdFname		= P_RES(":/rlh1000/rlh1000lcd.png");
    SymbFname		= "";

    memsize		= 0x10000;
    InitMemValue	= 0xFF;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	""                                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8 , 0x2000 ,	""                                  , ""	, ROM , "Ext ROM"));
    SlotList.append(CSlot(16, 0x4000 ,	""                                  , ""	, ROM , "ROM Capsules"));
    SlotList.append(CSlot(16, 0x8000 ,	""                                  , ""	, RAM , "Ext RAM"));
    SlotList.append(CSlot(16, 0xC000 ,	P_RES(":/rlh1000/rom-C000-FFFF.bin"), ""	, ROM , "ROM"));


// Ratio = 3,57
    setDXmm(227);//Pc_DX_mm 206
    setDYmm(95);//Pc_DY_mm =83)
    setDZmm(31);//Pc_DZ_mm =30)

    setDX(811);//Pc_DX		= 483;//409;
    setDY(340);//Pc_DY		= 252;//213;

    Lcd_X		= 89;
    Lcd_Y		= 70;
    Lcd_DX		= 240;//168;//144 ;
    Lcd_DY		= 21;
    Lcd_ratio_X	= 2.15;// * 1.18;
    Lcd_ratio_Y	= 2.75;// * 1.18;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    PowerSwitch = 0;

//    pLCDC		= new Clcdc_pc2001(this);
    pCPU		= new Cm6502(this);    m6502 = (Cm6502*)pCPU;
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"rlh1000.map");

    ioFreq = 0;
}

Crlh1000::~Crlh1000() {

}


bool Crlh1000::init(void)				// initialize
{

//pCPU->logsw = true;
#ifndef QT_NO_DEBUG
//    pCPU->logsw = true;
#endif
    CpcXXXX::init();

    pCONNECTOR	= new Cconnector(this,44,0,Cconnector::Panasonic_44,"44 pins connector",true,
                                     QPoint(0,72));
    publish(pCONNECTOR);

    WatchPoint.add(&pCONNECTOR_value,64,44,this,"44 pins connector");
    return true;
}



bool Crlh1000::run() {


    CpcXXXX::run();

    return true;
}

bool Crlh1000::Chk_Adr(DWORD *d, DWORD data)
{

    if(*d <= 0x2000) return true; /* RAM */

    return false;
}

bool Crlh1000::Chk_Adr_R(DWORD *d, DWORD data)
{

    return true;
}


UINT8 Crlh1000::in(UINT8 Port)
{

    return 0;
}



UINT8 Crlh1000::out(UINT8 Port, UINT8 x)
{

    return 0;
}


bool Crlh1000::Set_Connector()
{

    return true;
}

bool Crlh1000::Get_Connector()
{

    return true;
}


void Crlh1000::TurnOFF(void) {
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
    AddLog(LOG_TEMP,"TURN OFF");

}

void Crlh1000::TurnON(void){
    CpcXXXX::TurnON();

}


void Crlh1000::Reset()
{
    CpcXXXX::Reset();
    pLCDC->init();

}

bool Crlh1000::LoadConfig(QXmlStreamReader *xmlIn)
{

    return true;
}

bool Crlh1000::SaveConfig(QXmlStreamWriter *xmlOut)
{

    return true;
}
