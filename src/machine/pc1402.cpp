 
#include	"common.h"

#include "pc1402.h"

Cpc1402::Cpc1402(CPObject *parent)	: Cpc1401(parent)
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
    SlotList.append(CSlot(24, 0x2000 ,	"",								"pc-1402/R1-1402.ram" , RAM , "RAM"));
    SlotList.append(CSlot(32, 0x8000 ,	":/PC1402/pc1402/bas-1402.rom", "pc-1402/bas-1402.rom" , ROM , "BASIC ROM"));

}

bool Cpc1402::Chk_Adr(DWORD *d,DWORD data)
{

	if ( (*d>=0x0000) && (*d<=0x1FFF) )	return(0);			// ROM area(0000-1fff) 
	if ( (*d>=0x8000) && (*d<=0xFFFF) )	return(0);			// ROM area(8000-ffff) 
	if ( (*d>=0x6000) && (*d<=0x67FF) )	{pLCDC->SetDirtyBuf(*d-0x6000);return(1);}
	if ( (*d>=0x2000) && (*d<=0x7FFF) )	return(1);			// RAM area(2000-7fff) 

	return(0);
}

bool Cpc1402::Chk_Adr_R(DWORD *d,DWORD data)
{
	return(1);
}
