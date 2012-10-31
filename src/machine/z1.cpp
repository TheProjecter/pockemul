#include <QtGui>
#include <QString>

#include "common.h"
#include "z1.h"
#include "i80x86.h"
#include "Inter.h"
#include "Keyb.h"
#include "Log.h"
#include "Lcdc_z1.h"
#include "hd66108.h"

#ifdef POCKEMUL_BIG_ENDIAN
#	define LOW(x)	((uint8 *)&(x) + 1)
#	define HIGH(x)	((uint8 *)&(x) + 0)
#else
#	define LOW(x)	((uint8 *)&(x) + 0)
#	define HIGH(x)	((uint8 *)&(x) + 1)
#endif


Cz1::Cz1(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 3865000);
    setcfgfname(QString("z1"));

    SessionHeader	= "Z1PKM";
    Initial_Session_Fname ="z1.pkm";

    BackGroundFname	= ":/z1/z1gr.png";
    LcdFname		= ":/z1/z1grlcd.png";
    SymbFname		= "";

    memsize		= 0xFFFFF;
    InitMemValue	= 0x00;

    SlotList.clear();
    SlotList.append(CSlot(64 , 0x00000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(64  , 0xa0000 ,	""                  , ""	, RAM , "VIDEO RAM"));
    SlotList.append(CSlot(128 , 0xE0000 ,	":/z1/romz1.bin"	, ""	, ROM , "ROM"));


    KeyMap		= KeyMap1250;
    KeyMapLenght= KeyMap1250Lenght;

    PowerSwitch	= 0;
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

    pLCDC		= new Clcdc_z1(this);
    pCPU		= new Ci80x86(this);
    pTIMER		= new Ctimer(this);
    pHD66108    = new CHD66108(this);
    pKEYB		= new Ckeyb(this,"z1.map");

    lastKeyBufSize = 0;
    newKey = false;

    i86cpu = (Ci80x86*)pCPU;
}

Cz1::~Cz1() {

}

bool Cz1::init(void)				// initialize
{
    if (!fp_log) fp_log=fopen("z1.log","wt");	// Open log file
//pCPU->logsw = true;
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;

#endif
    CpcXXXX::init();

    for(int i = 0; i < 0x10000; i++)
        mem[i] = i & 0xff;

    pHD66108->init();

    eoi = 0x8000;
    io_b8 = timer0Control = timer2Control = timer2Control = 0;

    intPulseId = pTIMER->initTP(240);
    lastIntPulse = false;
    return true;
}



bool Cz1::run() {


    CpcXXXX::run();

    if (pCPU->fp_log && !pCPU->halt && !off ) fprintf(pCPU->fp_log,"eoi=%04x   t2Ctrl=%04x\n\n", eoi,timer2Control);

    bool pulse = pTIMER->GetTP(intPulseId);
    if (pulse != lastIntPulse) {

        lastIntPulse = pulse;

        // Check if keybuffer size change
        if (lastKeyBufSize != pKEYB->keyPressedList.size()) {
            newKey = true;
            lastKeyBufSize = pKEYB->keyPressedList.size();
        }

        if (pKEYB->LastKey == K_POW_OFF) {
            newKey = false;
            AddLog(LOG_MASTER,"INT NMI");
            i86cpu->i86nmi(&i86cpu->i86stat);
        }
        if (newKey) {
            if(eoi & 0x8000) {
                if(i86cpu->i86int(&(i86cpu->i86stat), 0x0c)) {
                    newKey = false;
//                    if (pCPU->fp_log) fprintf(pCPU->fp_log,"INT 0x0C\n");
                    AddLog(LOG_MASTER,"INT 0x0C");
                    eoi = 0;
                }
            }
        }
        if((timer2Control & 0xe001) == 0xe001) {
            if(eoi & 0x8000) {
                if(i86cpu->i86int(&(i86cpu->i86stat), 0x13)) {
//                    if (pCPU->fp_log) fprintf(pCPU->fp_log,"INT 0x13\n");
                    AddLog(LOG_MASTER,"INT 0x13");
                    eoi = 0;
                }
            }
        }


    }

    return true;
}

bool Cz1::Chk_Adr(DWORD *d, DWORD data)
{
    /*
    if(0x400 <= p && p <= 0x40f)
        printf("WRITE IP=%04x p=%04x v=%02x\n", cpu->r16.ip, p, v);
    */
    if (*d==0x16BB) {
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"\nWRITE 16BB:%04X\n",data);
        return true;

    }
    if(*d < 0x10000) return true; /* RAM */
    if(*d < 0xa0000) return false;
    if(*d < 0xb0000){
        AddLog(LOG_DISPLAY,tr("WriteVram[%1]=%2").arg(*d,5,QChar('0')).arg(data));
        if(pCPU->fp_log) fprintf(pCPU->fp_log,"Write VRAM");
        pHD66108->writeVram( *d, data);
        return false;
    }

    return false;
}

