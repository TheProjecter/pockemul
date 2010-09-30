#ifndef _PC1350_H
#define _PC1350_H

#include <stdlib.h>
#include <stdio.h>


#include "extension.h"
#include "pcxxxx.h"
#include "sc61860.h"
#include "Keyb.h"
#include "Connect.h"




class Cpc13XX:public CpcXXXX{						//PC1350 emulator main class

public:
    virtual const char*	GetClassName(){ return("Cpc13XX");};

	virtual void	Set_PortA(BYTE data);
	virtual BYTE	Get_PortA(void);
	virtual bool	init(void);


	
    UINT8 in(UINT8 address){return(1);};
    UINT8 out(UINT8 address,UINT8 value){return(1);};
	virtual bool CheckUpdateExtension(CExtension *ext);
	
	Cpc13XX(CPObject *parent = 0)	: CpcXXXX(parent)
	{								//[constructor]
		setfrequency( (int) 768000/3);
		setcfgfname("pc1350");

		Pc_DX_mm = 182;
		Pc_DY_mm = 72;
		Pc_DZ_mm = 16;
		
		Pc_DX = 633;
		Pc_DY = 252;

		Lcd_X		= 75;
		Lcd_Y		= 48;
		Lcd_DX		= 150;
		Lcd_DY		= 32;
		Lcd_ratio_X	= 2;
		Lcd_ratio_Y	= 2;

		Lcd_Symb_X	= 50;
		Lcd_Symb_Y	= 48;
		Lcd_Symb_DX	= 30;
		Lcd_Symb_DY	= 64;
		
        pCONNECTOR	= new Cconnector(this,11,"Connector 11 pins",false);	publish(pCONNECTOR);
        pSIOCONNECTOR	= new Cconnector(this,15,"Connector 15 pins",false);	publish(pSIOCONNECTOR);

	}

	virtual ~Cpc13XX()
	{								//[constructor]
	}
private:
};

class Cpc1350:public Cpc13XX{						//PC1350 emulator main class

public:
	BYTE	Get_PortA(void);
	BYTE	Get_PortB(void);
	void	Set_PortB(BYTE data);
	void	Set_PortF(BYTE data);
		
	virtual bool	Chk_Adr(DWORD *d,DWORD data);
	virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
	virtual void	initExtension(void);
	bool	Set_Connector(void);
	bool	Get_Connector(void);
	bool	run(void);				// emulator main

	Cpc1350(CPObject *parent = 0)	: Cpc13XX(this)
	{								//[constructor]
		setcfgfname("pc1350");

		SessionHeader	= "PC1350PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1350.pkm";

		BackGroundFname	= ":/PC1350/pc1350/pc1350.png";
		LcdFname		= ":/PC1350/pc1350/1350lcd.png";
		SymbFname		= ":/PC1350/pc1350/1350symb.png";
		memsize			= 0x10000;
//		NbSlot		= 3;

		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1350/pc1350/cpu-1350.rom"	, "pc-1350/cpu-1350.rom"	, ROM , "CPU ROM"));
		SlotList.append(CSlot(24, 0x2000 ,	""								, "pc-1350/R1-1350.ram"		, RAM , "RAM"));
		SlotList.append(CSlot(32, 0x8000 ,	":/PC1350/pc1350/bas-1350.rom"	, "pc-1350/bas-1350.rom"	, ROM , "BASIC ROM"));
													  			 
		KeyMap		= KeyMap1350;
		KeyMapLenght= KeyMap1350Lenght;

		pLCDC		= new Clcdc_pc1350(this);
		pKEYB		= new Ckeyb_pc1350(this);
		pCPU		= new CSC61860(this);

		pTIMER		= new Ctimer(this);


		this->initExtension();
		extensionArray[0] = ext_11pins;
		extensionArray[1] = ext_MemSlot1;
		extensionArray[2] = ext_Serial;
		
	}
	virtual ~Cpc1350()
	{								//[constructor]
	}

};
#endif
