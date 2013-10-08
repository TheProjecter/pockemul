#include <QDebug>

#include "ti57.h"
#include "ti57cpu.h"
#include "Lcdc_ti57.h"
#include "Log.h"
#include "Keyb.h"
#include "Inter.h"
#include "Connect.h"



Cti57::Cti57(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 12500);  // 80µs per cycle
    setcfgfname(QString("ti57"));

    SessionHeader	= "TI57PKM";
    Initial_Session_Fname ="ti57.pkm";

    BackGroundFname	= P_RES(":/ti57/ti57.png");
    LcdFname		= P_RES(":/ti57/ti57lcd.png");
    SymbFname		= "";

    memsize		= 0xFFFF;
    InitMemValue	= 0xFF;

    SlotList.clear();
    SlotList.append(CSlot(4 , 0x0000 ,	P_RES(":/ti57/ti57.bin"), ""	, ROM , "ROM"));
//    SlotList.append(CSlot(16, 0x2000 ,	P_RES(":/pc2001/rom-2000-5FFF.bin"), ""	, ROM , "ROM"));
//    SlotList.append(CSlot(40 ,0x6000 ,	""	, ""	, RAM , "RAM"));


//    KeyMap		= KeyMap1250;
//    KeyMapLenght= KeyMap1250Lenght;

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(78);
    setDYmm(148);
    setDZmm(36);

    setDX(279);
    setDY(529);

    Lcd_X		= 30;
    Lcd_Y		= 43;
    Lcd_DX		= 220;//168;//144 ;
    Lcd_DY		= 40;
    Lcd_ratio_X	= 1;
    Lcd_ratio_Y	= 1;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    PowerSwitch = 0;

    pTIMER		= new Ctimer(this);
    pLCDC		= new Clcdc_ti57(this);
    pCPU		= new Cti57cpu(this);    ti57cpu = (Cti57cpu*)pCPU;
    pKEYB		= new Ckeyb(this,"ti57.map");

    ioFreq = 0;
}

Cti57::~Cti57() {
}

bool Cti57::init(void)				// initialize
{

pCPU->logsw = true;
#ifndef QT_NO_DEBUG
//    pCPU->logsw = true;
//    if (!fp_log) fp_log=fopen("pc2001.log","wt");	// Open log file
#endif
    CpcXXXX::init();


    return true;
}



bool Cti57::run() {

    TI57regs * _regs = ti57cpu->r;

    _regs->KEYR = getKey();

    if ( !_regs->Power && (_regs->KEYR !=0 )) {
        _regs->COND = 1;
        _regs->R5 = _regs->KEYR;
        _regs->Timer=0;
        _regs->Power = true;
        qWarning()<<"Wakeup";

    }


//    if (ti57cpu->r->KEYR != 0) {
//        qWarning() << "KEY:"<<ti57cpu->r->KEYR;
//    }

    CpcXXXX::run();

    pTIMER->state++;
    return true;
}

bool Cti57::Chk_Adr(UINT32 *d, UINT32 data)
{
    if(*d >= 0x6000) return true; /* RAM */

    return false;
}

bool Cti57::Chk_Adr_R(UINT32 *d, UINT32 *data)
{

    return true;
}


UINT8 Cti57::in(UINT8 Port)
{

    return 0;
}



UINT8 Cti57::out(UINT8 Port, UINT8 x)
{

    return 0;
}

bool Cti57::Set_Connector()
{
    return true;
}

bool Cti57::Get_Connector()
{
    return true;
}


void Cti57::TurnOFF(void) {
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
    AddLog(LOG_TEMP,"TURN OFF");

}

void Cti57::TurnON(void){
    CpcXXXX::TurnON();

    ti57cpu->r->Run = true;
    ti57cpu->r->Power = !ti57cpu->r->Power;
    pLCDC->updated = true;
//    Reset();
//    ti57cpu->r->PC = 0x0052;

}


void Cti57::Reset()
{
    CpcXXXX::Reset();

}

bool Cti57::LoadConfig(QXmlStreamReader *xmlIn)
{
    return true;
}

bool Cti57::SaveConfig(QXmlStreamWriter *xmlOut)
{
    return true;
}