bool Cz1::Chk_Adr_R(DWORD *d, DWORD data)
{
    /*
    if(0x400 <= p && p <= 0x40f)
        printf("READ IP=%04x p=%04x v=%02x\n", cpu->r16.ip, p, cpu->m[p]);
    */
    if (*d==0x16BB) {
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"\nREAD 16BB\n");
        return true;

    }


    if( *d < 0x10000) return true;
    else if (*d < 0xa0000) { mem[*d] = *d & 0xff; return true; }
    else if (*d < 0xb0000) {
        AddLog(LOG_DISPLAY,tr("ReadVram[%1]").arg(*d,5,QChar('0')));
        mem[*d] = pHD66108->readVram(*d);
        return true;
    }
    else if (*d < 0xe0000) { mem[*d] = *d & 0xff; return true; }

    return true;
}

UINT8 Cz1::in(UINT8 Port)
{
    return 0;
}

UINT8 Cz1::in8(UINT16 Port)
{
    UINT16 v=0;

    switch(Port) {
    case 0x003e: /* ? */
        return 0x20;
    case 0x005a: /* バッテリー容量 */
        return 0x04 | 0x80;
    /*
    case 0x0082:
        return 0x00;
    case 0x0083:
        return 0x08;
    case 0x0086:
        return 0x00;
    case 0x0087:
        return 0x08;
    case 0x00a2:
        return 0x0a;
    case 0x00a3:
        return 0x08;
    */
    case 0x0202:
    case 0x0203:
        v = getKey();
        if(!v) v = 0x8000;
        v = (Port == 0x0202 ? v & 0xff: v >> 8);
        if (v) { AddLog(LOG_KEYBOARD,tr("Read Keyboard[%1] = %2").arg(Port,4,16,QChar('0')).arg(v&0xff,2,16,QChar('0'))); }
        return v;
    case 0x220:
    case 0x221:
    case 0x222: {AddLog(LOG_TEMP,tr("IN [%1]").arg(Port,4,16,QChar('0')));
        if (fp_log) fprintf(fp_log,"IN[%04x]\n",Port);
        return 0x00;
        break;}
    default:
        if (fp_log) fprintf(fp_log,"IN[%04x]    pc=%08x\n",Port,pCPU->get_PC());
        AddLog(LOG_TEMP,tr("IN[%1]\t%2").arg(Port,4,16,QChar('0')).arg(pCPU->get_PC(),8,16,QChar('0')));
        return 0;
    }
     return (0);
}

