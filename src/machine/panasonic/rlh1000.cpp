
#include "rlh1000.h"
#include "m6502/m6502.h"
#include "Inter.h"
#include "Keyb.h"
#include "Connect.h"
#include "Log.h"
#include "Lcdc_rlh1000.h"

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
    InitMemValue	= 0xff;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	""                                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8 , 0x2000 ,	""                                  , ""	, ROM , "Ext ROM"));
    SlotList.append(CSlot(16, 0x4000 ,	P_RES(":/rlh1000/SnapBasic.bin")    , ""	, ROM , "ROM Capsules 1"));
    SlotList.append(CSlot(16, 0x8000 ,	""                                  , ""	, RAM , "Ext RAM"));
    SlotList.append(CSlot(16, 0xC000 ,	P_RES(":/rlh1000/HHC-rom-C000-FFFF.bin"), ""	, ROM , "ROM"));
    SlotList.append(CSlot(16, 0x10000 ,	""                                  , ""	, RAM , "I/O Hard"));
    SlotList.append(CSlot(16, 0x14000 ,	P_RES(":/rlh1000/HHCbasic.bin")    , ""	, ROM , "ROM Capsules 2"));
    SlotList.append(CSlot(16, 0x18000 ,	P_RES(":/rlh1000/SnapForth.bin")    , ""	, ROM , "ROM Capsules 3"));

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

    timercnt1=timercnt2=timercnt3=0;
    return true;
}



bool Crlh1000::run() {

    // 0x5802 : Shift 0x80   ???
    // 0x5820 : 2nd 0x80     ???
    // timercnt = timer (1/256 sec)



#if 0 //ndef QT_NO_DEBUG
    if (pCPU->get_PC()==0xc854) m6502->set_PC(0xc856);
#endif
//    mem[0x204]=1;
    CpcXXXX::run();

    if (pKEYB->LastKey>0) {
//        if ((IA & 0x80) != 0)
        {
//            qWarning()<<"Key INTR";
//            if ((getKey()))// & ktab[(IA >> 4) & 0x3]) !=0)
            {
//                mem[0x1087c] = 0xff;
                m6502->write_signal(101,1,1);
//                if(pCPU->fp_log) fprintf(pCPU->fp_log,"\nINTERUPT\n");
//                pKEYB->LastKey=0;
            }
        }
    }

//    quint64 deltaState = pTIMER->state - old_state;

    if (pTIMER->usElapsedId(1)>=3906) {
//        if (timercnt1!=0)
        {
            timercnt1--;
            if ((timercnt1==0)&&(timercnt2==0)&&(timercnt3==0)) {
                m6502->write_signal(101,1,1);
                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n ROM TIMER\n");
            }
            if (timercnt1==0xff) {
                timercnt2--;
                if (timercnt2==0xff) {
                    timercnt3--;
                }
            }
        }
        pTIMER->resetTimer(1);
    }

    return true;
}

bool Crlh1000::Chk_Adr(DWORD *d, DWORD data)
{

    if(*d < 0x2000) return true; /* RAM */

    if (*d==0x58FE) {
        latchByte = data;
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n ROM latchByte=%02X\n",data);
        *d+=0xC000;
        return true;
    }

    if((*d>=0x4000) && (*d <=0x7FFF)) {

        if (latchByte & 0x80){
            if ((*d>=0x5800)&&(*d<0x58A0)) {
                ((Clcdc_rlh1000*)pLCDC)->mem[*d-0x5800] = data;
                pLCDC->SetDirtyBuf(*d-0x5800);


                return(false);
            }
            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE ROM LCD [%04X]=%02X\n",*d,data);
        }
        else {
            if (*d==0x58FB) timercnt1=data;
            if (*d==0x58FC) timercnt2=data;
            if (*d==0x58FD) timercnt3=data;
        }

        *d+=0xC000;
        if ( (*d>=0x107FE) && (*d<=(0x107FC+0xFF0))) {
            quint8 t = *d-0x107FE;
            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE ROM [%02X]=%02X\n",t,data);
            if (data) {
                pKEYB->KStrobe |= t;
            }
            else
                pKEYB->KStrobe &= ~t;
            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n ROM KSTROBE=%02X\n",pKEYB->KStrobe);

        }
        else if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE ROM [%04X]=%02X\n",*d-0xC000,data);
        return true;

    }
    if((*d>=0x8000) && (*d < 0xC000)) return false; /* RAM */



    return false;
}