QString Cti57::Display() {
  QChar c;
  QString s;

  if (!pCPU) return "";
  s="";
  if (Power)
    for (int i = 11;i >=0; i--) {
        if (ti57cpu->r->RB[i] & 8) c=' ';
        else if (ti57cpu->r->RB[i] & 1) c='-';
        else
            switch(ti57cpu->r->RA[i]) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9: c=QChar(0x30 + ti57cpu->r->RA[i]); break;
            default: qWarning()<<"default:"<<i;c=QChar(0x41-10+ti57cpu->r->RA[i]); break;
            }
        s.append(c);
        if (ti57cpu->r->RB[i] & 2) s.append('.');
    }
  // Check for error
  if (ti57cpu->r->RA[14]==0x0e) {
      // error, blink screen
      s.prepend('E');
  }

  qWarning()<<"DISPLAY:"<<s;
  return s;
}

#define KEY(c)	( pKEYB->keyPressedList.contains(TOUPPER(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(TOLOWER(c)))

UINT8 Cti57::getKey()
{
    UINT8 code = 0;
    if (pKEYB->LastKey)
    {
        if (KEY('0'))			code = 0x27;
        if (KEY('1'))			code = 0x26;
        if (KEY('2'))			code = 0x36;
        if (KEY('3'))			code = 0x46;
        if (KEY('4'))			code = 0x25;
        if (KEY('5'))			code = 0x35;
        if (KEY('6'))			code = 0x45;
        if (KEY('7'))			code = 0x24;
        if (KEY('8'))			code = 0x34;
        if (KEY('9'))			code = 0x44;

        if (KEY('.'))			code = 0x37;
        if (KEY('+'))			code = 0x56;
        if (KEY('('))			code = 51;
        if (KEY(')'))			code = 67;
        if (KEY('-'))			code = 0x55;
        if (KEY('*'))			code = 0x54;
        if (KEY('/'))			code = 0x53;
        if (KEY('='))			code = 0x57;

        if (KEY(K_CCE))			code = 80;  // CLR
        if (KEY(K_LN))			code = 48;
        if (KEY(K_CE))			code = 64;
        if (KEY(K_GTO))			code = 20;
        if (KEY(K_SBR))			code = 21;
        if (KEY(K_RST))			code = 22;
        if (KEY(K_SHT))			code = 0x10;  // 2nd
        if (KEY(K_DEF))			code = 0x20;  // INV
        if (KEY(K_RS))			code = 23;
        if (KEY(K_SIGN))		code = 71;
        if (KEY(K_LRN))			code = 17;
        if (KEY(K_XT))			code = 33;

        if (KEY(K_SQR))			code = 49;
        if (KEY(K_ROOT))		code = 65;
        if (KEY(K_1X))			code = 81;
        if (KEY(K_SST))			code = 18;
        if (KEY(K_STO))			code = 34;
        if (KEY(K_RCL))			code = 50;
        if (KEY(K_SUM))			code = 66;
        if (KEY(K_POT))			code = 82;  // X^Y
        if (KEY(K_BST))			code = 19;
        if (KEY(K_EE))			code = 35;

    }

    return code;

}
#if 0
Keys: array [0..91] of record Key: Char; Shift: TShiftState; Code: Byte end=
({00} (Key: ' '; Shift: []; Code: $17),
 {11} (Key: #189; Shift: []; Code: $55),

 {23} (Key: #190; Shift: []; Code: $37),
 {24} (Key: Chr(VK_Add); Shift: []; Code: $56),
 {25} (Key: #187; Shift: [ssShift]; Code: $56),
 {26} (Key: Chr(VK_Subtract); Shift: []; Code: $55),
 {27} (Key: Chr(VK_Multiply); Shift: []; Code: $54),
 {28} (Key: '8'; Shift: [ssShift]; Code: $54),
 {29} (Key: Chr(VK_Divide); Shift: []; Code: $53),
 {30} (Key: #191; Shift: []; Code: $53),
 {31} (Key: #220; Shift: []; Code: $51),
 {32} (Key: Chr(VK_Return); Shift: []; Code: $57),
 {33} (Key: #187; Shift: []; Code: $57),
 {34} (Key: '9'; Shift: [ssShift]; Code: $33),
 {35} (Key: '0'; Shift: [ssShift]; Code: $43),
 {36} (Key: #219; Shift: []; Code: $00),
 {37} (Key: #221; Shift: []; Code: $00),
 {38} (Key: 'A'; Shift: []; Code: $00),
 {39} (Key: 'B'; Shift: []; Code: $15),
 {40} (Key: 'C'; Shift: []; Code: $00),
 {41} (Key: 'D'; Shift: []; Code: $00),
 {42} (Key: 'E'; Shift: []; Code: $23),
 {43} (Key: 'F'; Shift: []; Code: $00),
 {44} (Key: 'G'; Shift: []; Code: $14),
 {45} (Key: 'H'; Shift: []; Code: $00),
 {46} (Key: 'I'; Shift: []; Code: $00),
 {47} (Key: 'J'; Shift: []; Code: $00),
 {48} (Key: 'K'; Shift: []; Code: $00),
 {49} (Key: 'L'; Shift: []; Code: $11),
 {50} (Key: 'M'; Shift: []; Code: $42),
 {51} (Key: 'N'; Shift: []; Code: $30),
 {52} (Key: 'O'; Shift: []; Code: $00),
 {53} (Key: 'P'; Shift: []; Code: $47),
 {54} (Key: 'Q'; Shift: []; Code: $41),
 {55} (Key: 'R'; Shift: []; Code: $32),
 {56} (Key: 'S'; Shift: []; Code: $22),
 {57} (Key: 'T'; Shift: []; Code: $16),
 {58} (Key: 'U'; Shift: []; Code: $31),
 {59} (Key: 'V'; Shift: []; Code: $00),
 {60} (Key: 'W'; Shift: []; Code: $00),
 {61} (Key: 'X'; Shift: []; Code: $21),
 {62} (Key: 'Y'; Shift: []; Code: $00),
 {63} (Key: 'Z'; Shift: []; Code: $00),
 {64} (Key: #191; Shift: [ssShift]; Code: $00),
 {65} (Key: #188; Shift: []; Code: $37),
 {66} (Key: #186; Shift: []; Code: $00),
 {67} (Key: #222; Shift: []; Code: $00),
 {68} (Key: Chr(VK_Back); Shift: []; Code: $40),
 {69} (Key: Chr(VK_Left); Shift: []; Code: $13),
 {70} (Key: Chr(VK_Right); Shift: []; Code: $12),
 {71} (Key: Chr(VK_F1); Shift: []; Code: $20),
 {72} (Key: Chr(VK_Home); Shift: []; Code: $00),
 {73} (Key: Chr(VK_F2); Shift: []; Code: $10),
 {74} (Key: Chr(VK_Prior); Shift: []; Code: $00),
 {75} (Key: Chr(VK_F3); Shift: []; Code: $00),
 {76} (Key: Chr(VK_Next); Shift: []; Code: $00),
 {77} (Key: Chr(VK_F4); Shift: []; Code: $00),
 {78} (Key: Chr(VK_F5); Shift: []; Code: $00),
 {79} (Key: Chr(VK_F8); Shift: []; Code: $00),
 {80} (Key: Chr(VK_F9); Shift: []; Code: $00),
 {81} (Key: Chr(VK_F10); Shift: []; Code: $00),
 {82} (Key: Chr(VK_F11); Shift: []; Code: $00),
 {83} (Key: Chr(VK_F7); Shift: []; Code: $00),
 {84} (Key: '6'; Shift: [ssShift]; Code: $52),
 {85} (Key: Chr(VK_Delete); Shift: []; Code: $50),
 {86} (Key: Chr(VK_Insert); Shift: []; Code: $20),
 {87} (Key: Chr(VK_Control); Shift: [ssCtrl]; Code: $10),
 {88} (Key: Chr(VK_Up); Shift: []; Code: $13),
 {89} (Key: Chr(VK_Down); Shift: []; Code: $12),
 {90} (Key: 'E'; Shift: [ssShift]; Code: $00),
 {91} (Key: Chr(VK_End); Shift: []; Code: $00)
);
#endif
