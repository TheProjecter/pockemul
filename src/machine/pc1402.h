#ifndef _PC1402_H_
#define _PC1402_H_

#include "pc1401.h"

class Cpc1402:public Cpc1401{						//PC1360 emulator main class
public:
	char*	GetClassName(){ return("Cpc1402");};


	bool	Chk_Adr(DWORD *d,DWORD data);
	bool	Chk_Adr_R(DWORD *d,DWORD data);


	Cpc1402(CPObject *parent = 0)	: Cpc1401(this)
	{								//[constructor]
		setcfgfname("pc1402");

		SessionHeader	= "PC1402PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1402.pkm";
		
		BackGroundFname	= ":/PC1402/pc1402/pc1402.jpg";
		LcdFname		= ":/PC1402/pc1402/1402lcd.jpg";
		SymbFname		= ":/PC1402/pc1402/1402symb.jpg";


		memsize			= 0x10000;
//		NbSlot		= 3;
		
		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1402/pc1402/cpu-1402.rom", "pc-1402/cpu-1402.rom" , ROM , "CPU ROM"));
		SlotList.append(CSlot(24, 0x2000 ,	"",								"pc-1402/R1-1402.ram" , ROM , "RAM"));
		SlotList.append(CSlot(32, 0x8000 ,	":/PC1402/pc1402/bas-1402.rom", "pc-1402/bas-1402.rom" , ROM , "BASIC ROM"));

	}

private:
};
#endif
