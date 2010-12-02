#ifndef _PC1475_H_
#define _PC1475_H_

#include "pc1360.h"
#include "pc1450.h"
#include "Connect.h"
#include "Keyb.h"

class Cpc1475:public Cpc1360{						//PC1350 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1475");};

	bool Chk_Adr(DWORD *d,DWORD data);
	bool Chk_Adr_R(DWORD *d,DWORD data);

	Cpc1475(CPObject *parent = 0)	: Cpc1360(this)
	{								//[constructor]
		setfrequency(768000/3);
		setcfgfname("pc1475");

		SessionHeader	= "PC1475PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1475.pkm";

		BackGroundFname	= ":/PC1475/pc1475/pc1475.jpg";
		LcdFname		= ":/PC1475/pc1475/1475lcd.jpg";
		SymbFname		= ":/PC1475/pc1475/1475symb.jpg";
		memsize			= 0x40000;
//		NbSlot		= 14;

		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1475/pc1475/cpu-1475.rom"	, "pc-1475/cpu-1475.rom" , ROM , "CPU ROM"));
		SlotList.append(CSlot(8 , 0x2000 ,	""								, "pc-1475/R1-1475.ram" , ROM , "RAM"));
		SlotList.append(CSlot(16, 0x4000 ,	":/PC1475/pc1475/b0-1475.rom"	, "pc-1475/b0-1475.rom" , ROM , "BANK 1"));
		SlotList.append(CSlot(32, 0x8000 ,	""								, "pc-1475/R2-1475.ram" , RAM , "RAM"));
		SlotList.append(CSlot(16, 0x10000 ,	":/PC1475/pc1475/b0-1475.rom"	, "pc-1475/b0-1475.rom" , ROM , "BANK 1"));
		SlotList.append(CSlot(16, 0x14000 ,	":/PC1475/pc1475/b1-1475.rom"	, "pc-1475/b1-1475.rom" , ROM , "BANK 2"));
		SlotList.append(CSlot(16, 0x18000 ,	":/PC1475/pc1475/b2-1475.rom"	, "pc-1475/b2-1475.rom" , ROM , "BANK 3"));
		SlotList.append(CSlot(16, 0x1C000 ,	":/PC1475/pc1475/b3-1475.rom"	, "pc-1475/b3-1475.rom" , ROM , "BANK 4"));
		SlotList.append(CSlot(16, 0x20000 ,	":/PC1475/pc1475/b4-1475.rom"	, "pc-1475/b4-1475.rom" , ROM , "BANK 1"));
		SlotList.append(CSlot(16, 0x24000 ,	":/PC1475/pc1475/b5-1475.rom"	, "pc-1475/b5-1475.rom" , ROM , "BANK 2"));
		SlotList.append(CSlot(16, 0x28000 ,	":/PC1475/pc1475/b6-1475.rom"	, "pc-1475/b6-1475.rom" , ROM , "BANK 3"));
		SlotList.append(CSlot(16, 0x2C000 ,	":/PC1475/pc1475/b7-1475.rom"	, "pc-1475/b7-1475.rom" , ROM , "BANK 4"));
		SlotList.append(CSlot(32, 0x30000 ,	""								, "pc-1475/B0-1475.ram" , RAM , "RAM S1"));
		SlotList.append(CSlot(32, 0x38000 ,	""								, "pc-1475/B1-1475.ram" , RAM , "RAM S2"));

		KeyMap = KeyMap1450;
		KeyMapLenght = KeyMap1450Lenght;

		Pc_DX = 633;
		Pc_DY = 252;
		
		delete pLCDC;		pLCDC		= new Clcdc_pc1475(this);
		delete pKEYB;		pKEYB		= new Ckeyb_pc1450(this);


		Lcd_X		= 111;
		Lcd_Y		= 48;
		Lcd_DX		= 264;
		Lcd_DY		= 30;
        Lcd_ratio_X	= .9;

		Lcd_Symb_X	= 111;
		Lcd_Symb_Y	= 38;
		Lcd_Symb_DX	= 264;
		Lcd_Symb_DY	= 45;
        Lcd_Symb_ratio_X = .9;
	}

	~Cpc1475()
	{								//[constructor]
	}
private:
};

#endif
