#ifndef _PC1401_H
#define _PC1401_H

#include <stdlib.h>
#include <stdio.h>


#include "pcxxxx.h"
#include "sc61860.h"
#include "Keyb.h"
#include "Connect.h"

extern TransMap KeyMap1401[];
extern int KeyMap1401Lenght; 

class Cpc1401:public CpcXXXX{						//PC1403 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1401");};

	void	TurnON(void);

	virtual bool	Set_Connector(void);
	virtual bool	Get_Connector(void);
	BYTE	Get_PortA(void);
	void	Set_PortA(BYTE data);
	void	Set_PortB(BYTE data);

	bool	Chk_Adr(DWORD *d,DWORD data);
	bool	Chk_Adr_R(DWORD *d,DWORD data);
    UINT8 in(UINT8 address){return(1);};
    UINT8 out(UINT8 address,UINT8 value){return(1);};
	
	virtual bool CheckUpdateExtension(CExtension *ext);

			
	Cpc1401(CPObject *parent = 0)	: CpcXXXX(this)
	{								//[constructor]
		setfrequency( (int) 576000/3);
		setcfgfname("pc1401");

		SessionHeader	= "PC1401PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1401.pkm";

		BackGroundFname	= ":/PC1401/pc1401/pc1401.jpg";
		LcdFname		= ":/PC1401/pc1401/1401lcd.jpg";
		SymbFname		= ":/PC1401/pc1401/1401symb.jpg";
		memsize			= 0x10000;
//		NbSlot		= 3;
		
		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1401/pc1401/cpu-1401.rom", "pc-1401/cpu-1401.rom" , ROM , "CPU ROM"));
		SlotList.append(CSlot(24, 0x2000 ,	"",								"pc-1401/R1-1401.ram" , ROM , "RAM"));
		SlotList.append(CSlot(32, 0x8000 ,	":/PC1401/pc1401/bas-1401.rom", "pc-1401/bas-1401.rom" , ROM , "BASIC ROM"));

		KeyMap		= KeyMap1401;
		KeyMapLenght= KeyMap1401Lenght;

		Pc_DX_mm = 170;
		Pc_DY_mm = 72;
		Pc_DZ_mm = 10;
		
		Pc_DX = 633;
		Pc_DY = 252;
		
		pLCDC		= new Clcdc_pc1401(this);
		pCPU		= new CSC61860(this);
		pTIMER		= new Ctimer(this);
        pCONNECTOR	= new Cconnector(this,11,"Connector 11 pins",false);	publish(pCONNECTOR);
		pKEYB		= new Ckeyb_pc1401(this);

		Lcd_X	= 119;
		Lcd_Y	= 53;
		Lcd_DX	= 96;//206;
		Lcd_DY	= 7;//21;
		Lcd_ratio_X	= 206/96;
		Lcd_ratio_Y	= 21/7;

		Lcd_Symb_X	= 119;
		Lcd_Symb_Y	= 44;
		Lcd_Symb_DX	= 210;
		Lcd_Symb_DY	= 35;
		
	}

	~Cpc1401()
	{								//[constructor]
	}
private:
};




#endif
