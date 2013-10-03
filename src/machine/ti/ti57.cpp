#include "ti57.h"
#include "ti57cpu.h"
#include "Log.h"
#include "Keyb.h"
#include "Inter.h"
#include "Connect.h"



Cti57::Cti57(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 3865000/8);
    setcfgfname(QString("ti57"));

    SessionHeader	= "TI57PKM";
    Initial_Session_Fname ="ti57.pkm";

    BackGroundFname	= P_RES(":/ti57/ti57.png");
    LcdFname		= P_RES("");
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

    Lcd_X		= 89;
    Lcd_Y		= 70;
    Lcd_DX		= 240;//168;//144 ;
    Lcd_DY		= 21;
    Lcd_ratio_X	= 2.15;// * 1.18;
    Lcd_ratio_Y	= 2.75;// * 1.18;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    PowerSwitch = 0;

//    pLCDC		= new Clcdc_pc2001(this);
    pCPU		= new Cti57cpu(this);    ti57cpu = (Cti57cpu*)pCPU;
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"pc2001.map");

    ioFreq = 0;
}

Cti57::~Cti57() {
}

bool Cti57::init(void)				// initialize
{

//pCPU->logsw = true;
#ifndef QT_NO_DEBUG
//    pCPU->logsw = true;
//    if (!fp_log) fp_log=fopen("pc2001.log","wt");	// Open log file
#endif
    CpcXXXX::init();

    pTIMER->resetTimer(1);

    return true;
}



