#include "pc2001.h"
#include "upd7907/upd7907.h"
#include "upd16434.h"
#include "Log.h"
#include "Keyb.h"
#include "Inter.h"
#include "Lcdc_pc2001.h"


Cpc2001::Cpc2001(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 3865000/8);
    setcfgfname(QString("pc2001"));

    SessionHeader	= "PC2001PKM";
    Initial_Session_Fname ="pc2001.pkm";

    BackGroundFname	= ":/pc2001/pc-2001.png";
    LcdFname		= ":/pc2001/pc-2001lcd.png";
    SymbFname		= "";

    memsize		= 0xFFFF;
    InitMemValue	= 0xFF;

    SlotList.clear();
    SlotList.append(CSlot(4 , 0x0000 ,	":/pc2001/rom-0000-0FFF.bin", ""	, ROM , "ROM"));
    SlotList.append(CSlot(16, 0x2000 ,	":/pc2001/rom-2000-5FFF.bin", ""	, ROM , "ROM"));
    SlotList.append(CSlot(40 ,0x6000 ,	""	, ""	, RAM , "RAM"));


    KeyMap		= KeyMap1250;
    KeyMapLenght= KeyMap1250Lenght;

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(225);//Pc_DX_mm 206
    setDYmm(129);//Pc_DY_mm =83)
    setDZmm(32);//Pc_DZ_mm =30)

    setDX(804);//Pc_DX		= 483;//409;
    setDY(462);//Pc_DY		= 252;//213;

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

    pLCDC		= new Clcdc_pc2001(this);
    pCPU		= new Cupd7907(this);    upd7810 = (Cupd7907*)pCPU;
    for (int i=0;i<4;i++) upd16434[i]  = new CUPD16434(this);
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"z1.map");



//    i86cpu = (Ci80x86*)pCPU;
}

Cpc2001::~Cpc2001() {
    for (int i=0;i<4;i++) delete(upd16434[i]);
}

bool Cpc2001::init(void)				// initialize
{
    if (!fp_log) fp_log=fopen("pc2001.log","wt");	// Open log file
//pCPU->logsw = true;
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;

#endif
    CpcXXXX::init();

    pTIMER->resetTimer(1);
    return true;
}



bool Cpc2001::run() {


    CpcXXXX::run();

    if (pTIMER->msElapsedId(1)>20) {
        Cupd7907 *upd7810 = (Cupd7907 *)pCPU;
        upd7810->upd7810stat.irr |= 0x10;
        pTIMER->resetTimer(1);
    }

    // LCD transmission
    quint8 data = upd7810->upd7810stat.imem[0x08];
    if ( (data > 0) && (data != 0xff))
    {
        quint8 currentLCDctrl = upd7810->upd7810stat.imem[0] & 0x03;
        quint8 cmddata = (upd7810->upd7810stat.imem[0] >> 2) & 0x01;
        switch(cmddata) {
        case 0x01: upd16434[currentLCDctrl]->instruction(data);
            break;
        case 0x00: upd16434[currentLCDctrl]->data(data);
            break;
        }
        upd7810->upd7810stat.imem[0x08] = 0;
    }

    return true;
}

bool Cpc2001::Chk_Adr(DWORD *d, DWORD data)
{

    if(*d >= 0x6000) return true; /* RAM */

    return false;
}

bool Cpc2001::Chk_Adr_R(DWORD *d, DWORD data)
{

    return true;
}

UINT8 Cpc2001::in(UINT8 Port)
{
    switch (Port) {
    case 0x01 : return 0x02;
    case 0x02 : return getKey();
    }

    return 0;
}



UINT8 Cpc2001::out(UINT8 Port, UINT8 x)
{
    return 0;
}

UINT16 Cpc2001::out16(UINT16 address, UINT16 value)
{
    if (address == UPD7907_PORTE) {
        kstrobe = value;
    }
}


void Cpc2001::TurnOFF(void) {
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
    AddLog(LOG_TEMP,"TURN OFF");

}

void Cpc2001::TurnON(void){
    CpcXXXX::TurnON();

}


void Cpc2001::Reset()
{
    CpcXXXX::Reset();

}

bool Cpc2001::LoadConfig(QXmlStreamReader *xmlIn)
{

    return true;
}

bool Cpc2001::SaveConfig(QXmlStreamWriter *xmlOut)
{

    return true;
}



#define KEY(c)	( pKEYB->keyPressedList.contains(toupper(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(tolower(c)))

