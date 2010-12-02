#ifndef _PC1260_H_
#define _PC1260_H_

#include "pc1250.h"

class Cpc1260:public Cpc1250{
public:
    const char*	GetClassName(){ return("Cpc1260");};

	BYTE	Get_PortB(void);

	bool	Chk_Adr(DWORD *d,DWORD data);

	Cpc1260(CPObject *parent = 0)	: Cpc1250(this)
	{								//[constructor]
		setfrequency( (int) 768000/3);
		setcfgfname("pc1260");

		SessionHeader	= "PC1260PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1260.pkm";
		
		BackGroundFname	= ":/PC1260/pc1260/pc1260.jpg";
		LcdFname		= ":/PC1260/pc1260/1260lcd.jpg";
		SymbFname		= ":/PC1260/pc1260/1260symb.jpg";


		memsize			= 0x10000;
//		NbSlot		= 3;

		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1260/pc1260/cpu-1260.rom"	, "" , ROM , "CPU ROM"));
		SlotList.append(CSlot(24, 0x2000 ,	""									, "" , RAM , "RAM"));
		SlotList.append(CSlot(32, 0x8000 ,	":/PC1260/pc1260/bas-1260.rom"	, "" , ROM , "BASIC ROM"));

//		Pc_DX		= 409;
//		Pc_DY		= 213;

		Lcd_X		= 79;
		Lcd_Y		= 37;
		Lcd_DX		= 264;
		Lcd_DY		= 30;
		Lcd_ratio_X	= 1.18;
		Lcd_ratio_Y	= 1.18;

		Lcd_Symb_X	= 53;
		Lcd_Symb_Y	= 40;
		Lcd_Symb_DX	= 288;
		Lcd_Symb_DY	= 33;
        Lcd_Symb_ratio_X	= 1.18;

		delete pLCDC;	pLCDC = new Clcdc_pc1260(this);
	}

private:
};

class Cpc1261:public Cpc1260{
public:
    const char*	GetClassName(){ return("Cpc1261");};
	bool	Chk_Adr(DWORD *d,DWORD data);

	Cpc1261(CPObject *parent = 0)	: Cpc1260(this)
	{								//[constructor]
		setfrequency( (int) 768000/3);
		setcfgfname("pc1261");

		SessionHeader	= "PC1261PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1261.pkm";
		
		BackGroundFname	= ":/PC1261/pc1261/pc1261.jpg";
		LcdFname		= ":/PC1261/pc1261/1261lcd.jpg";
//		SymbFname		= "pc-1261\\1261symb.jpg";


		memsize			= 0x10000;
//		NbSlot		= 3;

		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1261/pc1261/cpu-1261.rom"	, "" , ROM , "CPU ROM"));
		SlotList.append(CSlot(24, 0x2000 ,	""									, "" , RAM , "RAM"));
		SlotList.append(CSlot(32, 0x8000 ,	":/PC1261/pc1261/bas-1261.rom"	, "" , ROM , "BASIC ROM"));

	}

private:
};

class Cpc1262:public Cpc1261{						//PC1360 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1262");};


	Cpc1262(CPObject *parent = 0)	: Cpc1261(this)
	{								//[constructor]
		setfrequency( (int) 768000/3);
		setcfgfname("pc1262");

		SessionHeader	= "PC1262PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1262.pkm";
		
		BackGroundFname	= ":/PC1262/pc1262/pc1262.jpg";
		LcdFname		= ":/PC1262/pc1262/1262lcd.jpg";
//		SymbFname		= "pc-1262\\1262symb.jpg";


		memsize			= 0x10000;
//		NbSlot		= 3;

		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1262/pc1262/cpu-1262.rom"	, "" , ROM , "CPU ROM"));
		SlotList.append(CSlot(24, 0x2000 ,	""									, "" , RAM , "RAM"));
		SlotList.append(CSlot(32, 0x8000 ,	":/PC1262/pc1262/bas-1262.rom"	, "" , ROM , "BASIC ROM"));
	}

private:
};
#endif
