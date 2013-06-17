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
#if 0
        mem[0x207]=7;
        mem[0x206]=6;
        mem[0x26C+7] = pKEYB->LastKey;
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


            if ( (*d>=0x47FE) && (*d<=(0x47FC+0xFF))) {
if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE ROM KBD [%04X]=%02X\n",*d,data);
                quint8 t = (*d-0x47FE)/4;

                if (t<32) {
                    if (data) {
                        strobe |= (1<<t);
                    }
                    else
                        strobe &= ~(1<<t);
                }
                else strobe = 0x1FFFF;

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
                if ( (*d>=0x47FC) && (*d<=(0x47FC+0xFF))) {
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

#define KEY(c)	((pKEYB->keyPressedList.contains(TOUPPER(c)) || \
                  pKEYB->keyPressedList.contains(c) || \
                  pKEYB->keyPressedList.contains(TOLOWER(c)))?1:0)

UINT8 Crlh1000::getKey(quint8 row )
{

    quint64 ks;
    ks =strobe;
    quint32 ligne=0;

    if ( row==32) ligne = 0x1FFFF;
    else ligne = (1<<row);

//    AddLog(LOG_KEYBOARD,tr("ks=%1(%2)").arg(ks,4,16,QChar('0')).arg(row));
    UINT8 data=0;

    if ((pKEYB->LastKey) )
    {
//        if (fp_log) fprintf(fp_log,"KSTROBE=%04X\n",ks);
//        qWarning()<<QString("ks:%1").arg(ks,4,16,QChar('0'));


        if (ks&0x01) {
            if ( (row&0x01) && KEY('A')) data = 1;
            if ( (row&0x02) && KEY('Z')) data = 1;
            if ( (row&0x04) && KEY('E')) data = 1;
            if ( (row&0x08) && KEY('R')) data = 1;
            if ( (row&0x10) && KEY('T')) data = 1;
            if ( (row&0x20) && KEY('Y')) data = 1;
            if ( (row&0x40) && KEY('U')) data = 1;
            if ( (row&0x80) && KEY('I')) data = 1;
            if ( (row&0x100) && KEY('O')) data = 1;
        }

        if (ks&0x8000) {
            if ( (row&0x01) && KEY('Q')) data = 1;
            if ( (row&0x02) && KEY('S')) data = 1;
            if ( (row&0x04) && KEY('D')) data = 1;
            if ( (row&0x08) && KEY('F')) data = 1;
            if ( (row&0x10) && KEY('G')) data = 1;
            if ( (row&0x20) && KEY('H')) data = 1;
            if ( (row&0x40) && KEY('J')) data = 1;
            if ( (row&0x80) && KEY('K')) data = 1;
            if ( (row&0x100) && KEY('L')) data = 1;
        }


        //        if (ks&0x02)
//        {
//            switch (row) {
//            case 1: data = KEY('A'); break;
//            case 2: data = KEY('Z'); break;
//            case 3: data = KEY('E'); break;
//            case 4: data = KEY('R'); break;
//            case 5: data = KEY('T'); break;
//            case 6: data = KEY('Y'); break;
//            case 7: data = KEY('U'); break;
//            case 8: data = KEY('I'); break;
//            case 0: data = KEY('O'); break;
//            }
//            if (data) strobe&=~0x02;
//        }
//        if (ks&0x04)
//        {
//            switch (row) {
//            case 1: data = KEY('A'); break;
//            case 2: data = KEY('Z'); break;
//            case 3: data = KEY('E'); break;
//            case 4: data = KEY('R'); break;
//            case 5: data = KEY('T'); break;
//            case 6: data = KEY('Y'); break;
//            case 7: data = KEY('U'); break;
//            case 8: data = KEY('I'); break;
//            case 0: data = KEY('O'); break;
//            }
//            if (data) strobe&=~0x04;
//        }
//        if (ks&0x08)
//        {
//            switch (row) {
//            case 1: data = KEY('A'); break;
//            case 2: data = KEY('Z'); break;
//            case 3: data = KEY('E'); break;
//            case 4: data = KEY('R'); break;
//            case 5: data = KEY('T'); break;
//            case 6: data = KEY('Y'); break;
//            case 7: data = KEY('U'); break;
//            case 8: data = KEY('I'); break;
//            case 0: data = KEY('O'); break;
//            }
//            if (data) strobe&=~0x08;
//        }
//        if (ks&0x10)
//        {
//            switch (row) {
//            case 1: data = KEY('A'); break;
//            case 2: data = KEY('Z'); break;
//            case 3: data = KEY('E'); break;
//            case 4: data = KEY('R'); break;
//            case 5: data = KEY('T'); break;
//            case 6: data = KEY('Y'); break;
//            case 7: data = KEY('U'); break;
//            case 8: data = KEY('I'); break;
//            case 0: data = KEY('O'); break;
//            }
//            if (data) strobe&=~0x10;
//        }
//        if (ks&0x20)
//        {
//            switch (row) {
//            case 1: data = KEY('A'); break;
//            case 2: data = KEY('Z'); break;
//            case 3: data = KEY('E'); break;
//            case 4: data = KEY('R'); break;
//            case 5: data = KEY('T'); break;
//            case 6: data = KEY('Y'); break;
//            case 7: data = KEY('U'); break;
//            case 8: data = KEY('I'); break;
//            case 0: data = KEY('O'); break;
//            }
//            if (data) strobe&=~0x20;
//        }
//        if (ks&0x40)
//        {
//            switch (row) {
//            case 1: data = KEY('A'); break;
//            case 2: data = KEY('Z'); break;
//            case 3: data = KEY('E'); break;
//            case 4: data = KEY('R'); break;
//            case 5: data = KEY('T'); break;
//            case 6: data = KEY('Y'); break;
//            case 7: data = KEY('U'); break;
//            case 8: data = KEY('I'); break;
//            case 0: data = KEY('O'); break;
//            }
//            if (data) strobe&=~0x40;
//        }
        if (ks&0x8000)
        {
            switch (row) {
            case 0: data = KEY('Q'); break;
            case 1: data = KEY('S'); break;
            case 2: data = KEY('D'); break;
            case 3: data = KEY('F'); break;
            case 4: data = KEY('G'); break;
            case 5: data = KEY('H'); break;
            case 6: data = KEY('J'); break;
            case 7: data = KEY('K'); break;
            case 8: data = KEY('L'); break;
            case 9: data = KEY('M'); break;
            case 10: data = KEY('W'); break;
            case 11: data = KEY('X'); break;
            case 12: data = KEY('C'); break;
            case 13: data = KEY('V'); break;
            case 14: data = KEY('B'); break;
            case 15: data = KEY('N'); break;
            case 16: data = KEY(','); break;
            }
//            if (data) strobe&=~0x8000;
        }
        if ( KEY(' ') || data) {
            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ KEYBOARD getKey=%04X\n",data);
            AddLog(LOG_KEYBOARD,tr("strobe=%1, row=%2\n").arg(ks,4,16,QChar('0')).arg(row));
        }
    }
        return (data>0 ? 0:0x01);

}
