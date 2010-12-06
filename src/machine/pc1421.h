#ifndef PC1421_H
#define PC1421_H


#include "pc1401.h"

class Cpc1421:public Cpc1401{						//PC1360 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1421");};


//    bool	Chk_Adr(DWORD *d,DWORD data);
//    bool	Chk_Adr_R(DWORD *d,DWORD data);


    Cpc1421(CPObject *parent = 0)	: Cpc1401(this)
    {								//[constructor]
        setfrequency( (int) 768000/3);
        setcfgfname("pc1421");

        SessionHeader	= "PC1421PKM";
        SessionHeaderLen= 9;
        Initial_Session_Fname ="pc1421.pkm";

        BackGroundFname	= ":/PC1421/pc1421/pc-1421.jpg";
//		LcdFname		= ":/PC1402/pc1402/1402lcd.jpg";
//		SymbFname		= ":/PC1402/pc1402/1402symb.jpg";


        memsize			= 0x10000;
//		NbSlot		= 3;

        SlotList.clear();
        SlotList.append(CSlot(8 , 0x0000 ,	":/PC1421/pc1421/cpu-1421.rom", "pc-1421/cpu-1421.rom" , ROM , "CPU ROM"));
        SlotList.append(CSlot(24, 0x2000 ,	"",								"pc-1421/R1-1421.ram" , ROM , "RAM"));
        SlotList.append(CSlot(32, 0x8000 ,	":/PC1421/pc1421/bas-1421.rom", "pc-1421/bas-1421.rom" , ROM , "BASIC ROM"));

    }

private:
};


#endif // PC1421_H
