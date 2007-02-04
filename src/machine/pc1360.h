#ifndef _PC1360_H_
#define _PC1360_H_

#include <stdlib.h>
#include <stdio.h>

#include "pc1350.h"
#include "Keyb.h"
#include "Connect.h"





class Cpc1360:public Cpc13XX{						//PC1360 emulator main class

public:
	char*	GetClassName(){ return("Cpc1360");};

	BYTE	Get_PortB(void);
//	void	Set_PortB(BYTE data);
	void	Set_PortF(BYTE data);
	void	Set_PortC(BYTE data);
	virtual bool	Chk_Adr(DWORD *d,DWORD data);
	virtual bool	Chk_Adr_R(DWORD *d,DWORD data);

	virtual bool	Set_Connector(void);
	virtual bool	Get_Connector(void);

	void	initExtension(void);

	Cpc1360(CPObject *parent = 0)	: Cpc13XX(this)
	{								//[constructor]
		setcfgfname("pc1360");

		SessionHeader	= "PC1360PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1360.pkm";

		BackGroundFname	= ":/PC1360/pc1360/pc1360.jpg";
		LcdFname		= ":/PC1360/pc1360/1360lcd.jpg";
		SymbFname		= ":/PC1360/pc1360/1360symb.jpg";
		memsize			= 0x40000;
//		NbSlot		= 14;
									    	 
		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1360/pc1360/cpu-1360.rom"	, "pc-1360/cpu-1360.rom", ROM , "CPU ROM"));
		SlotList.append(CSlot(8 , 0x2000 ,	""									, "pc-1360/R1-1360.ram"	, RAM , "SYSTEM RAM"));
		SlotList.append(CSlot(16, 0x4000 ,	":/PC1360/pc1360/B0-1360.rom"	, "pc-1360/B0-1360.rom" , ROM , "CURRENT ROM BANK"));
		SlotList.append(CSlot(32, 0x8000 ,	""									, "pc-1360/R2-1360.ram"	, RAM , "CURRENT RAM SLOT"));
		SlotList.append(CSlot(16, 0x10000 ,	":/PC1360/pc1360/B0-1360.rom"	, "pc-1360/B0-1360.rom" , ROM , "ROM BANK 0"));
		SlotList.append(CSlot(16, 0x14000 ,	":/PC1360/pc1360/B1-1360.rom"	, "pc-1360/B1-1360.rom" , ROM , "ROM BANK 1"));
		SlotList.append(CSlot(16, 0x18000 ,	":/PC1360/pc1360/B2-1360.rom"	, "pc-1360/B2-1360.rom" , ROM , "ROM BANK 2"));
		SlotList.append(CSlot(16, 0x1C000 ,	":/PC1360/pc1360/B3-1360.rom"	, "pc-1360/B3-1360.rom" , ROM , "ROM BANK 3"));
		SlotList.append(CSlot(16, 0x20000 ,	":/PC1360/pc1360/B4-1360.rom"	, "pc-1360/B4-1360.rom" , ROM , "ROM BANK 4"));
		SlotList.append(CSlot(16, 0x24000 ,	":/PC1360/pc1360/B5-1360.rom"	, "pc-1360/B5-1360.rom" , ROM , "ROM BANK 5"));
		SlotList.append(CSlot(16, 0x28000 ,	":/PC1360/pc1360/B6-1360.rom"	, "pc-1360/B6-1360.rom" , ROM , "ROM BANK 6"));
		SlotList.append(CSlot(16, 0x2C000 ,	":/PC1360/pc1360/B7-1360.rom"	, "pc-1360/B7-1360.rom" , ROM , "ROM BANK 7"));
		SlotList.append(CSlot(32, 0x30000 ,	""									, "pc-1360/B0-1360.ram" , RAM , "RAM SLOT 1"));
		SlotList.append(CSlot(32, 0x38000 ,	""									, "pc-1360/B1-1360.ram" , RAM , "RAM SLOT 2"));

		KeyMap = KeyMap1360;				    			 
		KeyMapLenght = KeyMap1360Lenght;
		
		pLCDC		= new Clcdc_pc1360(this);
		pKEYB		= new Ckeyb_pc1360(this);
		pCPU		= new CSC61860(this);
		pTIMER		= new Ctimer(this);

		this->initExtension();
		extensionArray[0] = ext_11pins;
		extensionArray[1] = ext_MemSlot1;
		extensionArray[2] = ext_MemSlot2;
		extensionArray[3] = ext_Serial;
//		InitMemValue = 0xFF;		
	}

	virtual ~Cpc1360()
	{								//[constructor]
	}

private:

};
#endif