UINT8 Cz1::out(UINT8 Port, UINT8 x)
{
    return 0;
}
UINT8 Cz1::out8(UINT16 Port, UINT8 x)
{
    switch(Port) {
    case 0x0002:
        *LOW(eoi) = x;
//        AddLog(LOG_MASTER,tr("Set eoi Low[%1]=%2").arg(x,2,16,QChar('0')).arg(eoi,4,16,QChar('0')));
        break;
    case 0x0003:
        *HIGH(eoi) = x;
//        AddLog(LOG_MASTER,tr("Set eoi High[%1]=%2").arg(x,2,16,QChar('0')).arg(eoi,4,16,QChar('0')));
        break;
    case 0x0036:
        *LOW(timer0Control) = x;
        break;
    case 0x0037:
        *HIGH(timer0Control) = x;
        break;
    case 0x003e:
        *LOW(timer1Control) = x;
        break;
    case 0x003f:
        *HIGH(timer1Control) = x;
        break;
    case 0x0046:
        *LOW(timer2Control) = x;
        AddLog(LOG_MASTER,tr("Set T2Control Low[%1]=%2").arg(x,2,16,QChar('0')).arg(timer2Control,4,16,QChar('0')));
        break;
    case 0x0047:
        *HIGH(timer2Control) = x;
        AddLog(LOG_MASTER,tr("Set T2Control High[%1]=%2").arg(x,2,16,QChar('0')).arg(timer2Control,4,16,QChar('0')));
        break;
    case 0x00b8:
        io_b8 = x;
        break;
    case 0x0200:
//        ks = ks & 0xff00 | x;
//        AddLog(LOG_KEYBOARD,tr("Set KSL[%1]=%2").arg(x,2,16,QChar('0')).arg(ks,4,16,QChar('0')));
        *LOW(ks) = x;
        break;
    case 0x0201:
//        ks = (x <<8) | (ks & 0xff);
//        AddLog(LOG_KEYBOARD,tr("Set KSH[%1]=%2").arg(x,2,16,QChar('0')).arg(ks,4,16,QChar('0')));
        *HIGH(ks) = x;
        break;
    case 0x0220: /* ?? */

        AddLog(LOG_TEMP,tr("OUT 220H = %1").arg(x,4,16,QChar('0')));
        dumpXYW();
        fprintf(fp_log,"OUT 220H %02x\n", x);
        fprintf(fp_log,"X=");
        for(int i = 0x400; i <= 0x40f; i++)
            fprintf(fp_log,"%02x ", mem[i]);
        fprintf(fp_log,"\n");
        fprintf(fp_log,"Y=");
        for(int i = 0x410; i <= 0x41f; i++)
            fprintf(fp_log,"%02x ", mem[i]);
        fprintf(fp_log,"\n");
        fprintf(fp_log,"W=");
        for(int i = 0x420; i <= 0x42e; i++)
            fprintf(fp_log,"%02x ", mem[i]);
        fprintf(fp_log,"\n");
#if 1
        if(x == 0x99)
        {
//            mem[0x400] = 0x00;
//            mem[0x401] = 0x00;
//            mem[0x402] = 0x00;
//            mem[0x403] = 0x00;
//            mem[0x404] = 0x00;
//            mem[0x405] = 0x00;
//            mem[0x406] = 0x01;
//            mem[0x407] = 0x06;
//            mem[0x408] = 0x00;
        }
//        if(i86cpu->i86read8(&(i86cpu->i86stat), 0, 0x400) != 0)
//            mem[0x400] = 0;
//        if(i86cpu->i86read8(&(i86cpu->i86stat), 0, 0x405) != 0)
//            mem[0x405] = 0;

#endif
#if 0
        for(int i = 0x400; i < 0x420; i++)
            i86cpu->i86write8(&(i86cpu->i86stat), 0, i, 0);
        i86cpu->i86write8(&(i86cpu->i86stat), 0, 0x404, 0x12);
        i86cpu->i86write8(&(i86cpu->i86stat), 0, 0x405, 0x34);
        i86cpu->i86write8(&(i86cpu->i86stat), 0, 0x406, 5);//rand() % 0x10);
        i86cpu->i86write8(&(i86cpu->i86stat), 0, 0x407, 0x12);
        i86cpu->i86write8(&(i86cpu->i86stat), 0, 0x408, 0x34);
        i86cpu->i86write8(&(i86cpu->i86stat), 0, 0x409, 0x56);
        i86cpu->i86write8(&(i86cpu->i86stat), 0, 0x40a, 0x78);
#endif
        break;
    case 0x0221:
        AddLog(LOG_TEMP,tr("OUT 221H = %1").arg(x,4,16,QChar('0')));
        dumpXYW();

        fprintf(fp_log,"OUT 221H %02x\n", x);
        fprintf(fp_log,"X=");
        for(int i = 0x400; i <= 0x40f; i++)
            fprintf(fp_log,"%02x ", mem[i]);
        fprintf(fp_log,"\n");
        fprintf(fp_log,"Y=");
        for(int i = 0x410; i <= 0x41f; i++)
            fprintf(fp_log,"%02x ", mem[i]);
        fprintf(fp_log,"\n");
        fprintf(fp_log,"W=");
        for(int i = 0x420; i <= 0x42e; i++)
            fprintf(fp_log,"%02x ", mem[i]);
        fprintf(fp_log,"\n");


#if 1
        if(x == 0x10) {
            for(int i = 0x400; i <= 0x408; i++)
                i86cpu->i86write8(&(i86cpu->i86stat), 0, i, 0);
        }
        if(x == 0x11) {
            for(int i = 0x410; i <= 0x418; i++)
                i86cpu->i86write8(&(i86cpu->i86stat), 0, i, 0);
        }
//            for(int i = 0x420; i < 0x42f; i++)
//                i86cpu->i86write8(&(i86cpu->i86stat), 0, i, 0);

#endif


        break;
    default:
        if (fp_log) fprintf(fp_log,"OUT[%04x]=%02x\tpc=%08x\n",Port,x,pCPU->get_PC());
        break;
    }

    return 0;
}

UINT16 Cz1::in16(UINT16 address)
{
}

