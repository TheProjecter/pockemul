#include "e500.h"
#include "sc62015.h"
#include "hd61102.h"
#include "Connect.h"
#include "Keyb.h"
#include "Lcdc_e500.h"

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

    memsize		= 0x100000;
    InitMemValue	= 0xff;
    /* ROM area(c0000-fffff) S3: */
    SlotList.clear();
    SlotList.append(CSlot(256, 0x40000 , ""             , ""            , RAM , "RAM S1"));
    SlotList.append(CSlot(256, 0x80000 , ""             , ""            , RAM , "RAM S2"));
    SlotList.append(CSlot(256, 0xC0000 , ":/e500/s3.rom", "e500/s3.rom" , ROM , "ROM S3"));


    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(200);//Pc_DX_mm = 135;
    setDYmm(100);//Pc_DY_mm = 70;
    setDZmm(14);//Pc_DZ_mm = 10;

    setDX(715);//Pc_DX		= 483;//409;
    setDY(357);//Pc_DY		= 252;//213;

    Lcd_X		= 69;
    Lcd_Y		= 99;
    Lcd_DX		= 240;//168;//144 ;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 348.0/240;
    Lcd_ratio_Y	= 60.0/32;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;


    pLCDC		= new Clcdc_e500(this);
    pCPU		= new Csc62015(this);
    pTIMER		= new Ctimer(this);
    //pCONNECTOR	= new Cconnector11(this);		publish(pCONNECTOR);
    pCONNECTOR	= new Cconnector(this,11,0,Cconnector::Sharp_11,"Connector 11 pins",false,QPoint(1,87));		publish(pCONNECTOR);
    pKEYB		= new Ckeyb(this,"e500.map");

    pHD61102_1  = new CHD61102(this);
    pHD61102_2  = new CHD61102(this);

}

bool Ce500::init(void) {
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();

//	if(emsmode!=0) EMS_Load();

    return true;
}

void Ce500::disp(qint8 cmd,DWORD data)
{
    switch(cmd){
    case   6:							/* LCDC 2 write data */
        pHD61102_2->instruction(0x100 | data);
        pLCDC->Refresh = true;
        break;
    case 0xa:							/* LCDC 1 write data */
        pHD61102_1->instruction(0x100 | data);
        pLCDC->Refresh = true;
        break;
    case   4:							/* LCDC 2 inst */
        pHD61102_2->instruction(data);
        pLCDC->Refresh = true;
        break;
    case   8:							/* LCDC 1 inst */
        pHD61102_1->instruction(data);
        pLCDC->Refresh = true;
        break;
    case   0:							/* LCDC 1&2 inst */
        pHD61102_1->instruction(data);
        pHD61102_2->instruction(data);
        pLCDC->Refresh = true;
        break;
    case   2:							/* LCDC 1&2 write data */
        pHD61102_1->instruction(0x100|data);
        pHD61102_2->instruction(0x100|data);
        pLCDC->Refresh = true;
        break;
    case   7:							/* LCDC 2 read data */
        pCPU->set_mem(0x2007,SIZE_8,pHD61102_2->instruction(0x300));
        break;
    case 0xb:							/* LCDC 1 read data */
        pCPU->set_mem(0x200b,SIZE_8,pHD61102_1->instruction(0x300));
        break;
    }
}

/*---------------------------------------------------------------------------*/
/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/
bool Ce500::Chk_Adr(DWORD *d,DWORD data)
{
    if(*d>0xbffff) return(0);			/* ROM area(c0000-fffff) S3: */
    if(*d>0x7ffff) return(1);			/* RAM area(80000-bffff) S1: */
    if(*d>0x3ffff) return(1);			/* RAM area(40000-7ffff) S2: */

    if((*d&0x6000)==0x2000){
        *d&=0x200f; disp(*d&15,data);//lcdc.access=1; lcdc.lcdcadr=*d&15;
        return(1-(*d&1));			/* LCDC (0200x) */
    }

#if 0

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
    if(*d>0xbffff) return(1);			/* ROM area(c0000-fffff) S3: */
    if(*d>0x7ffff) return(1);			/* RAM area(80000-bffff) S1: */
    if(*d>0x3ffff) return(1);			/* RAM area(40000-7ffff) S2: */

    if((*d&0x6000)==0x2000){
        *d&=0x200f; disp(*d&15,data);//pLCDC->SetDirtyBuf(pLCDC->SetDirtyBuf(*d & 15));
        return(1-(*d&1));			/* LCDC (0200x) */
    }
#if 0
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
    CpcXXXX::TurnON();
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
