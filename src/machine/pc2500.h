#ifndef PC2500_H
#define PC2500_H

#include "pc1350.h"
#include "Ce515p.h"

class Cpc2500:public Cpc1350{						//PC1350 emulator main class

public:
    BYTE	Get_PortA(void);
//    BYTE	Get_PortB(void);
//    void	Set_PortB(BYTE data);
    void	Set_PortF(BYTE data);
    virtual BYTE    Get_PC(DWORD adr);
    virtual WORD    Get_16rPC(DWORD adr);

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
//    virtual void	initExtension(void);
    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);
    bool	run(void);				// emulator main
    virtual bool	init(void);

    Cce515p  *pce515p;

    Cpc2500(CPObject *parent = 0)	: Cpc1350(this)
    {								//[constructor]
        setcfgfname("PC2500");

        SessionHeader	= "PC2500PKM";
        SessionHeaderLen= 9;
        Initial_Session_Fname ="pc2500.pkm";

        BackGroundFname	= ":/PC2500/pc2500/pc2500.png";
        LcdFname		= ":/PC1350/pc1350/1350lcd.png";
        SymbFname		= ":/PC1350/pc1350/1350symb.png";
        memsize			= 0x18000;
//		NbSlot		= 3;

        SlotList.clear();
        SlotList.append(CSlot(8 , 0x0000 ,	":/PC2500/pc2500/cpu-2500.bin"	, "pc-2500/cpu-2500.bin"	, ROM , "CPU ROM"));
        SlotList.append(CSlot(24, 0x2000 ,	""								, "pc-2500/R1-2500.ram"		, RAM , "RAM"));
        SlotList.append(CSlot(32, 0x8000 ,	":/PC2500/pc2500/r1-2500.bin"	, "pc-2500/r1-2500.bin"	, ROM , "BAS ROM"));
        SlotList.append(CSlot(32, 0x10000 ,	":/PC2500/pc2500/r2-2500.bin"	, "pc-2500/r2-2500.bin"	, ROM , "BUSINESS ROM"));

        KeyMap		= KeyMap2500;
        KeyMapLenght= KeyMap2500Lenght;

        pLCDC		= new Clcdc_pc2500(this);
        pKEYB		= new Ckeyb_pc2500(this);
        pCPU		= new CSC61860(this);

        pTIMER		= new Ctimer(this);
        pce515p     = new Cce515p(this);
        pce515p->pTIMER = pTIMER;


        this->initExtension();
        extensionArray[0] = ext_11pins;
        extensionArray[1] = ext_MemSlot1;
        extensionArray[2] = ext_Serial;

        Pc_DX_mm = 182;
        Pc_DY_mm = 72;
        Pc_DZ_mm = 16;

        Pc_DX = 960;
        Pc_DY = 673;

        Lcd_X		= 560;
        Lcd_Y		= 70;
        Lcd_DX		= 150;
        Lcd_DY		= 32;
        Lcd_ratio_X	= 2;
        Lcd_ratio_Y	= 2;

        Lcd_Symb_X	= 50;
        Lcd_Symb_Y	= 48;
        Lcd_Symb_DX	= 30;
        Lcd_Symb_DY	= 64;
    }
    virtual ~Cpc2500()
    {								//[constructor]
    }

};

#endif // PC2500_H
