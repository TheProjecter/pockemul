#include <QtGui>

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
    setfrequency( (int) 3840000);
    setcfgfname(QString("z1"));

    SessionHeader	= "Z1PKM";
    SessionHeaderLen= 5;
    Initial_Session_Fname ="z1.pkm";

    BackGroundFname	= ":/z1/z1gr.png";
    LcdFname		= ":/z1/z1grlcd.png";
    SymbFname		= "";

    memsize		= 0xFFFFF;
    InitMemValue	= 0x00;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(128 , 0xE0000 ,	":/z1/romz1gr.bin"	, ""	, ROM , "ROM"));
//    SlotList.append(CSlot(8 , 0x4000 ,	""					, ""	, ROM , "Prise ROM 8kb"));
//    SlotList.append(CSlot(8 , 0x6000 ,	""					, ""	, ROM , "ROM Carte 8kb"));
//    SlotList.append(CSlot(6 , 0x8000 ,	""					, ""	, RAM , "Carte RAM 8kb"));
//    SlotList.append(CSlot(4 , 0xA000 ,	""                  , ""	, ROM , "ROM TV"));
//    SlotList.append(CSlot(20, 0xB000 ,	":/x07/rom_xo7.bin" , "x07/rom_xo7.bin" 	, ROM , "BASIC ROM"));

    KeyMap		= KeyMap1250;
    KeyMapLenght= KeyMap1250Lenght;

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(206);//Pc_DX_mm =200 ;
    setDYmm(83);//Pc_DY_mm =130;
    setDZmm(30);//Pc_DZ_mm = 10;

    setDX(736);//Pc_DX		= 483;//409;
    setDY(297);//Pc_DY		= 252;//213;

    Lcd_X		= 92;
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
    pKEYB		= new Ckeyb(this,"g850v.map");



}

Cz1::~Cz1() {

}

bool Cz1::init(void)				// initialize
{


#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();

    for(int i = 0; i < 0x20000; i++)
        mem[i] = i & 0xff;

    pHD66108->init();

    eoi = 0x8000;
    io_b8 = timer0Control = timer2Control = timer2Control = 0;
    return true;
}



bool Cz1::run() {

    Ci80x86 *i86cpu = (Ci80x86*)pCPU;

    CpcXXXX::run();

    if(pCPU->halt) {
        if(io_b8 == 1 && !(i86cpu->i86.r16.f & 0x0200)) return true;

    }

    if (pKEYB->LastKey == K_POW_OFF) {
        i86cpu->i86nmi(&i86cpu->i86);
    }
    else if (pKEYB->LastKey != 0) {
        if(eoi & 0x8000) {
            if(i86cpu->i86int(&i86cpu->i86, 0x0c)) {
                eoi = 0;
            }
        }
    }
    else if((timer2Control & 0xe001) == 0xe001) {
                if(eoi & 0x8000)
                    if(i86cpu->i86int(&i86cpu->i86, 0x13))
                        eoi = 0;
    }

    return true;
}

bool Cz1::Chk_Adr(DWORD *d, DWORD data)
{
    /*
    if(0x400 <= p && p <= 0x40f)
        printf("WRITE IP=%04x p=%04x v=%02x\n", cpu->r16.ip, p, v);
    */

    if(*d < 0x40000) return true; /* RAM */
    if(*d < 0xa0000) return false;
    if(*d < 0xb0000){
        AddLog(LOG_DISPLAY,tr("WriteVram[%1]=%2").arg(*d,5,QChar('0')).arg(data));
        pHD66108->writeVram( *d, data);
    }

    return false;
}

bool Cz1::Chk_Adr_R(DWORD *d, DWORD data)
{
    /*
    if(0x400 <= p && p <= 0x40f)
        printf("READ IP=%04x p=%04x v=%02x\n", cpu->r16.ip, p, cpu->m[p]);
    */
    if( *d < 0x40000) return true;
    else if (*d < 0xa0000) { mem[*d] = *d & 0xff; return true; }
    else if (*d < 0xb0000) {
        AddLog(LOG_DISPLAY,tr("ReadVram[%1]").arg(*d,5,QChar('0')));
        pHD66108->readVram(*d);
        return true;
    }
    else if (*d < 0xe0000) { mem[*d] = *d & 0xff; return true; }

    return true;
}

UINT8 Cz1::in(UINT8 Port)
{
    return 0;
}

