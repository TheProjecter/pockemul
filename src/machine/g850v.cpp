
#include "g850v.h"
#include "Connect.h"

#include "z80.h"
#include "sed1560.h"
#include "Inter.h"
#include "Keyb.h"
#include "Lcdc_g850.h"
#include "Log.h"

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
    memsize			= 0x060000;
    InitMemValue	= 0x00;

    SlotList.clear();


    Pc_Offset_X = Pc_Offset_Y = 0;


    setDXmm(196);//Pc_DX_mm = 195;
    setDYmm(95);//Pc_DY_mm = 86;
    setDZmm(20);//Pc_DZ_mm = 25;

    setDX(683);//Pc_DX		= 679; //572;
    setDY(330);//Pc_DY		= 299;//254;

    Lcd_X		= 62;
    Lcd_Y		= 60;
    Lcd_DX		= 144;
    Lcd_DY		= 48;
    Lcd_ratio_X	= 2;
    Lcd_ratio_Y	= 2;

    Lcd_Symb_X	= 64+288;
    Lcd_Symb_Y	= 60;
    Lcd_Symb_DX	= 20;
    Lcd_Symb_DY	= 96;

    SoundOn			= false;

    pLCDC		= new Clcdc_g850(this);
//    pLCDC->Color_Off.setRgb(
//                        (int) (95*pLCDC->contrast),
//                        (int) (119*pLCDC->contrast),
//                        (int) (103*pLCDC->contrast));


    pCPU        = new CZ80(this);
    pTIMER		= new Ctimer(this);

    pCONNECTOR	= new Cconnector(this,11,0,Cconnector::Sharp_11,"Connector 11 pins",false,QPoint(0,60));	publish(pCONNECTOR);
    pSIOCONNECTOR=new Cconnector(this,15,1,Cconnector::Sharp_15,"Connector 15 pins",false,QPoint(679,190));	publish(pSIOCONNECTOR);


    pKEYB		= new Ckeyb(this,"g850v.map");
    pSED1560    = new CSED1560(this);

    Tape_Base_Freq=2500;

    SlotList.clear();
    SlotList.append(CSlot(1, 0x00000 ,	":/G850V/base.bin"          , "" , ROM , "ROM BASE"));
    SlotList.append(CSlot(32, 0x00000 ,	""                          , "" , RAM , "RAM"));

    SlotList.append(CSlot(16, 0x08000 ,	":/G850V/rom00.bin"         , "" , ROM , "ROM BANK 00"));
    SlotList.append(CSlot(16, 0x0C000 ,	":/G850V/rom01.bin"         , "" , ROM , "ROM BANK 01"));

    SlotList.append(CSlot(16, 0x10000 ,	":/G850V/rom02.bin"         , "" , ROM , "ROM BANK 02"));
    SlotList.append(CSlot(16, 0x14000 ,	":/G850V/rom03.bin"         , "" , ROM , "ROM BANK 03"));
    SlotList.append(CSlot(16, 0x18000 ,	":/G850V/rom04.bin"         , "" , ROM , "ROM BANK 04"));
    SlotList.append(CSlot(16, 0x1C000 ,	":/G850V/rom05.bin"         , "" , ROM , "ROM BANK 05"));

    SlotList.append(CSlot(16, 0x20000 ,	":/G850V/rom06.bin"         , "" , ROM , "ROM BANK 06"));
    SlotList.append(CSlot(16, 0x24000 ,	":/G850V/rom07.bin"         , "" , ROM , "ROM BANK 07"));
    SlotList.append(CSlot(16, 0x28000 ,	":/G850V/rom08.bin"         , "" , ROM , "ROM BANK 08"));
    SlotList.append(CSlot(16, 0x2C000 ,	":/G850V/rom09.bin"         , "" , ROM , "ROM BANK 09"));

    SlotList.append(CSlot(16, 0x30000 ,	":/G850V/rom0a.bin"         , "" , ROM , "ROM BANK 0a"));
    SlotList.append(CSlot(16, 0x34000 ,	":/G850V/rom0b.bin"         , "" , ROM , "ROM BANK 0b"));
    SlotList.append(CSlot(16, 0x38000 ,	":/G850V/rom0c.bin"         , "" , ROM , "ROM BANK 0c"));
    SlotList.append(CSlot(16, 0x3C000 ,	":/G850V/rom0d.bin"         , "" , ROM , "ROM BANK 0d"));

    SlotList.append(CSlot(16, 0x40000 ,	":/G850V/rom0e.bin"         , "" , ROM , "ROM BANK 0e"));
    SlotList.append(CSlot(16, 0x44000 ,	":/G850V/rom0f.bin"         , "" , ROM , "ROM BANK 0f"));
    SlotList.append(CSlot(16, 0x48000 ,	":/G850V/rom10.bin"         , "" , ROM , "ROM BANK 10"));
    SlotList.append(CSlot(16, 0x4C000 ,	":/G850V/rom11.bin"         , "" , ROM , "ROM BANK 11"));

    SlotList.append(CSlot(16, 0x50000 ,	":/G850V/rom12.bin"         , "" , ROM , "ROM BANK 12"));
    SlotList.append(CSlot(16, 0x54000 ,	":/G850V/rom13.bin"         , "" , ROM , "ROM BANK 13"));
    SlotList.append(CSlot(16, 0x58000 ,	":/G850V/rom14.bin"         , "" , ROM , "ROM BANK 14"));
    SlotList.append(CSlot(16, 0x5C000 ,	":/G850V/rom15.bin"         , "" , ROM , "ROM BANK 15"));


}

