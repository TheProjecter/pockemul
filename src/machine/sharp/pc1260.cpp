#include	"common.h"
 
#include "pc1260.h"
#include "Lcdc.h"
#include "Keyb.h"

Cpc1260::Cpc1260(CPObject *parent)	: Cpc1250(this)
{								//[constructor]
    setfrequency( (int) 768000/3);
    setcfgfname("pc1260");

    SessionHeader	= "PC1260PKM";
    Initial_Session_Fname ="pc1260.pkm";

    BackGroundFname	= P_RES(":/pc1260/pc1260.png");
    LcdFname		= P_RES(":/pc1260/1260lcd.png");
    SymbFname		= P_RES(":/pc1260/1260symb.png");

    memsize			= 0x10000;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	P_RES(":/pc1260/cpu-1260.rom")	, "" , ROM , "CPU ROM"));
    SlotList.append(CSlot(24, 0x2000 ,	""									, "" , RAM , "RAM"));
    SlotList.append(CSlot(32, 0x8000 ,	P_RES(":/pc1260/bas-1260.rom")	, "" , ROM , "BASIC ROM"));

    Lcd_X		= 79;
    Lcd_Y		= 37;
    Lcd_DX		= 312;
    Lcd_DY		= 30;
    Lcd_ratio_X	= 1;//1.18;
    Lcd_ratio_Y	= 1;//1.18;

    Lcd_Symb_X	= 53;
    Lcd_Symb_Y	= 40;
    Lcd_Symb_DX	= 288;
    Lcd_Symb_DY	= 33;
    Lcd_Symb_ratio_X	= 1;//1.18;

    delete pLCDC;	pLCDC = new Clcdc_pc1260(this);
    pKEYB->fn_KeyMap = "pc1260.map";
}

BYTE	Cpc1260::Get_PortB(void)
{
	BYTE loc;
	loc = Cpc1250::Get_PortB();
	
	if (! Japan){
		if (IO_B & 4) loc = loc | 8;
	}
		
	return loc;
}

bool Cpc1260::Chk_Adr(DWORD *d,DWORD data)
{
	if ( (*d>=0x0000) && (*d<=0x1FFF) )	return(0);				// ROM area(0000-1fff) 
	if ( (*d>=0x5800) && (*d<=0x6800) )	return(1);				// ROM area(8000-ffff) 
	if ( (*d>=0x8000) && (*d<=0xFFFF) )	return(0);				// ROM area(8000-ffff) 
	if ( (*d>=0x2000) && (*d<=0x207D) )	{ pLCDC->SetDirtyBuf(*d-0x2000); return(1); }	/* LCDC (0200x) */
	if ( (*d>=0x2800) && (*d<=0x287D) )	{ pLCDC->SetDirtyBuf(*d-0x2000); return(1);	}	/* LCDC (0200x) */
	if ( (*d>=0x2000) && (*d<=0x20FF) )	return(1);				// ROM area(8000-ffff) 
	if ( (*d>=0x2800) && (*d<=0x28FF) )	return(1);				// ROM area(8000-ffff) 

	return(0);
}

Cpc1261::Cpc1261(CPObject *parent)	: Cpc1260(this)
{								//[constructor]
    setfrequency( (int) 768000/3);
    setcfgfname("pc1261");

    SessionHeader	= "PC1261PKM";
    Initial_Session_Fname ="pc1261.pkm";

    BackGroundFname	= P_RES(":/pc1261/pc1261.png");
    LcdFname		= P_RES(":/pc1261/1261lcd.png");
//		SymbFname		= "pc-1261\\1261symb.png";


    memsize			= 0x10000;
//		NbSlot		= 3;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	P_RES(":/pc1261/cpu-1261.rom")	, "" , ROM , "CPU ROM"));
    SlotList.append(CSlot(24, 0x2000 ,	""									, "" , RAM , "RAM"));
    SlotList.append(CSlot(32, 0x8000 ,	P_RES(":/pc1261/bas-1261.rom")	, "" , ROM , "BASIC ROM"));

}
bool Cpc1261::Chk_Adr(DWORD *d,DWORD data)
{
	if ( (*d>=0x4000) && (*d<=0x6800) )	return(1);				// RAM area(4000-6800) 

	return(Cpc1260::Chk_Adr(d,data));
}

Cpc1262::Cpc1262(CPObject *parent)	: Cpc1261(this)
{								//[constructor]
    setfrequency( (int) 768000/3);
    setcfgfname("pc1262");

    SessionHeader	= "PC1262PKM";
    Initial_Session_Fname ="pc1262.pkm";

    BackGroundFname	= P_RES(":/pc1262/pc1262.png");
    LcdFname		= P_RES(":/pc1262/1262lcd.png");
//		SymbFname		= "pc-1262\\1262symb.png";


    memsize			= 0x10000;
//		NbSlot		= 3;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	P_RES(":/pc1262/cpu-1262.rom")	, "" , ROM , "CPU ROM"));
    SlotList.append(CSlot(24, 0x2000 ,	""									, "" , RAM , "RAM"));
    SlotList.append(CSlot(32, 0x8000 ,	P_RES(":/pc1262/bas-1262.rom")	, "" , ROM , "BASIC ROM"));
}


