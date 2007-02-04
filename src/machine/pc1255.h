
#ifndef _PC1255_H_
#define _PC1255_H_

#include "pc1250.h"

class Cpc1255:public Cpc1250{
public:
	char*	GetClassName(){ return("Cpc1255");};


	bool	Chk_Adr(DWORD *d,DWORD data);


	Cpc1255(CPObject *parent = 0)	: Cpc1250(this)
	{								//[constructor]
		setcfgfname("pc1255");

		SessionHeader	= "PC1255PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1255.pkm";
		
		BackGroundFname	= ":/PC1255/pc1255/pc1255.jpg";
		LcdFname		= ":/PC1255/pc1255/1255lcd.jpg";
		SymbFname		= ":/PC1255/pc1255/1255symb.jpg";


		memsize			= 0x10000;
//		NbSlot		= 4;
	
		SlotList.clear();
		SlotList.append(CSlot(8	, 0x0000 ,	":/PC1255/pc1255/cpu-1255.rom"	, "pc1255/cpu-1255.rom"	, ROM , "CPU ROM"));
		SlotList.append(CSlot(8 , 0x2000 ,	""									, "pc1255/R1-1255.ram"	, RAM , "RAM"));
		SlotList.append(CSlot(16, 0x4000 ,	":/PC1255/pc1255/bas-1255.rom"	, "pc1255/bas-1255.rom"	, ROM , "BASIC ROM"));
		SlotList.append(CSlot(32, 0x8000 ,	""									, "pc1255/R2-1255.ram" 	, RAM , "RAM"));

		delete pLCDC; pLCDC		= new Clcdc_pc1255(this);

	}

private:
};

class Cpc1251H:public Cpc1255{
public:
	char*	GetClassName(){ return("Cpc1251H");};


	Cpc1251H(CPObject *parent = 0)	: Cpc1255(this)
	{								//[constructor]
		setcfgfname("pc1251H");

		SessionHeader	= "PC1251HPKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1251H/system.pkm";
		
		BackGroundFname	= ":/PC1251/pc1251/pc1251H.jpg";
	}

private:
};


#endif