UINT8 Cz1::in16(UINT16 Port)
{
    UINT16 v=0;

    switch(Port) {
    case 0x003e: /* ? */
        return 0x20;
    case 0x005a: /* バッテリー容量 */
        return 0x04;
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
        /*
        printf(
        "KEY %04x:"
        "%04x "
        "%04x "
        "%04x "
        "%04x "
        "%04x "
        "%04x "
        "%04x "
        "%04x "
        "%04x "
        "%04x "
        "%04x\n",
        z1->key.strobe,
        z1->key.matrix[0],
        z1->key.matrix[1],
        z1->key.matrix[2],
        z1->key.matrix[3],
        z1->key.matrix[4],
        z1->key.matrix[5],
        z1->key.matrix[6],
        z1->key.matrix[7],
        z1->key.matrix[8],
        z1->key.matrix[9],
        z1->key.matrix[10]
        );
        */
//        v =
//        (z1->key.strobe & 0x0001 ? z1->key.matrix[0]: 0) |
//        (z1->key.strobe & 0x0002 ? z1->key.matrix[1]: 0) |
//        (z1->key.strobe & 0x0004 ? z1->key.matrix[2]: 0) |
//        (z1->key.strobe & 0x0008 ? z1->key.matrix[3]: 0) |
//        (z1->key.strobe & 0x0010 ? z1->key.matrix[4]: 0) |
//        (z1->key.strobe & 0x0020 ? z1->key.matrix[5]: 0) |
//        (z1->key.strobe & 0x0040 ? z1->key.matrix[6]: 0) |
//        (z1->key.strobe & 0x0080 ? z1->key.matrix[7]: 0) |
//        (z1->key.strobe & 0x0100 ? z1->key.matrix[8]: 0) |
//        (z1->key.strobe & 0x0200 ? z1->key.matrix[9]: 0) |
//        (z1->key.strobe & 0x0400 ? z1->key.matrix[10]: 0);
//        if(!v)
//            v = 0x8000;
        v = getKey();
        if (v) { AddLog(LOG_KEYBOARD,tr("Read Keyboard = %1").arg(v,4,16,QChar('0'))); }
        return (Port == 0x0202 ? v & 0xff: v >> 8);
    default:
        /*
        printf("IO IN %04x\n", port);
        */
        return 0;
    }
     return (0);
}

