#include "e500.h"
#include "sc62015.h"
#include "Connect.h"
#include "Keyb.h"

#if 1

Ce500::Ce500(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 576000/3);
    setcfgfname(QString("e500"));

    SessionHeader	= "E500PKM";
    SessionHeaderLen= 7;
    Initial_Session_Fname ="e500.pkm";

    BackGroundFname	= ":/e500/pc-e500.png";
    LcdFname		= ":/e500/e500lcd.png";
    SymbFname		= ":/e500/e500symb.png";

    memsize		= 0x10000;

    SlotList.clear();
    SlotList.append(CSlot(8	, 0x0000 ,	":/e500/cpu-1250.rom"	, "pc1250/cpu-1250.rom"	, ROM , "CPU ROM"));
    SlotList.append(CSlot(8 , 0x2000 ,	""								, "pc1250/R1-1250.ram"	, RAM , "RAM"));
    SlotList.append(CSlot(16, 0x4000 ,	":/e500/bas-1250.rom"	, "pc1250/bas-1250.rom"	, ROM , "BASIC ROM"));
    SlotList.append(CSlot(32, 0x8000 ,	""								, "pc1250/R2-1250.ram" 	, RAM , "RAM"));

    KeyMap		= KeyMap1250;
    KeyMapLenght= KeyMap1250Lenght;

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(200);//Pc_DX_mm = 135;
    setDYmm(100);//Pc_DY_mm = 70;
    setDZmm(14);//Pc_DZ_mm = 10;

    setDX(715);//Pc_DX		= 483;//409;
    setDY(357);//Pc_DY		= 252;//213;

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


    //pLCDC		= new Clcdc_e500(this);
    pCPU		= new Csc62015(this);
    pTIMER		= new Ctimer(this);
    //pCONNECTOR	= new Cconnector11(this);		publish(pCONNECTOR);
    pCONNECTOR	= new Cconnector(this,11,0,Cconnector::Sharp_11,"Connector 11 pins",false,QPoint(1,87));		publish(pCONNECTOR);
    pKEYB		= new Ckeyb(this,"e500.map");

}




/*---------------------------------------------------------------------------*/
/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/
bool Ce500::Chk_Adr(DWORD *d,DWORD data)
{
#if 0
    if(*d>0xbffff) return(0);			/* ROM area(c0000-fffff) S3: */
    if(*d>0x7ffff) return(1);			/* RAM area(80000-bffff) S1: */
    if(*d>0x3ffff) return(1);			/* RAM area(40000-7ffff) S2: */
    if(*d>0x1ffff){
//        if(sc.e6) return(0);			/* ROM area(20000-3ffff) ->E650/U6000 */
//        else{
//            *d=BASE_128[GetBank()]+(*d&0x1ffff);
//            return(1-(sc.emsmode>>4));		/* RAM area(20000-3ffff) EMS */
//        }
    }
    if(*d>0x0ffff){
        *d=BASE_64[GetBank()]+(*d&0xffff);
        return(1-(sc.emsmode>>4));			/* RAM area(10000-1ffff) EMS */
    }
    if((*d&0xf000)==0xe000){
        *d&=0xf005; ssfdc.access=1; ssfdc.adr=*d;
        return(1);					/* SSFDC (0e00x) */
    }
    if((*d&0x6000)==0x2000){
        *d&=0x200f; lcdc.access=1; lcdc.lcdcadr=*d&15;
        return(1-(*d&1));			/* LCDC (0200x) */
    }
    if((*d&0x3000)==0x1000){
        *d&=0x103f; rtc.access=1; rtc.adr=*d&31;
        return((*d&0x10)==0);		/* CLOCK (010xx) */
    }
#endif
    return true;
}

/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/
bool Ce500::Chk_Adr_R(DWORD *d,DWORD data)
{
#if 0
    if(*d>0xbffff) return(0);			/* ROM area(c0000-fffff) S3: */
    if(*d>0x7ffff) return(1);			/* RAM area(80000-bffff) S1: */
    if(*d>0x3ffff) return(1);			/* RAM area(40000-7ffff) S2: */
    if(*d>0x1ffff){
        if(sc.e6) return(0);			/* ROM area(20000-3ffff) ->E650/U6000 */
        else{
            *d=BASE_128[GetBank()]+(*d&0x1ffff);
            return(1-(sc.emsmode>>4));		/* RAM area(20000-3ffff) EMS */
        }
    }
    if(*d>0x0ffff){
        *d=BASE_64[GetBank()]+(*d&0xffff);
        return(1-(sc.emsmode>>4));			/* RAM area(10000-1ffff) EMS */
    }
    if((*d&0xf000)==0xe000){
        *d&=0xf005; ssfdc.access=1; ssfdc.adr=*d;
        return(1);					/* SSFDC (0e00x) */
    }
    if((*d&0x6000)==0x2000){
        *d&=0x200f; lcdc.access=1; lcdc.lcdcadr=*d&15;
        return(1-(*d&1));			/* LCDC (0200x) */
    }
    if((*d&0x3000)==0x1000){
        *d&=0x103f; rtc.access=1; rtc.adr=*d&31;
        return((*d&0x10)==0);		/* CLOCK (010xx) */
    }
#endif
    return true;
}



bool Ce500::Set_Connector()
{
}

bool Ce500::Get_Connector()
{
}

BYTE Ce500::Get_PortA()
{
}

BYTE Ce500::Get_PortB()
{
}

void Ce500::TurnON()
{
}

bool Ce500::LoadExtra(QFile *)
{
}

bool Ce500::SaveExtra(QFile *)
{
}

UINT8 Ce500::in(UINT8 address)
{
}

UINT8 Ce500::out(UINT8 address, UINT8 value)
{
}
#endif
