#include <QtGui>
#include <QString>

#include "common.h"
#include "fp200.h"
#include "i8085.h"

Cfp200::Cfp200(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 3865000);
    setcfgfname(QString("fp200"));

    SessionHeader	= "FP200PKM";
    Initial_Session_Fname ="fp200.pkm";

    BackGroundFname	= ":/fp200/fp200.png";
    LcdFname		= ":/fp200/fp200lcd.png";
    SymbFname		= "";

    memsize		= 0xFFFF;
    InitMemValue	= 0x00;

    SlotList.clear();
    SlotList.append(CSlot(32 , 0x0000 ,	":/fp200/fp200rom.bin"   , ""	, ROM , "ROM"));
    SlotList.append(CSlot(8  , 0x8000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8  , 0xa000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8  , 0xc000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8  , 0xe000 ,	""                  , ""	, RAM , "RAM"));



    KeyMap		= KeyMapfp200;
    KeyMapLenght= KeyMapfp200Lenght;


    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(206);//Pc_DX_mm =200 ;
    setDYmm(83);//Pc_DY_mm =130;
    setDZmm(30);//Pc_DZ_mm = 10;

    setDX(736);//Pc_DX		= 483;//409;
    setDY(297);//Pc_DY		= 252;//213;

    Lcd_X		= 77;
    Lcd_Y		= 44;
    Lcd_DX		= 192;//168;//144 ;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 2;// * 1.18;
    Lcd_ratio_Y	= 2;// * 1.18;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    PowerSwitch = 0;

//    pLCDC		= new Clcdc_z1(this);
    pCPU		= new Ci8085(this);
    pTIMER		= new Ctimer(this);
//    pHD66108    = new CHD66108(this);
//    pKEYB		= new Ckeyb(this,"z1.map");

    lastKeyBufSize = 0;
    newKey = false;

    i85cpu = (Ci8085*)pCPU;
}

Cfp200::~Cfp200() {

}

