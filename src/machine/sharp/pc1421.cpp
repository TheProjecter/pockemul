
#include "common.h"

#include "pc1421.h"

// Nothing to change. Only ROM differ from 1401

Cpc1421::Cpc1421(CPObject *parent)	: Cpc1401(parent)
{								//[constructor]
    setfrequency( (int) 768000/3);
    setcfgfname("pc1421");

    SessionHeader	= "PC1421PKM";
    Initial_Session_Fname ="pc1421.pkm";

    BackGroundFname	= P_RES(":/pc1421/pc-1421.png");
//		LcdFname		= ":/pc1402/1402lcd.jpng";
//		SymbFname		= ":/pc1402/1402symb.png";


//    memsize			= 0x10000;
//		NbSlot		= 3;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	P_RES(":/pc1421/cpu-1421.rom"), "pc-1421/cpu-1421.rom" , ROM , "CPU ROM"));
    SlotList.append(CSlot(24, 0x2000 ,	"",								"pc-1421/R1-1421.ram" , RAM , "RAM"));
    SlotList.append(CSlot(32, 0x8000 ,	P_RES(":/pc1421/bas-1421.rom"), "pc-1421/bas-1421.rom" , ROM , "BASIC ROM"));

}
