#include	"common.h"

#include "pc1251.h"

Cpc1251::Cpc1251(CPObject *parent)	: Cpc1250(this)
{								//[constructor]
    setcfgfname("pc1251");

    SessionHeader	= "PC1251PKM";
    SessionHeaderLen= 9;
    Initial_Session_Fname ="pc1251.pkm";

    BackGroundFname	= ":/PC1251/pc1251/pc1251.png"; //":/PC1251/pc1251/pc1251hd.jpg"; //
    LcdFname		= ":/PC1251/pc1251/1251lcd.png";
    SymbFname		= ":/PC1251/pc1251/1251symb.png";


    memsize			= 0x10000;
//		NbSlot		= 4;

    SlotList.clear();
    SlotList.append(CSlot(8	, 0x0000 ,	":/PC1251/pc1251/cpu-1251.rom", "pc-1251/cpu-1251.rom", ROM , "CPU ROM"));
    SlotList.append(CSlot(8 , 0x2000 ,	""						, "pc-1251/R1-1251.ram"	, RAM , "RAM"));
    SlotList.append(CSlot(16, 0x4000 ,	":/PC1251/pc1251/bas-1251.rom", "pc-1251/bas-1251.rom", ROM , "BASIC ROM"));
    SlotList.append(CSlot(32, 0x8000 ,	""						, "pc-1251/R2-1251.ram" , RAM , "RAM"));

}

bool Cpc1251::Chk_Adr(DWORD *d,DWORD data)
{

	if ( (*d>=0xB000) && (*d<=0xB7FF) )	{ *d+=0x800;}
	if ( (*d>=0xB800) && (*d<=0xC7FF) )	return(1);		// RAM area(B800-C7ff) 

	return (Cpc1250::Chk_Adr(d,data));
}

bool Cpc1251::Chk_Adr_R(DWORD *d,DWORD data)
{
	if ( (*d>=0xB000) && (*d<=0xB7FF) )	{ *d+=0x800;return(1);	}
	return(Cpc1250::Chk_Adr_R(d,data));
}