Cg850v::~Cg850v()
{

}


bool Cg850v::init()
{
    // if DEBUG then log CPU
//    pCPU->logsw = true;
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();
    pCPU->init();
    romBank = exBank = ramBank = 0;
    return true;
}


bool Cg850v::Set_Connector()
{

    return true;
}

bool Cg850v::Get_Connector()
{

    return true;
}


void Cg850v::TurnON()
{
    CpcXXXX::TurnON();
    pSED1560->updated = true;
}

void Cg850v::Reset()
{
    pCPU->Reset();
    out( 0x11, 0);
    out( 0x12, 0);
    out( 0x14, 0);
    out( 0x15, 1);
    out( 0x16, 0xff);
    out( 0x17, 0x0f);
    out( 0x18, 0);
    out( 0x19, 0);
    out( 0x1b, 0);
    out( 0x1c, 1);
//    z80.r.im = 1;
    mem[0x790d] = 0;

    mem[0x779c] = (mem[0x779c] >= 0x07 && mem[0x779c] <= 0x1f ? mem[0x779c]: 0x0f);
    out( 0x40, 0x24);
    out( 0x40, mem[0x790d] + 0x40);
    out( 0x40, mem[0x779c] + 0x80);
    out( 0x40, 0xa0);
    out( 0x40, 0xa4);
    out( 0x40, 0xa6);
    out( 0x40, 0xa9);
    out( 0x40, 0xaf);
    out( 0x40, 0xc0);
    out( 0x40, 0x25);
    out( 0x60, 0);
    out( 0x61, 0xff);
    out( 0x62, 0);
    out( 0x64, 0);
    out( 0x65, 1);
    out( 0x66, 1);
    out( 0x67, 0);
    out( 0x6b, 4);
    out( 0x6c, 0);
    out( 0x6d, 0);
    out( 0x6e, 4);

}

bool Cg850v::Mem_Mirror(DWORD *d)
{
    if ( (*d>=0x8000) && (*d<=0xbfff) ) {
        if (exBank != 0) {
//            mem[*d] = 0x2d;
            AddLog(LOG_DISPLAY,"PB");
            *d = 0x8000 + 0x1840;   // Tricks to return 0x2d value :-(
        }
    }
    if ( (*d>=0xc000) && (*d<=0xffff) ) {
        if (romBank < 23) {
            *d += ((romBank-1) * 0x4000);
        }
        else {
//            mem[*d] = 0xff;
            AddLog(LOG_DISPLAY,"PB2");
        }
    }
    return true;
}

bool Cg850v::Chk_Adr(DWORD *d, DWORD data)
{
    Mem_Mirror(d);
    if (*d<=0x7fff) return true;
    return false;
}

bool Cg850v::Chk_Adr_R(DWORD *d, DWORD data)
{
    Mem_Mirror(d);
    return true;
}

