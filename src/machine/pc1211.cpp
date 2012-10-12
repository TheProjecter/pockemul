#include "pc1211.h"
#include "Keyb.h"

Cpc1211::Cpc1211(CPObject *parent)	: CpcXXXX(parent)
{
    setfrequency( (int) 576000/3);
    setcfgfname(QString("pc1211"));

    SessionHeader	= "PC1211PKM";
    Initial_Session_Fname ="pc1211.pkm";

    BackGroundFname	= ":/pc1211/pc1211.png";
    LcdFname		= ":/pc1211/1211lcd.png";
    SymbFname		= ":/pc1211/1211symb.png";

    memsize		= 0x10000;

    KeyMap		= KeyMap1250;
    KeyMapLenght= KeyMap1250Lenght;

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(135);//Pc_DX_mm = 135;
    setDYmm(70);//Pc_DY_mm = 70;
    setDZmm(10);//Pc_DZ_mm = 10;

    setDX(483);//Pc_DX		= 483;//409;
    setDY(252);//Pc_DY		= 252;//213;

    Lcd_X		= 55;
    Lcd_Y		= 49;
    Lcd_DX		= 144;//168;//144 ;
    Lcd_DY		= 8;
    Lcd_ratio_X	= 2 * 1.18;
    Lcd_ratio_Y	= 2 * 1.18;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    pKEYB		= new Ckeyb(this,"pc1250.map");
}

Cpc1211::~Cpc1211()
{
}