bool Crlh1000::Chk_Adr_R(DWORD *d, DWORD *data)
{



    if((*d>=0x4000) && (*d < 0x7FFF)) {
        if ((latchByte & 0x04)==0) {
            if (latchByte & 0x80){  // LCD mapping
                if ((*d>=0x5800)&&(*d<0x58A0)) {
                    *data = ((Clcdc_rlh1000*)pLCDC)->mem[*d-0x5800];
                    return false;
                }
                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ ROM LCD [%04X]\n",*d);

            }
            else {  // KBD mapping
                if ( (*d>=0x47FC) && (*d<=(0x47FC+0x40))) {

                    *data = getKey(pKEYB->KStrobe);
                    if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ ROM KBD [%04X]\n",*d);
                    return false;
                }
            }
            if (*d==0x58FB) { *data=timercnt1; return false; }
            if (*d==0x58FC) { *data=timercnt2; return false; }
            if (*d==0x58FD) { *data=timercnt3; return false; }
            if (*d==0x58FF) { *data=0x00; return false; }

            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ ROM [%04X]\n",*d);
            return true;
        }
        else {
            DWORD offset = 0;
            switch (latchByte & 0x03) {
            case 0x00 : offset = 0; break;
            case 0x01 : offset = 0x10000; break;
            case 0x02 : offset = 0x14000; break;
            case 0x03 : // External ROM ???? break;
            default: break;
            }
            *d += offset;
            return true;
        }

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

#define KEY(c)	( pKEYB->keyPressedList.contains(TOUPPER(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(TOLOWER(c)))

UINT8 Crlh1000::getKey(quint8 port )
{

    quint8 ks;
    ks =port;//0xff^port;//1<<(port/4);
    AddLog(LOG_KEYBOARD,tr("ks=%1(%2)").arg(ks,4,16,QChar('0')).arg(port));
    UINT8 data=0;

    if ((pKEYB->LastKey) && ks )
    {
//        if (fp_log) fprintf(fp_log,"KSTROBE=%04X\n",ks);
//        qWarning()<<QString("ks:%1").arg(ks,4,16,QChar('0'));

        if (ks&0x01) {
            if (KEY('Q'))			data|=0x01;
            if (KEY('A'))			data|=0x02;
            if (KEY('Z'))			data|=0x04;
            if (KEY('+'))			data|=0x08;
            if (KEY('1'))			data|=0x10;
        }

        if (ks&0x02) {
            if (KEY('W'))			data|=0x01;
            if (KEY('S'))			data|=0x02; // A
            if (KEY('X'))			data|=0x04; // Q
            if (KEY('*'))			data|=0x08;
            if (KEY('2'))			data|=0x10; // 1
        }
        if (ks&0x04) {
            if (KEY('E'))			data|=0x01; // X
            if (KEY('D'))			data|=0x02; // S
            if (KEY('C'))			data|=0x04; // W
            if (KEY('-'))			data|=0x08;
            if (KEY('3'))			data|=0x10; // 2
        }

        if (ks&0x08) {
            if (KEY('R'))			data|=0x01; // C
            if (KEY('F'))			data|=0x02; // D
            if (KEY('V'))			data|=0x04; // E
            if (KEY('/'))			data|=0x08;
            if (KEY('4'))			data|=0x10; // 3
        }

        if (ks&0x10) {
            if (KEY('T'))			data|=0x01; // V
            if (KEY('G'))			data|=0x02; // F
            if (KEY('B'))			data|=0x04; // R
            if (KEY(K_F1))			data|=0x08;
            if (KEY('5'))			data|=0x10; // 4
        }

        if (ks&0x20) {
            if (KEY('Y'))			data|=0x01; // B
            if (KEY('H'))			data|=0x02; // G
            if (KEY('N'))			data|=0x04; // T
            if (KEY(K_F2))			data|=0x08;
            if (KEY('6'))			data|=0x10;
        }
        if (ks&0x40) {
            if (KEY('U'))			data|=0x01; // N
            if (KEY('J'))			data|=0x02; // H
            if (KEY('M'))			data|=0x04; // Y
            if (KEY(K_F3))			data|=0x08;
            if (KEY('7'))			data|=0x10;
        }
        if (ks&0x80) {
            if (KEY('I'))			data|=0x01; // M
            if (KEY('K'))			data|=0x02; // J
            if (KEY(K_RA))			data|=0x04; // U
            if (KEY(K_F4))			data|=0x08;
            if (KEY('8'))			data|=0x10;
        }
        if (ks&0x100) {
            if (KEY('O'))			data|=0x01;
            if (KEY('L'))			data|=0x02; // K
            if (KEY(K_LA))			data|=0x04; // I
            if (KEY(K_F5))			data|=0x08;
            if (KEY('9'))			data|=0x10;
        }
        if (ks&0x200) {
            if (KEY('P'))			data|=0x01; // *
            if (KEY(K_INS))			data|=0x02;
            if (KEY(K_UA))			data|=0x04;
            if (KEY(' '))			data|=0x08;
            if (KEY('0'))			data|=0x10;
        }
        if (ks&0x400) {
            if (KEY('='))			data|=0x01;     // numpad -
            if (KEY(K_BS))			data|=0x02;
            if (KEY(K_DA))			data|=0x04;
            if (KEY(','))			data|=0x08;
            if (KEY(':'))			data|=0x10;
            if (KEY('.'))			data|=0x20; // :
        }
        if (ks&0x800) {
            if (KEY(K_RET))			data|=0x01;

//            if (pKEYB->isShift) data|=0x02;

//            if (KEY(K_F2))			data|=0x02;
//            if (KEY(K_F3))			data|=0x04;
//            if (KEY(K_F4))		data|=0x08;
//            if (KEY(K_F5))			data|=0x10;
//            if (KEY(K_F6))			data|=0x20;
        }
        if (ks&0x1000) {
            if (KEY(K_F1))			data|=0x01;
            if (KEY(K_F2))			data|=0x02;
            if (pKEYB->isShift)     data|=0x04;
            if (KEY(K_F4))		data|=0x08;
            if (KEY(K_F5))			data|=0x10;
            if (KEY(K_F6))			data|=0x20;
        }
        if (ks&0x2000) {
////            if (KEY(K_F5))			data|=0x01;
//            if (KEY(K_INS))			data|=0x02;
//            if (KEY(K_DEL))			data|=0x04;
//            if (KEY(K_DA))			data|=0x08;
//            if (KEY(K_LA))			data|=0x10;
//            if (KEY(K_RA))			data|=0x20;
        }
        if (ks&0x4000) {
//            if (KEY(K_F7))			data|=0x01;
//            if (KEY(K_RET))			data|=0x02;
//            if (KEY(K_F6))			data|=0x04;
//            if (KEY(K_SML))			data|=0x08;  // KANA ???
//            if (KEY(K_CLR))			data|=0x10; // CLR ???
//            if (KEY(K_F5))			data|=0x20;
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
    return (data^0xff);

}