UINT8 Cg850v::in(UINT8 address)
{
    switch(address) {
    case 0x10: pCPU->imem[address] = getKey();
        return 0;
    case 0x11:
        return 0;
    case 0x12:
        return 0;
    case 0x13:
        return 0;
    case 0x14:
        return 0;
    case 0x15:
        return 0;
    case 0x16:
        return 0;
    case 0x17:
        return 0;
    case 0x18:
        return 0;
    case 0x19:
        pCPU->imem[address] = ((exBank & 0x07) << 4) | (romBank & 0x0f);
        return 0;
    case 0x1a:
        return 0;
    case 0x1b:
        pCPU->imem[address] = ramBank;
        return 0;
    case 0x1c:
        return 0;
    case 0x1d:
        return 0;
    case 0x1e:
        return 0;
    case 0x1f:
        return 0;
    case 0x40:
    case 0x42:
    case 0x44:
    case 0x46:
    case 0x48:
    case 0x4a:
    case 0x4c:
    case 0x4e:
    case 0x50:
    case 0x52:
    case 0x54:
    case 0x56:
    case 0x58:
    case 0x5a:
    case 0x5c:
    case 0x5e: pCPU->imem[address] = pSED1560->instruction(0x100);
        return 0;
    case 0x41:
    case 0x43:
    case 0x45:
    case 0x47:
    case 0x49:
    case 0x4b:
    case 0x4d:
    case 0x4f:
    case 0x51:
    case 0x53:
    case 0x55:
    case 0x57:
    case 0x59:
    case 0x5b:
    case 0x5d:
    case 0x5f: pCPU->imem[address] = pSED1560->instruction(0x500);
        return 0;
    case 0x6d: pCPU->imem[address] = 0x00;
        return 0;
    case 0x6f: pCPU->imem[address] = 0xff;
        return 0;
    }

    return 0;
}

UINT8 Cg850v::out(UINT8 address, UINT8 value)
{
    switch(address) {
    case 0x11: ks1 = value;
        return 0;
    case 0x12: ks2 = value & 0x03;
        return 0;
    case 0x13:
        return 0;
    case 0x14:
        return 0;
    case 0x15:
        return 0;
    case 0x16:
        return 0;
    case 0x17:
        return 0;
    case 0x18:
        return 0;
    case 0x19:
        romBank = (value & 0x0f);
        exBank = ((value & 0x70) >> 4);
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"BANK SWITCH: %i - %i\n",romBank,exBank);
        return 0;
    case 0x1a:
        return 0;
    case 0x1b:
        ramBank = value & 0x04;
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"RAM BANK SWITCH: %i\n",ramBank);
        return 0;
    case 0x1c:
        return 0;
    case 0x1e:
        return 0;
    case 0x1f:
        return 0;
    case 0x40: pSED1560->instruction(0x200 | value);
        return 0;
    case 0x41: pSED1560->instruction(0x600 | value);
        return 0;
    case 0x69: romBank = value;
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"ROM BANK SWITCH: %i\n",romBank);
        return 0;
    }
}




bool Cg850v::run()
{
    CpcXXXX::run();
}

void Cg850v::Set_Port(PORTS Port, BYTE data)
{
}

BYTE Cg850v::Get_Port(PORTS Port)
{
}


bool Cg850v::LoadConfig(QXmlStreamReader *)
{
}

bool Cg850v::SaveConfig(QXmlStreamWriter *)
{
}

bool Cg850v::InitDisplay()
{
    CpcXXXX::InitDisplay();

    return(1);
}

bool Cg850v::CompleteDisplay()
{
}

