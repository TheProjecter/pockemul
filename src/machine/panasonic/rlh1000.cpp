#include <QDebug>

#include "rlh1000.h"
#include "m6502/m6502.h"
#include "Inter.h"
#include "Keyb.h"
#include "Connect.h"
#include "Log.h"
#include "Lcdc_rlh1000.h"
#include "ui/dialogdasm.h"




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
    InitMemValue	= 0xFF;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	""                                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8 , 0x2000 ,	""/*P_RES(":/rlh1000/rlp1004a.bin")*/    , ""	, ROM , "Ext ROM"));
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

QMap<quint8,quint8> scandef;

bool Crlh1000::init(void)				// initialize
{

pCPU->logsw = true;
    if (!fp_log) fp_log=fopen("rlh1000.log","wt");	// Open log file
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
    strobe=0;


    scandef[K_RET] = 0xF0;
    scandef['A'] = 0x38;
    scandef['Z'] = 0x39;
    scandef['E'] = 0x3A;
    scandef['R'] = 0x3B;
    scandef['T'] = 0x3C;
    scandef['Y'] = 0x3D;
    scandef['U'] = 0x3E;
    scandef['I'] = 0x3F;

    return true;
}



bool Crlh1000::run() {

    // 0x5802 : Shift 0x80   ???
    // 0x5820 : 2nd 0x80     ???
    // timercnt = timer (1/256 sec)

    //TODO: change this !!!
    if (dialogdasm)
        dialogdasm->imem=false;

#if 0 //ndef QT_NO_DEBUG
    if (pCPU->get_PC()==0xc854) m6502->set_PC(0xc856);
#endif
//    mem[0x204]=1;
    CpcXXXX::run();

    if (pKEYB->LastKey>0) {
#if 1
        mem[0x207]=7;
        mem[0x206]=6;
        mem[0x26C+7] = pKEYB->LastKey;
        mem[0x27B]=0x80;
        qWarning()<<"push key:"<<pKEYB->LastKey;//<<" at:"<<fetch;
        pKEYB->LastKey=0;
#endif

        m6502->write_signal(101,1,1);
        //                if(pCPU->fp_log) fprintf(pCPU->fp_log,"\nINTERUPT\n");
    }


//    quint64 deltaState = pTIMER->state - old_state;

    if (pTIMER->usElapsedId(1)>=3906) {
//        if (timercnt1!=0)
        {
            timercnt1--;
            if ((timercnt1==0)&&(timercnt2==0)&&(timercnt3==0)) {
//                strobe=0;
                m6502->write_signal(101,1,1);
                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ ROM KBD TIMER\n");
            }
            if (timercnt1==0xff) {
//                strobe=0;
                m6502->write_signal(101,1,1);
                AddLog(LOG_CONSOLE,"INTERRUPT\n");
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
        if ((latchByte & 0x40) == 0) {
            // HALT CPU
            pCPU->halt = true;
            AddLog(LOG_CONSOLE,"CPU HALT\n");
            if (fp_log) fprintf(fp_log,"\nCPU HALT\n");
            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\nCPU HALT\n");
        }
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n ROM latchByte=%02X\n",data);
        *d+=0xC000;
        return true;
    }

    if((*d>=0x4000) && (*d <=0x7FFF)) {

        if (latchByte & 0x80){
            // LCD I/O Mapping
            if ((*d>=0x5800)&&(*d<0x58A0)) {
                ((Clcdc_rlh1000*)pLCDC)->mem[*d-0x5800] = data;
                pLCDC->SetDirtyBuf(*d-0x5800);
                return(false);
            }
            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE ROM LCD [%04X]=%02X\n",*d,data);
            *d+=0xc000;
            return true;
        }
        else {
            // KBD I/O Mapping
            if (*d==0x58FB) { timercnt1=data; return false; }
            if (*d==0x58FC) { timercnt2=data; return false; }
            if (*d==0x58FD) { timercnt3=data; return false; }
            if (*d==0x58FF) { return true; }


            if ( (*d>=0x47FE) && (*d<=(0x47FC+0xff))) {
if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE ROM KBD [%04X]=%02X\n",*d,data);
                quint8 t = (*d-0x47FE)/4;

#if 0
                    strobe = (data ? t : 0 );
#else
                //                if (t<32) {
                if (t==32){
                    strobe32= data;
                }
                else {
                    if (data) {
                        strobe |= (1<<(t&0x07));
                    }
                    else {
                        strobe &= ~(1<<(t&0x07));
                    }
                }
                if (fp_log) fprintf(fp_log,"WRITE KEYBOARD [%04X,%2i]=%i  strobe=%08X ",*d,t,data,strobe);
if (fp_log) fprintf(fp_log,"str32=%i\n",strobe32);
#endif
//                }
//                else strobe = (data ? 0x1FFFF : 0);

                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE KEYBOARD [%i]=%i  strobe=%08X\n",t,data,strobe);

//                pKEYB->KStrobe&=0x7F;
//                pKEYB->KStrobe = (1<<(pKEYB->KStrobe/8));
                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n ROM KSTROBE=%08X\n",strobe);
                return false;
            }
            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE ROM [%02X]=%02X\n",*d,data);
            *d+=0xc000;
            return true;
        }


        return true;

    }
    if((*d>=0x8000) && (*d < 0xC000)) return true; /* RAM */



    return false;
}

bool Crlh1000::Chk_Adr_R(DWORD *d, DWORD *data)
{



    if((*d>=0x4000) && (*d <= 0x7FFF)) {
        if ((latchByte & 0x04)==0) {
            if (latchByte & 0x80){
                // LCD mapping
                if ((*d>=0x5800)&&(*d<0x58A0)) {
                    *data = ((Clcdc_rlh1000*)pLCDC)->mem[*d-0x5800];
                    return false;
                }
                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ ROM LCD [%04X]\n",*d);

            }
            else {
                // KBD mapping
                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ ROM KBD [%04X]\n",*d);
                if ( (*d>=0x47FC) && (*d<=(0x47FC+0x40))) {
                    quint8 t=(*d-0x47FC)/4;

                    *data = getKey(t);

                    if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ KEYBOARD [%i]=%04X\n",t,*data);
                    if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ ROM KBD [%04X,%02X]=%02x\n",strobe,t,*data);
                    return false;
                }
            }
            if (*d==0x58FB) { *data=timercnt1; return false; }
            if (*d==0x58FC) { *data=timercnt2; return false; }
            if (*d==0x58FD) { *data=timercnt3; return false; }
            if (*d==0x58FF) { *data = 0; return false; }

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

#define KEY(c)	((pKEYB->keyPressedList.contains(TOUPPER(c)) || \
                  pKEYB->keyPressedList.contains(c) || \
                  pKEYB->keyPressedList.contains(TOLOWER(c)))?1:0)

UINT8 Crlh1000::getKey(quint8 row )
{
    row &= 0x07;
if (fp_log) fprintf(fp_log,"READ KEYBOARD [%2i] ",row);

    quint8 ks;
    ks =strobe;
    quint32 ligne=0;

    quint8 kr = row;

//    AddLog(LOG_KEYBOARD,tr("ks=%1(%2)").arg(ks,4,16,QChar('0')).arg(row));
    UINT8 data=0;

    if ((pKEYB->LastKey) )
    {
        if (row==32) return 0xFF;

        if (fp_log) fprintf(fp_log," PUSH kr=%i ",kr);
        if (ks&0x01) {
            if (KEY(K_RET) && (kr==0)) return 1;
        }

        if (ks&0x80) {

            if (KEY('A') && (kr==0)) return 1;
            if (KEY('Z') && (kr==1)) return 1;
            if (KEY('E') && (kr==2)) return 1;
            if (KEY('R') && (kr==3)) return 1;
            if (KEY('T') && (kr==4)) return 1;
            if (KEY('Y') && (kr==5)) return 1;
            if (KEY('U') && (kr==6)) return 1;

        }
        if (ks&0x02) {
            if (KEY('Q') && (kr==5)) return 1;
            if (KEY('S') && (kr==6)) return 1;
        }
    }

    if (fp_log) fprintf(fp_log,"=%i\n",data);
    return data;

}