UINT16 Cpc2001::getKey()
{

    UINT16 ks = kstrobe^0xFFFF;
    UINT16 data=0;

    if ((pKEYB->LastKey) && ks )
    {
        if (fp_log) fprintf(fp_log,"KSTROBE=%04X\n",ks);

        if (ks&0x01) {
//            if (KEY(K_F1))			data|=0x01;
//            if (KEY(K_F2))			data|=0x02;
            if (KEY(K_SHT))			data|=0x04;
//            if (KEY(K_F4))			data|=0x08;
//            if (KEY(K_F5))			data|=0x10;
//            if (KEY(K_F6))			data|=0x20;
        }

        if (ks&0x02) {
            if (KEY('Q'))			data|=0x01;
            if (KEY('A'))			data|=0x02;
//            if (KEY(K_F3))			data|=0x04;
//            if (KEY('0'))			data|=0x08;
            if (KEY('1'))			data|=0x10;
            if (KEY('Z'))			data|=0x20;
        }
        if (ks&0x04) {
            if (KEY('W'))			data|=0x01;
            if (KEY('S'))			data|=0x02;
            if (KEY('X'))			data|=0x04;
//            if (KEY('1'))			data|=0x08;
            if (KEY('2'))			data|=0x10;
//            if (KEY(K_F1))			data|=0x20; // F1
        }

        if (ks&0x08) {
            if (KEY(','))			data|=0x01;
            if (KEY('D'))			data|=0x02;
            if (KEY('C'))			data|=0x04;
//            if (KEY('2'))			data|=0x08;
            if (KEY('3'))			data|=0x10;
//            if (KEY(K_F2))			data|=0x20;     //F2
        }

        if (ks&0x10) {
            if (KEY('R'))			data|=0x01;
            if (KEY('F'))			data|=0x02;
            if (KEY('V'))			data|=0x04;
//            if (KEY('3'))			data|=0x08;
            if (KEY('4'))			data|=0x10;
//            if (KEY(K_F3))			data|=0x20;     //F3
        }
        if (ks&0x20) {
            if (KEY('T'))			data|=0x01;
            if (KEY('G'))			data|=0x02;
            if (KEY('B'))			data|=0x04;
//            if (KEY('4'))			data|=0x08;
            if (KEY('5'))			data|=0x10;
//            if (KEY(K_F4))			data|=0x20;     //F4
        }
        if (ks&0x40) {
            if (KEY('Y'))			data|=0x01;
            if (KEY('H'))			data|=0x02;
            if (KEY('N'))			data|=0x04;
//            if (KEY('5'))			data|=0x08;
            if (KEY('6'))			data|=0x10;
            if (KEY(','))			data|=0x20;
        }
        if (ks&0x80) {
            if (KEY('U'))			data|=0x01;
            if (KEY('J'))			data|=0x02;
            if (KEY('M'))			data|=0x04;
//            if (KEY('6'))			data|=0x08;
            if (KEY('7'))			data|=0x10;
            if (KEY('.'))			data|=0x20;
        }
        if (ks&0x100) {
            if (KEY('I'))			data|=0x01;
            if (KEY('K'))			data|=0x02;
            if (KEY('/'))			data|=0x04;
//            if (KEY('7'))			data|=0x08;
            if (KEY('8'))			data|=0x10;
            if (KEY('/'))			data|=0x20;
        }
        if (ks&0x200) {
            if (KEY('O'))			data|=0x01;
            if (KEY('L'))			data|=0x02;
            if (KEY('*'))			data|=0x04;
//            if (KEY('8'))			data|=0x08;
            if (KEY('9'))			data|=0x10;
            if (KEY(';'))			data|=0x20;
        }
        if (ks&0x400) {
            if (KEY('P'))			data|=0x01;
//            if (KEY(K_F2))			data|=0x02;
            if (KEY('-'))			data|=0x04;
//            if (KEY('9'))			data|=0x08;
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
            if (KEY('.'))			data|=0x04;
//            if (KEY(K_F2))			data|=0x08;
            if (KEY('['))			data|=0x10;
//            if (KEY(K_F6))			data|=0x20;
        }
        if (ks&0x2000) {
            if (KEY(K_DEL))			data|=0x01;
            if (KEY(K_INS))			data|=0x02;
//            if (KEY(K_F3))			data|=0x04;
            if (KEY(K_CLR))			data|=0x08;
            if (KEY(K_LA))			data|=0x10;
            if (KEY(K_RA))			data|=0x20;
        }
        if (ks&0x4000) {
//            if (KEY(K_F1))			data|=0x01;
            if (KEY(K_RET))			data|=0x02;
//            if (KEY(K_F3))			data|=0x04;
//            if (KEY(K_F4))			data|=0x08;
//            if (KEY(K_F5))			data|=0x10;
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