#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )
BYTE Cg850v::getKey()
{

    UINT8 data=0;

//    if (pKEYB->LastKey)
//    {
//        BYTE ks1 = pLH5810->GetReg(LH5810_OPB);
//        if (!( ks1 & 0x40)) {
//            if (KEY(K_CTRL))		data|=0x01;
//            if (KEY(K_KBII))		data|=0x02;
//            if (KEY(K_BS))			data|=0x04;
//        }
//    }
    WORD ks = ks1 | (ks2 << 8);
    if ((pKEYB->LastKey) && ks )
    {
        if (ks&1) {
            if (KEY(K_OF))			data|=0x01; //ok
            if (KEY('Q'))			data|=0x02;
            if (KEY('W'))			data|=0x04;
            if (KEY('E'))			data|=0x08; // ok
            if (KEY('R'))			data|=0x10; // ok
            if (KEY('T'))			data|=0x20; // ok
            if (KEY('Y'))			data|=0x40; // ok
            if (KEY('U'))			data|=0x80; // ok
        }
        if (ks&2) {
            if (KEY('A'))			data|=0x01;
            if (KEY('S'))			data|=0x02;
            if (KEY('D'))			data|=0x04;			// OFF
            if (KEY('F'))			data|=0x08;
            if (KEY('G'))			data|=0x10;
            if (KEY('H'))			data|=0x20;
            if (KEY('J'))			data|=0x40;
            if (KEY('K'))			data|=0x80;
        }
        if (ks&4) {
            if (KEY('Z'))			data|=0x01;
            if (KEY('X'))			data|=0x02;
            if (KEY('C'))			data|=0x04;
            if (KEY('V'))			data|=0x08;
            if (KEY('B'))			data|=0x10;
            if (KEY('N'))			data|=0x20;
            if (KEY('M'))			data|=0x40;
            if (KEY(','))			data|=0x80;
        }
        if (ks&8) {
            if (KEY(K_BASIC))		data|=0x01;
            if (KEY(K_TXT))			data|=0x02;
            if (KEY(K_SML))			data|=0x04;
            if (KEY(K_EXTMENU))		data|=0x08;
            if (KEY(K_TAB))			data|=0x10;
            if (KEY(' '))			data|=0x20;
            if (KEY(K_DA))			data|=0x40;
            if (KEY(K_UA))			data|=0x80;
        }
        if (ks&0x10) {
            if (KEY(K_LA))			data|=0x01;			// +
            if (KEY(K_RA))			data|=0x02;			// *
            if (KEY(K_BRK))			data|=0x04;			// /
            if (KEY('O'))			data|=0x08;
            if (KEY('.'))			data|=0x10;			// Key F2
            if (KEY('='))			data|=0x20;
            if (KEY('+'))			data|=0x40;
            if (KEY(K_RET))			data|=0x80;
        }
        if (ks&0x20) {
            if (KEY('L'))			data|=0x01;			// =
            if (KEY(';'))			data|=0x02;			// LEFT ARROW
            if (KEY(K_CON))			data|=0x04;
            if (KEY('1'))			data|=0x08;
            if (KEY('2'))			data|=0x10;
            if (KEY('3'))			data|=0x20;
            if (KEY('-'))			data|=0x40;
            if (KEY(K_MPLUS))		data|=0x80;
        }
        if (ks&0x40) {
            if (KEY('I'))			data|=0x01;			// R ARROW
            if (KEY('O'))			data|=0x02;			// MODE
            if (KEY(K_INS))			data|=0x04;			// CLS
            if (KEY('4'))			data|=0x08;
            if (KEY('5'))			data|=0x10;
            if (KEY('6'))			data|=0x20;
            if (KEY('*'))			data|=0x40;
            if (KEY(K_RM))			data|=0x80;
        }
        if (ks&0x80) {
            if (KEY('P'))			data|=0x01;
            if (KEY(K_BS))			data|=0x02;
            if (KEY(K_PI))			data|=0x04;
            if (KEY('7'))			data|=0x08;
            if (KEY('8'))			data|=0x10;			// Key F4
            if (KEY('9'))			data|=0x20;
            if (KEY('/'))			data|=0x40;
            if (KEY(')'))			data|=0x80;			// DOWN ARROW
        }
        if (ks&0x100) {
            if (KEY(K_NPR))     	data|=0x01;
            if (KEY(K_DEG))			data|=0x02;
            if (KEY(K_ROOT))		data|=0x04;
            if (KEY(K_SQR))			data|=0x08;
            if (KEY(K_POT))			data|=0x10;
            if (KEY('('))			data|=0x20;
            if (KEY(K_1X))			data|=0x40;
            if (KEY(K_MDF))			data|=0x80;
        }

        if (ks&0x200) {
            if (KEY(K_SHT2))		data|=0x01;
            if (KEY(K_SIN))			data|=0x02;
            if (KEY(K_COS))			data|=0x04;
            if (KEY(K_LN))			data|=0x08;
            if (KEY(K_LOG))			data|=0x10;
            if (KEY(K_TAN))			data|=0x20;
            if (KEY(K_FSE))			data|=0x40;
            if (KEY(K_CLR))			data|=0x80;
        }
//        if (fp_log) fprintf(fp_log,"Read key [%02x]: strobe=%02x result=%02x\n",pKEYB->LastKey,ks,data^0xff);
        //SetReg(LH5810_OPA,data^0xff);
    }
    return data;//^0xff;

}
