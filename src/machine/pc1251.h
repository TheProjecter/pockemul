
#ifndef _PC1251_H_
#define _PC1251_H_

#include "pc1250.h"

class Cpc1251:public Cpc1250{						//PC1360 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1251");};


	bool	Chk_Adr(DWORD *d,DWORD data);
	bool	Chk_Adr_R(DWORD *d,DWORD data);


	Cpc1251(CPObject *parent = 0)	: Cpc1250(this)
	{								//[constructor]
		setcfgfname("pc1251");

		SessionHeader	= "PC1251PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1251.pkm";
		
        BackGroundFname	= ":/PC1251/pc1251/pc1251.png";
		LcdFname		= ":/PC1251/pc1251/1251lcd.jpg";
		SymbFname		= ":/PC1251/pc1251/1251symb.jpg";
		

		memsize			= 0x10000;
//		NbSlot		= 4;

		SlotList.clear();
		SlotList.append(CSlot(8	, 0x0000 ,	":/PC1251/pc1251/cpu-1251.rom", "pc-1251/cpu-1251.rom", ROM , "CPU ROM"));
		SlotList.append(CSlot(8 , 0x2000 ,	""						, "pc-1251/R1-1251.ram"	, RAM , "RAM"));
		SlotList.append(CSlot(16, 0x4000 ,	":/PC1251/pc1251/bas-1251.rom", "pc-1251/bas-1251.rom", ROM , "BASIC ROM"));
		SlotList.append(CSlot(32, 0x8000 ,	""						, "pc-1251/R2-1251.ram" , RAM , "RAM"));

	}

private:
};

class Ctrspc3Ext:public Cpc1251{						//PC1250 emulator main class
public:

    Ctrspc3Ext(CPObject *parent = 0)	: Cpc1251(this)
    {								//[constructor]
        setcfgfname("trspc3ext");

        SessionHeader	= "TRSPC3EXTPKM";
        SessionHeaderLen= 12;
        Initial_Session_Fname ="trspc3ext.pkm";

        BackGroundFname	= ":/PC1250/pc1250/pc31k.png";
    }


};

#endif
