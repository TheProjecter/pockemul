
#include "g850v.h"
#include "Connect.h"

Cg850v::Cg850v(CPObject *parent)	: CpcXXXX(this)
{								//[constructor]
#ifndef QT_NO_DEBUG
    if (!fp_log) fp_log=fopen("g850.log","wt");	// Open log file
#endif

    setfrequency( (int) 8000000);
    ioFreq = 0;
    setcfgfname(QString("g850"));

    SessionHeader	= "G850PKM";
    SessionHeaderLen= 7;
    Initial_Session_Fname ="g850.pkm";
    BackGroundFname	= ":/G850V/g850v.png";
    LcdFname		= ":/G850V/g850vlcd.png";
    SymbFname		= ":/G850V/g850vsymb.png";
    memsize			= 0x020000;
    InitMemValue	= 0x00;

    SlotList.clear();


    Pc_Offset_X = Pc_Offset_Y = 0;


    setDXmm(196);//Pc_DX_mm = 195;
    setDYmm(95);//Pc_DY_mm = 86;
    setDZmm(20);//Pc_DZ_mm = 25;

    setDX(683);//Pc_DX		= 679; //572;
    setDY(330);//Pc_DY		= 299;//254;

    Lcd_X		= 60;
    Lcd_Y		= 50;
    Lcd_DX		= 156;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 2;
    Lcd_ratio_Y	= 2;

    Lcd_Symb_X	= 60;
    Lcd_Symb_Y	= 40;
    Lcd_Symb_DX	= 317;
    Lcd_Symb_DY	= 5;

    SoundOn			= false;

//    pLCDC		= new Clcdc_pc1600(this);
//    pLCDC->Color_Off.setRgb(
//                        (int) (95*pLCDC->contrast),
//                        (int) (119*pLCDC->contrast),
//                        (int) (103*pLCDC->contrast));


    pCPU        = new CZ80(this);
    pTIMER		= new Ctimer(this);

    pCONNECTOR	= new Cconnector(this,11,0,Cconnector::Sharp_11,"Connector 11 pins",false,QPoint(0,60));	publish(pCONNECTOR);
    pSIOCONNECTOR=new Cconnector(this,15,1,Cconnector::Sharp_15,"Connector 15 pins",false,QPoint(679,190));	publish(pSIOCONNECTOR);


    pKEYB		= new Ckeyb(this,"g850.map");

    Tape_Base_Freq=2500;

    SlotList.clear();

    SlotList.append(CSlot(16, 0x08000 ,	":/G850V/rom00.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x0C000 ,	":/G850V/rom01.bin"         , "" , ROM , "ROM"));

    SlotList.append(CSlot(16, 0x10000 ,	":/G850V/rom02.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x14000 ,	":/G850V/rom03.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x18000 ,	":/G850V/rom04.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x1C000 ,	":/G850V/rom05.bin"         , "" , ROM , "ROM"));

    SlotList.append(CSlot(16, 0x10000 ,	":/G850V/rom06.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x14000 ,	":/G850V/rom07.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x18000 ,	":/G850V/rom08.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x1C000 ,	":/G850V/rom09.bin"         , "" , ROM , "ROM"));

    SlotList.append(CSlot(16, 0x10000 ,	":/G850V/rom0a.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x14000 ,	":/G850V/rom0b.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x18000 ,	":/G850V/rom0c.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x1C000 ,	":/G850V/rom0d.bin"         , "" , ROM , "ROM"));

    SlotList.append(CSlot(16, 0x10000 ,	":/G850V/rom0e.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x14000 ,	":/G850V/rom0f.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x18000 ,	":/G850V/rom10.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x1C000 ,	":/G850V/rom11.bin"         , "" , ROM , "ROM"));

    SlotList.append(CSlot(16, 0x10000 ,	":/G850V/rom12.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x14000 ,	":/G850V/rom13.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x18000 ,	":/G850V/rom14.bin"         , "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x1C000 ,	":/G850V/rom15.bin"         , "" , ROM , "ROM"));


}

Cg850v::~Cg85v0()
{

}