bool Cti57::run() {


    CpcXXXX::run();

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



#define KEY(c)	( pKEYB->keyPressedList.contains(TOUPPER(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(TOLOWER(c)))

UINT16 Cti57::getKey()
{

    UINT16 ks = kstrobe^0xFFFF;
    UINT16 data=0;

    if ((pKEYB->LastKey) && ks )
    {
//        if (fp_log) fprintf(fp_log,"KSTROBE=%04X\n",ks);

        if (ks&0x01) {
//            if (KEY(K_F1))			data|=0x01;
//            if (KEY(K_F2))			data|=0x02;
//            if (KEY(K_SHT))			data|=0x04;
            if (pKEYB->isShift) data|=0x04;
//            if (KEY(K_F4))			data|=0x08;
//            if (KEY(K_F5))			data|=0x10;
//            if (KEY(K_F6))			data|=0x20;
        }

        if (ks&0x02) {
            if (KEY('Q'))			data|=0x01;
            if (KEY('A'))			data|=0x02;
//            if (KEY(K_F1))			data|=0x04;
            if (KEY(K_0))			data|=0x08;
            if (KEY('1'))			data|=0x10;
            if (KEY('Z'))			data|=0x20;
        }
        if (ks&0x04) {
            if (KEY('W'))			data|=0x01;
            if (KEY('S'))			data|=0x02;
            if (KEY('X'))			data|=0x04;
            if (KEY(K_1))			data|=0x08;
            if (KEY('2'))			data|=0x10;
//            if (KEY(K_F1))			data|=0x20; // F1
        }

        if (ks&0x08) {
            if (KEY(','))			data|=0x01;
            if (KEY('D'))			data|=0x02;
            if (KEY('C'))			data|=0x04;
            if (KEY(K_2))			data|=0x08;
            if (KEY('3'))			data|=0x10;
//            if (KEY(K_F2))			data|=0x20;     //F2
        }

        if (ks&0x10) {
            if (KEY('R'))			data|=0x01;
            if (KEY('F'))			data|=0x02;
            if (KEY('V'))			data|=0x04;
            if (KEY(K_3))			data|=0x08;
            if (KEY('4'))			data|=0x10;
//            if (KEY(K_F3))			data|=0x20;     //F3
        }
        if (ks&0x20) {
            if (KEY('T'))			data|=0x01;
            if (KEY('G'))			data|=0x02;
            if (KEY('B'))			data|=0x04;
            if (KEY(K_4))			data|=0x08;
            if (KEY('5'))			data|=0x10;
//            if (KEY(K_F4))			data|=0x20;     //F4
        }
        if (ks&0x40) {
            if (KEY('Y'))			data|=0x01;
            if (KEY('H'))			data|=0x02;
            if (KEY('N'))			data|=0x04;
            if (KEY(K_5))			data|=0x08;
            if (KEY('6'))			data|=0x10;
            if (KEY(','))			data|=0x20;
        }
        if (ks&0x80) {
            if (KEY('U'))			data|=0x01;
            if (KEY('J'))			data|=0x02;
            if (KEY('M'))			data|=0x04;
            if (KEY(K_6))			data|=0x08;
            if (KEY('7'))			data|=0x10;
            if (KEY('.'))			data|=0x20;
        }
        if (ks&0x100) {
            if (KEY('I'))			data|=0x01;
            if (KEY('K'))			data|=0x02;
            if (KEY('/'))			data|=0x04;
            if (KEY(K_7))			data|=0x08;
            if (KEY('8'))			data|=0x10;
            if (KEY('/'))			data|=0x20;
        }
        if (ks&0x200) {
            if (KEY('O'))			data|=0x01;
            if (KEY('L'))			data|=0x02;
            if (KEY('*'))			data|=0x04;
            if (KEY(K_8))			data|=0x08;
            if (KEY('9'))			data|=0x10;
            if (KEY(';'))			data|=0x20;
        }
        if (ks&0x400) {
            if (KEY('P'))			data|=0x01;
//            if (KEY(K_F2))			data|=0x02;
//            if (KEY('-'))			data|=0x04;     // numpad -
            if (KEY(K_9))			data|=0x08;
            if (KEY('0'))			data|=0x10;
            if (KEY(':'))			data|=0x20;
        }
        if (ks&0x800) {
            if (KEY('@'))			data|=0x01;
//            if (KEY(K_F2))			data|=0x02;
            if (KEY('+'))			data|=0x04;
            if (KEY('E'))			data|=0x08;
            if (KEY('-'))			data|=0x10;
            if (KEY(']'))			data|=0x20;
        }
        if (ks&0x1000) {
            if (KEY('^'))			data|=0x01;
            if (KEY(' '))			data|=0x02; //???
//            if (KEY('.'))			data|=0x04; // Numpad .
            if (KEY(K_UA))			data|=0x08;
            if (KEY('['))			data|=0x10;
//            if (KEY(K_F6))			data|=0x20;
        }
        if (ks&0x2000) {
            if (KEY(K_DEL))			data|=0x01;
            if (KEY(K_INS))			data|=0x02;
//            if (KEY(K_UA))			data|=0x04;
            if (KEY(K_DA))			data|=0x08;
            if (KEY(K_LA))			data|=0x10;
            if (KEY(K_RA))			data|=0x20;
        }
        if (ks&0x4000) {
//            if (KEY(K_F1))			data|=0x01;
            if (KEY(K_RET))			data|=0x02;
//            if (KEY(K_F3))			data|=0x04;
            if (KEY(K_SML))			data|=0x08;  // KANA ???
            if (KEY(K_CLR))			data|=0x10; // CLR ???
//            if (KEY(K_F6))			data|=0x20;
        }
//        if (ks&0x8000) {
//            if (KEY(K_F1))			data|=0x01;
//            if (KEY(K_F2))			data|=0x02;
//            if (KEY(K_F3))			data|=0x04;
//            if (KEY(K_F4))			data|=0x08;
//            if (KEY(K_F5))			data|=0x10;
//            if (KEY(K_F6))			data|=0x20;
//        }


//        if (fp_log) fprintf(fp_log,"Read key [%02x]: strobe=%02x result=%02x\n",pKEYB->LastKey,ks,data^0xff);

    }
    return (data^0xff) & 0x3F;

}

