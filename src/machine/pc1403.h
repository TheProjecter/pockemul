#ifndef _PC1403_H
#define _PC1403_H

#include <stdlib.h>
#include <stdio.h>

#include "pc1401.h"
#include "sc61860.h"
#include "Keyb.h"
#include "Connect.h"

extern TransMap KeyMap1403[];
extern int KeyMap1403Lenght; 

class Cpc1403:public Cpc1401{						//PC1403 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1403");};


	bool	Set_Connector(void);
	bool	Get_Connector(void);


	bool Chk_Adr(DWORD *d,DWORD data);
	bool Chk_Adr_R(DWORD *d,DWORD data);
    UINT8 in(UINT8 address){return(1);};
    UINT8 out(UINT8 address,UINT8 value){return(1);};

	Cpc1403(CPObject *parent = 0)	: Cpc1401(this)
	{											//[constructor]
		setfrequency( (int) 768000/3);
		setcfgfname("pc1403");

		SessionHeader	= "PC1403PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1403.pkm";

		BackGroundFname	= ":/PC1403/pc1403/pc1403.jpg";
		LcdFname		= ":/PC1403/pc1403/1403lcd.jpg";
		SymbFname		= ":/PC1403/pc1403/1403symb.jpg";
		memsize			= 0x20000;
//		NbSlot		= 8;
										   
		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1403/pc1403/cpu-1403.rom"	, "pc-1403/cpu-1403.rom" , ROM , "CPU ROM"));
		SlotList.append(CSlot(8 , 0x2000 ,	""								, "pc-1403/R1-1403.ram" , ROM , "RAM"));
		SlotList.append(CSlot(16, 0x4000 ,	":/PC1403/pc1403/ba1-1403.rom"	, "pc-1403/ba1-1403.rom" , ROM , "BANK 1"));
		SlotList.append(CSlot(32, 0x8000 ,	""								, "pc-1403/R2-1403.ram" , RAM , "RAM"));
		SlotList.append(CSlot(16, 0x10000 ,	":/PC1403/pc1403/ba1-1403.rom"	, "pc-1403/ba1-1403.rom" , ROM , "BANK 1"));
		SlotList.append(CSlot(16, 0x14000 ,	":/PC1403/pc1403/ba2-1403.rom"	, "pc-1403/ba2-1403.rom" , ROM , "BANK 2"));
		SlotList.append(CSlot(16, 0x18000 ,	":/PC1403/pc1403/ba3-1403.rom"	, "pc-1403/ba3-1403.rom" , ROM , "BANK 3"));
		SlotList.append(CSlot(16, 0x1C000 ,	":/PC1403/pc1403/ba4-1403.rom"	, "pc-1403/ba4-1403.rom" , ROM , "BANK 4"));

		RomBank = 0;
		
		KeyMap		= KeyMap1403;
		KeyMapLenght= KeyMap1403Lenght;


		
		delete pLCDC;	pLCDC		= new Clcdc_pc1403(this);
		delete pCPU;	pCPU		= new CSC61860(this);
		delete pTIMER;	pTIMER		= new Ctimer(this);
        delete pCONNECTOR;pCONNECTOR= new Cconnector(this,11,"Connector 11 pins",false);
		delete pKEYB;	pKEYB		= new Ckeyb_pc1403(this);

		Lcd_X	= 116;
		Lcd_Y	= 53;
		Lcd_DX	= 144;
		Lcd_DY	= 11;
		Lcd_ratio_X	= 1.5;
		Lcd_ratio_Y	= 2;

		Lcd_Symb_X	= 119;
		Lcd_Symb_Y	= 44;
		Lcd_Symb_DX	= 210;
		Lcd_Symb_DY	= 35;

	}

	~Cpc1403()
	{								//[constructor]
	}
private:
};

class Cpc1403H:public Cpc1403{						
public:
    const char*	GetClassName(){ return("Cpc1403H");};




	bool Chk_Adr(DWORD *d,DWORD data);
	bool Chk_Adr_R(DWORD *d,DWORD data);

	Cpc1403H(CPObject *parent = 0)	: Cpc1403(this)
	{											//[constructor]
		setfrequency( (int) 768000/3);
		setcfgfname("pc1403H");

		SessionHeader	= "PC1403HPKM";
		SessionHeaderLen= 10;
		Initial_Session_Fname ="pc1403H.pkm";

		BackGroundFname	= ":/PC1403/pc1403/pc1403h.jpg";
		LcdFname		= ":/PC1403/pc1403/1403lcd.jpg";
		SymbFname		= ":/PC1403/pc1403/1403symb.jpg";
		memsize			= 0x20000;
//		NbSlot		= 8;

	}

	~Cpc1403H()
	{								//[constructor]
	}

private:
};




#endif
