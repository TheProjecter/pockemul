#include "common.h"

#include "pc1245.h"

Cpc1245::Cpc1245(CPObject *parent)	: Cpc1250(this)
{								//[constructor]
	setcfgfname("pc1245");

	SessionHeader	= "PC1245PKM";
	SessionHeaderLen= 9;
	Initial_Session_Fname ="pc1245.pkm";
	
	BackGroundFname	= ":/PC1245/pc1245/pc1245.png";
	LcdFname		= ":/PC1245/pc1245/1245lcd.png";
	SymbFname		= ":/PC1245/pc1245/1245symb.png";
	

	memsize			= 0x10000;
//	NbSlot		= 4;

	SlotList.clear();
	SlotList.append(CSlot(8	, 0x0000 ,	":/PC1245/pc1245/cpu-1245.rom"	, "pc1245/cpu-1245.rom"	, ROM , "CPU ROM"));
	SlotList.append(CSlot(8 , 0x2000 ,	""								, "pc1245/R1-1245.ram"	, RAM , "RAM"));
	SlotList.append(CSlot(16, 0x4000 ,	":/PC1245/pc1245/bas-1245.rom"	, "pc1245/bas-1245.rom"	, ROM , "BASIC ROM"));
	SlotList.append(CSlot(32, 0x8000 ,	""								, "pc1245/R2-1245.ram" 	, RAM , "RAM"));

	Lcd_X		= 50;
	Lcd_Y		= 56;
	Lcd_DX		= 96;
	Lcd_DY		= 8;
	Lcd_ratio_X	= (float) 2;
	Lcd_ratio_Y	= (float) 2;

	Lcd_Symb_X	= 50;
	Lcd_Symb_Y	= 45;
	Lcd_Symb_DX	= 173;
	Lcd_Symb_DY	= 5;
	Lcd_Symb_ratio	= 1*1.18;
	
	delete pLCDC;	pLCDC		= new Clcdc_pc1245(this);
}

BYTE	Cpc1245::Get_PortB(void)
{
	int data=IO_B & 0x0f;
	int retval = 0;

	if (KEY(K_MOD) && ( previous_key != K_MOD) )
	{
		switch (PowerSwitch)
		{
		case PS_RUN : PowerSwitch = PS_PRO; break;
		case PS_PRO : PowerSwitch = PS_RUN; break;
		}
	}
	previous_key = pKEYB->LastKey;

	if (IO_B&8)
	{
		switch (PowerSwitch)
		{
		case PS_PRO : retval|=2; break;
		}
	}

	if ( (IO_B&2) && (PowerSwitch==PS_PRO) ) retval|=8;


	int ret = (retval & ~data) | (IO_B & 0xf0);
	return (ret);
}
