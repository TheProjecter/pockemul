

#include <QtGui>

#include "common.h"
#include "pb1000.h"
#include "hd61700.h"
#include "hd44352.h"

#include "Inter.h"
#include "init.h"
#include "ct6834.h"
#include "uart.h"
#include "Log.h"
#include "Lcdc_pb1000.h"
#include "Keyb.h"
#include "Connect.h"
#include "dialoganalog.h"


#define PRT_DATA       0x00
#define PRT_ACK        0x01
#define MASK_PRT_DATA  0x20
#define MASK_PRT_ACK   0x40

#define INIT_T6834     0xf0
#define LEC_T6834      0xf1
#define LEC_T6834_ACK  0xf2

#define SIO_GET_PIN(n)		pSio->pSIOCONNECTOR->Get_pin(n)
#define SIO_SET_PIN(n,v)	pSio->pSIOCONNECTOR->Set_pin(n,v)


#define SIO_GNDP	1
#define SIO_SD 		2
#define SIO_RD		3
#define SIO_RS		4
#define SIO_CS		5

#define SIO_GND		7
#define SIO_CD		8
#define SIO_VC1		10
#define SIO_RR		11
#define SIO_PAK		12
#define SIO_VC2		13
#define SIO_ER		14
#define SIO_PRQ		15

Cpb1000::Cpb1000(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 910000/3);
    setcfgfname(QString("pb1000"));

    SessionHeader	= "PB1000PKM";
    SessionHeaderLen= 9;
    Initial_Session_Fname ="pb1000.pkm";

    BackGroundFname	= ":/pb1000/pb-1000.png";
    LcdFname		= ":/pb1000/pb-1000lcd.png";
    SymbFname		= ":/pb1000/pb-1000symb.png";

    memsize		= 0x20000;
    InitMemValue	= 0xff;


    SlotList.clear();
    SlotList.append(CSlot(6 , 0x0000 ,	":/pb1000/rom0.bin" , ""	, ROM , "CPU ROM"));
    SlotList.append(CSlot(8 , 0x6000 ,	""					, ""	, RAM , "RAM0"));
    SlotList.append(CSlot(32, 0x8000 ,	":/pb1000/rom1.bin"	, ""	, ROM , "ROM"));
    SlotList.append(CSlot(32, 0x18000 ,	""					, ""	, RAM , "RAM1"));

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(187);//Pc_DX_mm = 200;
    setDYmm(177);//Pc_DY_mm = 130;
    setDZmm(24);//Pc_DZ_mm = 30;

    setDX(668);//715);
    setDY(633);//465);

    Lcd_X		= 90;
    Lcd_Y		= 130;
    Lcd_DX		= 192;//168;//144 ;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 2;// * 1.18;
    Lcd_ratio_Y	= 2;// * 1.18;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;

    PowerSwitch = 0;

    pLCDC		= new Clcdc_pb1000(this);
    pCPU		= new CHD61700(this);
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"pb1000.map");
    pHD44352    = new CHD44352(this);


//    Cpt = 0;
//    Nb=0;
//    Lec_K7=0;
//    IT_T6834 = 0;

}

bool Cpb1000::init(void)				// initialize
{

#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();

    return true;
}

bool Cpb1000::run() {


    CpcXXXX::run();

}

bool Cpb1000::Chk_Adr(DWORD *d, DWORD data)
{
    if ( (*d>=0x06100) && (*d<=0x061FF) )	{
//        pLCDC->Refresh = true;
        return(true);		// RAM area()
    }
    if ( (*d>=0x06000) && (*d<=0x07FFF) )	return(true);		// RAM area()
    if ( (*d>=0x18000) && (*d<=0x1FFFF) )	return(true);		// RAM area()

    return false;
}

bool Cpb1000::Chk_Adr_R(DWORD *d, DWORD data)
{
    return true;
}

UINT8 Cpb1000::in(UINT8 Port)
{

}

UINT8 Cpb1000::out(UINT8 Port, UINT8 Value)
{

 return 0;
}





void Cpb1000::TurnOFF(void) {
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
    AddLog(LOG_TEMP,"TURN OFF");
}

void Cpb1000::TurnON(void){
    CpcXXXX::TurnON();

    AddLog(LOG_CANON,"TURN ON");

}


void Cpb1000::Reset()
{

    CpcXXXX::Reset();

}

bool Cpb1000::LoadConfig(QXmlStreamReader *xmlIn)
{
//    pT6834->Load_Internal(xmlIn);
    return true;
}

bool Cpb1000::SaveConfig(QXmlStreamWriter *xmlOut)
{
//    pT6834->save_internal(xmlOut);
    return true;
}