UINT16 Cz1::out16(UINT16 Port, UINT16 x)
{
    switch (Port) {
    case 0x220:
        fprintf(fp_log,"before OUT[%04X]=%04x  PC=%08x\n", Port,x,pCPU->get_PC());
        dumpXYW();
        switch (x) {
        case 0x0499:
            for(int i = 0x406; i >= 0x401; i--)
                mem[i] = mem[i-1];
            mem[0x400]=0;
        }
        fprintf(fp_log,"after OUT[%04X]=%04x  PC=%08x\n", Port,x,pCPU->get_PC());
        dumpXYW();
        break;
    default:
        out8(Port + 1, x >> 8);
        out8(Port, x & 0xff);
    }

//    if ((Port >=0x220)&&(Port < 0x240)) {
//        fprintf(fp_log,"OUT[%04X]=%04x  pc=%08x\n", Port,x,pCPU->get_PC());
//        AddLog(LOG_TEMP,tr("OUT[%1]=%2\t%3").arg(Port,4,16,QChar('0')).arg(x,4,16,QChar('0')).arg(pCPU->get_PC(),8,16,QChar('0')));
//        dumpXYW();

//    }
//    out8(Port + 1, x >> 8);
//    out8(Port, x & 0xff);
}

void Cz1::dumpXYW(void) {
#if 0
    QString _tmp="";
    for(int i = 0x400; i <= 0x40f; i++) {
        _tmp = _tmp+QString("%1 ").arg(mem[i],2,16,QChar('0'));
    AddLog(LOG_TEMP,"X="+_tmp);
    _tmp="";
    for(int i = 0x410; i <= 0x41f; i++)
        _tmp = _tmp+QString("%1 ").arg(mem[i],2,16,QChar('0'));
    AddLog(LOG_TEMP,"Y="+_tmp);
    _tmp="";
    for(int i = 0x420; i < 0x42f; i++)
        _tmp = _tmp+QString("%1 ").arg(mem[i],2,16,QChar('0'));
    AddLog(LOG_TEMP,"W="+_tmp);
#else
    fprintf(fp_log,"X=");
    for(int i = 0x400; i <= 0x40f; i++)
        fprintf(fp_log,"%02x ", mem[i]);
    fprintf(fp_log,"\n");
    fprintf(fp_log,"Y=");
    for(int i = 0x410; i <= 0x41f; i++)
        fprintf(fp_log,"%02x ", mem[i]);
    fprintf(fp_log,"\n");
    fprintf(fp_log,"W=");
    for(int i = 0x420; i <= 0x42e; i++)
        fprintf(fp_log,"%02x ", mem[i]);
    fprintf(fp_log,"\n");

#endif
}

void Cz1::TurnOFF(void) {
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
    AddLog(LOG_TEMP,"TURN OFF");

}

void Cz1::TurnON(void){
    CpcXXXX::TurnON();


    AddLog(LOG_CANON,"TURN ON");

}


void Cz1::Reset()
{
    CpcXXXX::Reset();

}

bool Cz1::LoadConfig(QXmlStreamReader *xmlIn)
{

    return true;
}

bool Cz1::SaveConfig(QXmlStreamWriter *xmlOut)
{

    return true;
}

/*
    KO0     KO1     KO2     KO3     KO4     KO5     KO6     KO7     KO8     KO9     KO10	KOS
Ki0	BRK
KI1         TAB     W       R       Y       I       P       MENU	CAL     DEGR	SQR
KI2         Q       E       T       U       O       2nd     log     ln      sin     X2
KI3         RESET   S       F       H       K       ;       MR      M +     (       ENG
KI4         A       D       G       J       L       :       7       8       )       CLS
KI5         CAPS	X       V       N       ,       ?       4       5       9       cos
KI6         Z       C       B       M       INS     DEL     1       2       6       ^
KI7                 SRCH	OUT     SPACE	LA      RA      .       3       *       ANS
KI8                 IN      CALC	=       DA      0       E       +       /
KI9                                                         ENTER   -       BS      tan
KIS                                                                                         SHFT

*/

#define KEY(c)	( pKEYB->keyPressedList.contains(toupper(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(tolower(c)))

UINT16 Cz1::getKey()
{

    UINT16 data=0;

    if ((pKEYB->LastKey) && ks )
    {
        if (ks&1) {
            if (KEY(K_BRK))			data|=0x01;
        }
        if (ks&2) {

            if (KEY(K_TAB))			data|=0x02;
            if (KEY('Q'))			data|=0x04;
            if (KEY(K_RESET))		data|=0x08;
            if (KEY('A'))			data|=0x10;
            if (KEY(K_SML))			data|=0x20;
            if (KEY('Z'))			data|=0x40;
        }
        if (ks&4) {
            if (KEY('W'))			data|=0x02;
            if (KEY('E'))			data|=0x04;
            if (KEY('S'))			data|=0x08;
            if (KEY('D'))			data|=0x10;
            if (KEY('X'))			data|=0x20;
            if (KEY('C'))			data|=0x40;
//            if (KEY(K_SEARCH))			data|=0x80;
            if (KEY(K_IN))			data|=0x100;
        }
        if (ks&8) {
            if (KEY('R'))			data|=0x02;
            if (KEY('T'))			data|=0x04;
            if (KEY('F'))           data|=0x08;
            if (KEY('G'))			data|=0x10;
            if (KEY('V'))			data|=0x20;
            if (KEY('B'))			data|=0x40;
            if (KEY(K_OUT))			data|=0x80;
            if (KEY(K_CALC))		data|=0x100;
        }
        if (ks&0x10) {
            if (KEY('Y'))			data|=0x02;
            if (KEY('U'))			data|=0x04;
            if (KEY('H'))			data|=0x08;
            if (KEY('J'))			data|=0x10;
            if (KEY('N'))			data|=0x20;
            if (KEY('M'))			data|=0x40;
            if (KEY(' '))			data|=0x80;
            if (KEY('='))			data|=0x100;
        }
        if (ks&0x20) {
            if (KEY('I'))			data|=0x02;
            if (KEY('O'))			data|=0x04;
            if (KEY('K'))			data|=0x08;
            if (KEY('L'))			data|=0x10;
            if (KEY(','))			data|=0x20;
            if (KEY(K_INS))			data|=0x40;
            if (KEY(K_LA))   		data|=0x80;
            if (KEY(K_DA))			data|=0x100;
        }
        if (ks&0x40) {
            if (KEY('P'))			data|=0x02;
            if (KEY(K_SHT))         data|=0x04;
            if (KEY(';'))			data|=0x08;
            if (KEY(':'))			data|=0x10;
            if (KEY(K_UA))			data|=0x20;
            if (KEY(K_DEL))			data|=0x40;
            if (KEY(K_RA))			data|=0x80;
            if (KEY('0'))			data|=0x100;
        }
        if (ks&0x80) {
            if (KEY(K_MENU))		data|=0x02;
            if (KEY(K_LOG))			data|=0x04;
//            if (KEY(K_M))			data|=0x08;
            if (KEY('7'))			data|=0x10;
            if (KEY('4'))			data|=0x20;
            if (KEY('1'))			data|=0x40;
            if (KEY('.'))			data|=0x80;
            if (KEY(K_EXP))			data|=0x100;
            if (KEY(K_RET))			data|=0x200;
        }
        if (ks&0x100) {
            if (KEY(K_CAL))			data|=0x02;
            if (KEY(K_LN))  		data|=0x04;
            if (KEY(K_MPLUS))		data|=0x08;
            if (KEY('8'))			data|=0x10;
            if (KEY('5'))			data|=0x20;
            if (KEY('2'))			data|=0x40;
            if (KEY('3'))			data|=0x80;
            if (KEY('+'))			data|=0x100;
            if (KEY('-'))			data|=0x200;
        }

        if (ks&0x200) {
            if (KEY(K_DEG))			data|=0x02;
            if (KEY(K_SIN))			data|=0x04;
            if (KEY('('))			data|=0x08;
            if (KEY(')'))			data|=0x10;
            if (KEY('9'))			data|=0x20;
            if (KEY('6'))			data|=0x40;
            if (KEY('*'))			data|=0x80;
            if (KEY('/'))			data|=0x100;
            if (KEY(K_BS))			data|=0x200;
        }
        if (ks&0x400) {
            if (KEY(K_ROOT))			data|=0x02;
            if (KEY(K_SQR))			data|=0x04;
//            if (KEY(K_ENG)			data|=0x08;
            if (KEY(K_CLR))			data|=0x10;
            if (KEY(K_COS))			data|=0x20;
            if (KEY(K_POT))			data|=0x40;
            if (KEY(K_ANS))			data|=0x80;
            if (KEY(K_TAN))			data|=0x200;
        }
        if (ks&0x800) {
            if (KEY(K_SHT2))			data|=0x400;
        }
//        if (fp_log) fprintf(fp_log,"Read key [%02x]: strobe=%02x result=%02x\n",pKEYB->LastKey,ks,data^0xff);

    }
    return data;

}
