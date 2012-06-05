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
    SlotList.append(CSlot(128 , 0xE0000 ,	":/z1/romz1.bin"	, ""	, ROM , "ROM"));
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

    Lcd_X		= 67;
    Lcd_Y		= 63;
    Lcd_DX		= 192;//168;//144 ;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 1;// * 1.18;
    Lcd_ratio_Y	= 1;// * 1.18;

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
    pKEYB		= new Ckeyb(this,"x07.map");



}

Cz1::~Cz1() {

}

bool Cz1::init(void)				// initialize
{

#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();

    for(int i = 0; i < 0x2000; i++)
        mem[i] = i & 0xff;

    return true;
}



bool Cz1::run() {


    CpcXXXX::run();

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

//    if(p < 0x40000)
//        return cpu->m[p]; /* RAM */
//    else if(p < 0xa0000)
//        return p & 0xff;
//    else
    if( (*d >=0xa0000) && (*d < 0xb0000)) {
        pHD66108->readVram(*d);
        return true;
    }
//        return 0;//readVram(cpu, p);
//    else if(p < 0xe0000)
//        return p & 0xff;
//    else
//        return cpu->m[p - 0xe0000 + 0x40000]; /* ROM */
    return true;
}

UINT8 Cz1::in(UINT8 Port)
{
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
//        return (Port == 0x0202 ? v & 0xff: v >> 8);
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

    switch(Port) {
    case 0x0002:
//        *LOW(z1->eoi) = x;
        break;
    case 0x0003:
//        *HIGH(z1->eoi) = x;
        break;
    case 0x0036:
//        *LOW(z1->timer0.control) = x;
        break;
    case 0x0037:
//        *HIGH(z1->timer0.control) = x;
        break;
    case 0x003e:
//        *LOW(z1->timer1.control) = x;
        break;
    case 0x003f:
//        *HIGH(z1->timer1.control) = x;
        break;
    case 0x0046:
//        *LOW(z1->timer2.control) = x;
        break;
    case 0x0047:
//        *HIGH(z1->timer2.control) = x;
        break;
    case 0x00b8:
//        z1->io_b8 = x;
        break;
    case 0x0200:
        ks = ks & 0xff00 | x;
//        *LOW(z1->key.strobe) = x;
        break;
    case 0x0201:
        ks = (x <<8) | (ks & 0xff);
//        *HIGH(z1->key.strobe) = x;
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

