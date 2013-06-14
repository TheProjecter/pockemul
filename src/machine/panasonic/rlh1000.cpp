
#include "rlh1000.h"
#include "m6502.h"
#include "Inter.h"
#include "Keyb.h"
#include "Connect.h"
#include "Log.h"
#include "Lcdc.h"

Crlh1000::Crlh1000(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 1000000);
    setcfgfname(QString("rlh1000"));

    SessionHeader	= "RLH1000PKM";
    Initial_Session_Fname ="rlh1000.pkm";

    BackGroundFname	= P_RES(":/rlh1000/rlh1000.png");
    LcdFname		= P_RES(":/rlh1000/rlh1000lcd.png");
    SymbFname		= "";

    memsize		= 0x20000;
    InitMemValue	= 0x00;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	""                                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8 , 0x2000 ,	""                                  , ""	, ROM , "Ext ROM"));
    SlotList.append(CSlot(16, 0x4000 ,	P_RES(":/rlh1000/SnapBasic.bin")    , ""	, ROM , "ROM Capsules"));
    SlotList.append(CSlot(16, 0x8000 ,	""                                  , ""	, RAM , "Ext RAM"));
    SlotList.append(CSlot(16, 0xC000 ,	P_RES(":/rlh1000/HHC-rom-C000-FFFF.bin"), ""	, ROM , "ROM"));
    SlotList.append(CSlot(16, 0x10000 ,	""                                  , ""	, RAM , "I/O Hard"));

// Ratio = 3,57
    setDXmm(227);
    setDYmm(95);
    setDZmm(31);

    setDX(811);
    setDY(340);

    Lcd_X		= 205;
    Lcd_Y		= 55;
    Lcd_DX		= 159;//168;//144 ;
    Lcd_DY		= 8;
    Lcd_ratio_X	= 2.5;
    Lcd_ratio_Y	= 3;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    PowerSwitch = 0;

    pLCDC		= new Clcdc_rlh1000(this);
    pCPU		= new Cm6502(this);    m6502 = (Cm6502*)pCPU;
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"rlh1000.map");

    ioFreq = 0;
}

Crlh1000::~Crlh1000() {

}


bool Crlh1000::init(void)				// initialize
{

//pCPU->logsw = true;
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();

    pCONNECTOR	= new Cconnector(this,44,0,Cconnector::Panasonic_44,"44 pins connector",false,
                                     QPoint(0,72));
    publish(pCONNECTOR);

    WatchPoint.add(&pCONNECTOR_value,64,44,this,"44 pins connector");

    pTIMER->resetTimer(1);

    latchByte = 0x00;
    return true;
}



bool Crlh1000::run() {

    // 0x58FB = timer (1/256 sec)

#ifndef QT_NO_DEBUG
    if (pCPU->get_PC()==0xc854) m6502->set_PC(0xc856);
#endif

    CpcXXXX::run();



    if (pTIMER->usElapsedId(1)>=3906) {
        mem[0x118FB]++;
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n TIMER: %02X\n",mem[0x118FB]);
        pTIMER->resetTimer(1);
    }

    return true;
}

bool Crlh1000::Chk_Adr(DWORD *d, DWORD data)
{

    if(*d < 0x2000) return true; /* RAM */

    if (*d==0x58FE) {
        latchByte = data;
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n latchByte=%02X\n",data);
        *d+=0xC000;
        return true;
    }

    if((*d>=0x4000) && (*d < 0x7FFF)) {
        if ((latchByte & 0x04)==0) {
            *d+=0xC000;
            if ((*d>=0x11800)&&(*d<0x118A0)) {
                pLCDC->SetDirtyBuf(*d-0x11800);
                return(true);
            }
            return true;
        }
        return false; /* ROM */
    }
    if((*d>=0x8000) && (*d < 0xC000)) return false; /* RAM */

    if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE ROM [%04X]=%02X\n",*d,data);

    return false;
}

bool Crlh1000::Chk_Adr_R(DWORD *d, DWORD data)
{

    if (*d==0xFEE5) {
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n FOUND\n");
    }

    if((*d>=0x4000) && (*d < 0x7FFF)) {
        if ((latchByte & 0x04)==0) *d+=0xC000;
        return true; /* ROM */
    }
    return true;
}


UINT8 Crlh1000::in(UINT8 Port)
{

    return 0;
}



UINT8 Crlh1000::out(UINT8 Port, UINT8 x)
{

    return 0;
}


bool Crlh1000::Set_Connector()
{

    return true;
}

bool Crlh1000::Get_Connector()
{

    return true;
}


void Crlh1000::TurnOFF(void) {
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
    AddLog(LOG_TEMP,"TURN OFF");

}

void Crlh1000::TurnON(void){
    CpcXXXX::TurnON();

    pCPU->Reset();
}


void Crlh1000::Reset()
{
    CpcXXXX::Reset();
    pLCDC->init();

}

bool Crlh1000::LoadConfig(QXmlStreamReader *xmlIn)
{

    return true;
}

bool Crlh1000::SaveConfig(QXmlStreamWriter *xmlOut)
{

    return true;
}
