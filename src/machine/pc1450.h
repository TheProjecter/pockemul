#ifndef _PC1450_H_
#define _PC1450_H_

#include <stdlib.h>
#include <stdio.h>


#include "pc1350.h"
#include "sc61860.h"

extern TransMap KeyMap1450[];
extern int KeyMap1450Lenght; 



class Cpc1450:public Cpc1350{						//PC1350 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1450");};


    virtual void	initExtension(void);
    virtual BYTE	Get_PortA(void);

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    UINT8	in(UINT8 address){return(1);};
    UINT8 out(UINT8 address,UINT8 value){return(1);};
	

	Cpc1450(CPObject *parent = 0)	: Cpc1350(this)
	{								//[constructor]
		setfrequency(768000/3);
		setcfgfname("pc1450");

		SessionHeader	= "PC1450PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="pc1450.pkm";

		BackGroundFname	= ":/PC1450/pc1450/pc1450.jpg";
		LcdFname		= ":/PC1450/pc1450/1450lcd.jpg";
		SymbFname		= ":/PC1450/pc1450/1450symb.jpg";
		memsize			= 0x10000;
//		NbSlot		= 3;

		SlotList.clear();
		SlotList.append(CSlot(8 , 0x0000 ,	":/PC1450/pc1450/cpu-1450.rom", "pc-1350/cpu-1450.rom" , ROM , "CPU ROM"));
		SlotList.append(CSlot(24, 0x2000 ,	""						, "pc-1350/R1-1450.ram" , RAM , "RAM"));
		SlotList.append(CSlot(32, 0x8000 ,	":/PC1450/pc1450/bas-1450.rom", "pc-1350/bas-1450.rom" , ROM , "BASIC ROM"));

		KeyMap = KeyMap1450;
		KeyMapLenght = KeyMap1450Lenght;


		
		delete pLCDC;
		pLCDC		= new Clcdc_pc1450(this);

		delete pKEYB;
        pKEYB		= new Ckeyb(this,"pc1450.map",scandef_pc1450);


		Lcd_X		= 130;
		Lcd_Y		= 53;
		Lcd_DX		= 96;
		Lcd_DY		= 10;
		Lcd_ratio_X	= 2;
		Lcd_ratio_Y	= 2;

		Lcd_Symb_X	= 130;
		Lcd_Symb_Y	= 44;
		Lcd_Symb_DX	= 196;
		Lcd_Symb_DY	= 35;
		
        //initExtension();
	}

	~Cpc1450()
	{								//[constructor]
	}
private:
};

#endif