UINT8 Cz1::out(UINT8 Port, UINT8 x)
{
    return 0;
}
UINT8 Cz1::out16(UINT16 Port, UINT8 x)
{
    switch(Port) {
    case 0x0002:
        *LOW(eoi) = x;
        break;
    case 0x0003:
        *HIGH(eoi) = x;
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
        break;
    case 0x0047:
        *HIGH(timer2Control) = x;
        break;
    case 0x00b8:
        io_b8 = x;
        break;
    case 0x0200:
//        ks = ks & 0xff00 | x;
        AddLog(LOG_KEYBOARD,tr("Set KSL[%1]=%2").arg(x,2,16,QChar('0')).arg(ks,4,16,QChar('0')));
        *LOW(ks) = x;
        break;
    case 0x0201:
//        ks = (x <<8) | (ks & 0xff);
        AddLog(LOG_KEYBOARD,tr("Set KSH[%1]=%2").arg(x,2,16,QChar('0')).arg(ks,4,16,QChar('0')));
        *HIGH(ks) = x;
        break;
    case 0x0220: /* ?? */
        /*
        printf("OUT 220H %02x\n", x);
        printf("X=");
        for(i = 0x400; i <= 0x40f; i++)
            printf("%02x ", i86->m[i]);
        printf("\n");
        printf("Y=");
        for(i = 0x410; i <= 0x41f; i++)
            printf("%02x ", i86->m[i]);
        printf("\n");
        printf("W=");
        for(i = 0x420; i <= 0x42e; i++)
            printf("%02x ", i86->m[i]);
        printf("\n");
        fflush(stdout);

        if(x == 0x99)
            i86->m[0x406] = 1;
        if(i86read8(i86, 0, 0x400) != 0)
            i86->m[0x400] = 0;
        if(i86read8(i86, 0, 0x405) != 0)
            i86->m[0x405] = 0;
        */

        /*
        for(i = 0x400; i < 0x420; i++)
            i86write8(i86, 0, i, 0);
        i86write8(i86, 0, 0x406, rand() % 10);
        */
        break;
    case 0x0221:
        /*
        printf("OUT 221H %02x\n", x);
        printf("X=");
        for(i = 0x400; i <= 0x40f; i++)
            printf("%02x ", i86->m[i]);
        printf("\n");
        printf("Y=");
        for(i = 0x410; i <= 0x41f; i++)
            printf("%02x ", i86->m[i]);
        printf("\n");
        printf("W=");
        for(i = 0x420; i <= 0x42e; i++)
            printf("%02x ", i86->m[i]);
        printf("\n");
        fflush(stdout);
        */
        /*
        if(x == 0x10) {
            for(i = 0x400; i <= 0x40f; i++)
                i86write8(i86, 0, i, 0);
            for(i = 0x410; i <= 0x41f; i++)
                i86write8(i86, 0, i, 0);
        }
        */
        break;
    default:
        /*
        printf("IO OUT %04x,%02x\n", port, x);
        */
        break;
    }

 return 0;
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
KI2         Q       E       T       U       O       S       log     ln      sin     X2
KI3         RESET   S       F       H       K       ;       MR      M +     (       ENG
KI4         A       D       G       J       L       :       7       8       )       CLS
KI5         CAPS	X       V       N       ,       ?       4       5       9       cos
KI6         Z       C       B       M       INS     DEL     1       2       6       ^
KI7                 SRCH	OUT     SPACE	?       ?       .       3       *       ANS
KI8                 IN      CALC	=       ?       0       E       +       /
KI9                                                 ?       -       BS      tan
KIS                                                                                         SHFT

*/

#define KEY(c)	( pKEYB->keyPressedList.contains(toupper(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(tolower(c)))

UINT16 Cz1::getKey()
{

    UINT8 data=0;

    if ((pKEYB->LastKey) && ks )
    {
        if (ks&1) {
            if (KEY(K_OF))			data|=0x01;
            if (KEY('Q'))			data|=0x02;
            if (KEY('W'))			data|=0x04;
            if (KEY('E'))			data|=0x08;
            if (KEY('R'))			data|=0x10;
            if (KEY('T'))			data|=0x20;
            if (KEY('Y'))			data|=0x40;
            if (KEY('U'))			data|=0x80;
        }
        if (ks&2) {
            if (KEY('A'))			data|=0x01;
            if (KEY('S'))			data|=0x02;
            if (KEY('D'))			data|=0x04;
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
            if (KEY(K_LA))			data|=0x01;
            if (KEY(K_RA))			data|=0x02;
            if (KEY(K_ANS))			data|=0x04;
            if (KEY('0'))			data|=0x08;
            if (KEY('.'))			data|=0x10;
            if (KEY('='))			data|=0x20;
            if (KEY('+'))			data|=0x40;
            if (KEY(K_RET))			data|=0x80;
        }
        if (ks&0x20) {
            if (KEY('L'))			data|=0x01;
            if (KEY(';'))			data|=0x02;
            if (KEY(K_CON))			data|=0x04;
            if (KEY('1'))			data|=0x08;
            if (KEY('2'))			data|=0x10;
            if (KEY('3'))			data|=0x20;
            if (KEY('-'))			data|=0x40;
            if (KEY(K_MPLUS))		data|=0x80;
        }
        if (ks&0x40) {
            if (KEY('I'))			data|=0x01;
            if (KEY('O'))			data|=0x02;
            if (KEY(K_INS))			data|=0x04;
            if (KEY('4'))			data|=0x08;
            if (KEY('5'))			data|=0x10;
            if (KEY('6'))			data|=0x20;
            if (KEY('*'))			data|=0x40;
            if (KEY(K_RM))			data|=0x80;
        }
        if (ks&0x80) {
            if (KEY('P'))			data|=0x01;
            if (KEY(K_F1))			data|=0x02;
            if (KEY(K_PI))			data|=0x04;
            if (KEY('7'))			data|=0x08;
            if (KEY('8'))			data|=0x10;
            if (KEY('9'))			data|=0x20;
            if (KEY('/'))			data|=0x40;
            if (KEY(')'))			data|=0x80;
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
            if (KEY(K_CCE))			data|=0x80;
        }
//        if (fp_log) fprintf(fp_log,"Read key [%02x]: strobe=%02x result=%02x\n",pKEYB->LastKey,ks,data^0xff);

    }
    return data;

}
