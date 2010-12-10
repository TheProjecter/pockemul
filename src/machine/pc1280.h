#ifndef PC1280_H
#define PC1280_H


#include "pc1360.h"


#include "Connect.h"
#include "Keyb.h"


class Cpc1280:public Cpc1360{						//PC1350 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1280");};

    bool Chk_Adr(DWORD *d,DWORD data);
    bool Chk_Adr_R(DWORD *d,DWORD data);

    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);
    virtual void	initExtension(void);

    Cpc1280(CPObject *parent = 0)	: Cpc1360(this)
    {								//[constructor]
        setfrequency(768000/3);
        setcfgfname("pc1280");

        SessionHeader	= "PC1280PKM";
        SessionHeaderLen= 9;
        Initial_Session_Fname ="pc1280.pkm";

        BackGroundFname	= ":/PC1280/pc1280/pc-1280.jpg";
        LcdFname		= ":/PC1280/pc1280/1280lcd.jpg";
        SymbFname		= ":/PC1280/pc1280/1280symb.jpg";
        memsize			= 0x40000;
//		NbSlot		= 14;

        SlotList.clear();
        SlotList.append(CSlot(8 , 0x0000 ,	":/PC1280/pc1280/cpu-1280.rom"	, "pc-1280/cpu-1280.rom" , ROM , "CPU ROM"));
        SlotList.append(CSlot(8 , 0x2000 ,	""								, "pc-1280/R1-1280.ram" , RAM , "RAM"));
        SlotList.append(CSlot(16, 0x4000 ,	":/PC1280/pc1280/b0-1280.rom"	, "pc-1280/b0-1280.rom" , ROM , "BANK 1"));
        SlotList.append(CSlot(32, 0x8000 ,	""								, "pc-1280/R2-1280.ram" , RAM , "RAM"));
        SlotList.append(CSlot(16, 0x10000 ,	":/PC1280/pc1280/b0-1280.rom"	, "pc-1280/b0-1280.rom" , ROM , "BANK 1"));
        SlotList.append(CSlot(16, 0x14000 ,	":/PC1280/pc1280/b1-1280.rom"	, "pc-1280/b1-1280.rom" , ROM , "BANK 2"));
        SlotList.append(CSlot(16, 0x18000 ,	":/PC1280/pc1280/b2-1280.rom"	, "pc-1280/b2-1280.rom" , ROM , "BANK 3"));
        SlotList.append(CSlot(16, 0x1C000 ,	":/PC1280/pc1280/b3-1280.rom"	, "pc-1280/b3-1280.rom" , ROM , "BANK 4"));
        SlotList.append(CSlot(16, 0x20000 ,	":/PC1280/pc1280/b4-1280.rom"	, "pc-1280/b4-1280.rom" , ROM , "BANK 1"));
        SlotList.append(CSlot(16, 0x24000 ,	":/PC1280/pc1280/b5-1280.rom"	, "pc-1280/b5-1280.rom" , ROM , "BANK 2"));
        SlotList.append(CSlot(16, 0x28000 ,	":/PC1280/pc1280/b6-1280.rom"	, "pc-1280/b6-1280.rom" , ROM , "BANK 3"));
        SlotList.append(CSlot(16, 0x2C000 ,	":/PC1280/pc1280/b7-1280.rom"	, "pc-1280/b7-1280.rom" , ROM , "BANK 4"));
        SlotList.append(CSlot(32, 0x30000 ,	""								, "pc-1280/B0-1280.ram" , RAM , "RAM S1"));
        SlotList.append(CSlot(32, 0x38000 ,	""								, "pc-1280/B1-1280.ram" , RAM , "RAM S2"));

        KeyMap = KeyMap1280;
        KeyMapLenght = KeyMap1280Lenght;

        Pc_DX = 483;
        Pc_DY = 502;

        delete pLCDC;		pLCDC		= new Clcdc_pc1475(this);
        delete pKEYB;		pKEYB		= new Ckeyb_pc1280(this);


        Lcd_X		= 80;
        Lcd_Y		= 48;
        Lcd_DX		= 264;
        Lcd_DY		= 30;
        Lcd_ratio_X	= 1.22;
        Lcd_ratio_Y	= 1.22;

        Lcd_Symb_X	= 80;
        Lcd_Symb_Y	= 40;
        Lcd_Symb_DX	= 264;
        Lcd_Symb_DY	= 48;
        Lcd_Symb_ratio_X = 1.22;
        //Lcd_Symb_ratio_Y = 1.22;
    }

    ~Cpc1280()
    {								//[constructor]
    }
private:
};


#endif // PC1280_H
