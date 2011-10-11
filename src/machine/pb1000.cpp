

#include <QtGui>

#include "common.h"
#include "pb1000.h"
#include "hd61700.h"
#include "hd44352.h"

#include "Inter.h"
#include "init.h"
#include "ct6834.h"
#include "uart.h"
#include "Log.h"
#include "Lcdc_pb1000.h"
#include "Keyb.h"
#include "Connect.h"
#include "dialoganalog.h"


#define PRT_DATA       0x00
#define PRT_ACK        0x01
#define MASK_PRT_DATA  0x20
#define MASK_PRT_ACK   0x40

#define INIT_T6834     0xf0
#define LEC_T6834      0xf1
#define LEC_T6834_ACK  0xf2

#define SIO_GET_PIN(n)		pSio->pSIOCONNECTOR->Get_pin(n)
#define SIO_SET_PIN(n,v)	pSio->pSIOCONNECTOR->Set_pin(n,v)


#define SIO_GNDP	1
#define SIO_SD 		2
#define SIO_RD		3
#define SIO_RS		4
#define SIO_CS		5

#define SIO_GND		7
#define SIO_CD		8
#define SIO_VC1		10
#define SIO_RR		11
#define SIO_PAK		12
#define SIO_VC2		13
#define SIO_ER		14
#define SIO_PRQ		15

Cpb1000::Cpb1000(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 910000/3);
    setcfgfname(QString("pb1000"));

    SessionHeader	= "PB1000PKM";
    SessionHeaderLen= 9;
    Initial_Session_Fname ="pb1000.pkm";

    BackGroundFname	= ":/pb1000/pb-1000.png";
    LcdFname		= ":/pb1000/pb-1000lcd.png";
    SymbFname		= ":/pb1000/pb-1000symb.png";

    memsize		= 0x20000;
    InitMemValue	= 0xff;


    SlotList.clear();
    SlotList.append(CSlot(6 , 0x0000 ,	":/pb1000/rom0.bin" , ""	, ROM , "CPU ROM"));
    SlotList.append(CSlot(8 , 0x6000 ,	""					, ""	, RAM , "RAM0"));
    SlotList.append(CSlot(32, 0x8000 ,	":/pb1000/rom1.bin"	, ""	, ROM , "ROM"));
    SlotList.append(CSlot(32, 0x18000 ,	""					, ""	, RAM , "RAM1"));

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(187);//Pc_DX_mm = 200;
    setDYmm(177);//Pc_DY_mm = 130;
    setDZmm(24);//Pc_DZ_mm = 30;

    setDX(668);//715);
    setDY(633);//465);

    Lcd_X		= 90;
    Lcd_Y		= 130;
    Lcd_DX		= 192;//168;//144 ;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 2;// * 1.18;
    Lcd_ratio_Y	= 2;// * 1.18;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;

    PowerSwitch = 0;

    pLCDC		= new Clcdc_pb1000(this);
    pCPU		= new CHD61700(this);
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"pb1000.map");
    pHD44352    = new CHD44352(this);

    m_kb_matrix = 0;


//    Cpt = 0;
//    Nb=0;
//    Lec_K7=0;
//    IT_T6834 = 0;

}

bool Cpb1000::init(void)				// initialize
{

#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();

    return true;
}

bool Cpb1000::run() {


    CpcXXXX::run();

    if (pKEYB->LastKey == K_POW_ON) {
        AddLog(LOG_KEYBOARD,tr("ON Interrupt"));
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"PULSE TIMER\n");
        ((CHD61700*)pCPU)->execute_set_input(HD61700_ON_INT,1);
        pKEYB->LastKey = 0;
    }

    if (pKEYB->LastKey) {
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"NEW KEY\n");
        ((CHD61700*)pCPU)->execute_set_input(HD61700_KEY_INT,1);
    }

}

bool Cpb1000::Chk_Adr(DWORD *d, DWORD data)
{
    if ( (*d>=0x06100) && (*d<=0x061FF) )	{
//        pLCDC->Refresh = true;
        return(true);		// RAM area()
    }
    if ( (*d>=0x06000) && (*d<=0x07FFF) )	return(true);		// RAM area()
    if ( (*d>=0x18000) && (*d<=0x1FFFF) )	return(true);		// RAM area()

    return false;
}

bool Cpb1000::Chk_Adr_R(DWORD *d, DWORD data)
{
    return true;
}

UINT8 Cpb1000::in(UINT8 Port)
{

}

UINT8 Cpb1000::out(UINT8 Port, UINT8 Value)
{

 return 0;
}





void Cpb1000::TurnOFF(void) {
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
    AddLog(LOG_TEMP,"TURN OFF");
}

void Cpb1000::TurnON(void){
    CpcXXXX::TurnON();

    AddLog(LOG_CANON,"TURN ON");

}


void Cpb1000::Reset()
{

    CpcXXXX::Reset();

}

bool Cpb1000::LoadConfig(QXmlStreamReader *xmlIn)
{
//    pT6834->Load_Internal(xmlIn);
    return true;
}

bool Cpb1000::SaveConfig(QXmlStreamWriter *xmlOut)
{
//    pT6834->save_internal(xmlOut);
    return true;
}

#if 0
    PORT_START("KO1")
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("BRK")		PORT_CODE(KEYCODE_F10)
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("OFF")		PORT_CODE(KEYCODE_7_PAD)
    PORT_START("KO2")
        PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EXE")		PORT_CODE(KEYCODE_ENTER)		PORT_CHAR(13)
        PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("IN")		PORT_CODE(KEYCODE_F4)
        PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MEMO IN")	PORT_CODE(KEYCODE_0_PAD)
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(",")		PORT_CODE(KEYCODE_COMMA)		PORT_CHAR(',')	PORT_CHAR('?')
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("'")		PORT_CODE(KEYCODE_QUOTE)		PORT_CHAR('"')	PORT_CHAR('!')
        PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("$")		PORT_CODE(KEYCODE_1_PAD)		PORT_CHAR('$')	PORT_CHAR('#')
        PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("&")		PORT_CODE(KEYCODE_2_PAD)		PORT_CHAR('&')	PORT_CHAR('%')
        PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("=")		PORT_CODE(KEYCODE_EQUALS)		PORT_CHAR('=')	PORT_CHAR('\'')
        PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(";")		PORT_CODE(KEYCODE_COLON)		PORT_CHAR(';')	PORT_CHAR('<')
        PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(":")		PORT_CODE(KEYCODE_BACKSLASH2)	PORT_CHAR(':')	PORT_CHAR('>')
    PORT_START("KO3")
        PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("RIGHT")	PORT_CODE(KEYCODE_RIGHT)
        PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("OUT")		PORT_CODE(KEYCODE_F3)
        PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MEMO")	PORT_CODE(KEYCODE_F11)
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("U")		PORT_CODE(KEYCODE_U)			PORT_CHAR('U')
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Q")		PORT_CODE(KEYCODE_Q)			PORT_CHAR('Q')
        PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("W")		PORT_CODE(KEYCODE_W)			PORT_CHAR('W')
        PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E")		PORT_CODE(KEYCODE_E)			PORT_CHAR('E')
        PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("R")		PORT_CODE(KEYCODE_R)			PORT_CHAR('R')
        PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("T")		PORT_CODE(KEYCODE_T)			PORT_CHAR('T')
        PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y")		PORT_CODE(KEYCODE_Y)			PORT_CHAR('Y')
    PORT_START("KO4")
        PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DOWN")	PORT_CODE(KEYCODE_DOWN)
        PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CALC")	PORT_CODE(KEYCODE_HOME)
        PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CAL")		PORT_CODE(KEYCODE_F9)
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("H")		PORT_CODE(KEYCODE_H)			PORT_CHAR('H')
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CAPS")	PORT_CODE(KEYCODE_CAPSLOCK)
        PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A")		PORT_CODE(KEYCODE_A)        	PORT_CHAR('A')
        PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("S")		PORT_CODE(KEYCODE_S)        	PORT_CHAR('S')
        PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D")		PORT_CODE(KEYCODE_D)        	PORT_CHAR('D')
        PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F")		PORT_CODE(KEYCODE_F)        	PORT_CHAR('F')
        PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("G")		PORT_CODE(KEYCODE_G)        	PORT_CHAR('G')
    PORT_START("KO5")
        PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("UP")		PORT_CODE(KEYCODE_UP)
        PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LEFT")	PORT_CODE(KEYCODE_LEFT)
        PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MENU")	PORT_CODE(KEYCODE_F5)
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M")		PORT_CODE(KEYCODE_M)			PORT_CHAR('M')
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Z")		PORT_CODE(KEYCODE_Z)        	PORT_CHAR('Z')
        PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("X")		PORT_CODE(KEYCODE_X)        	PORT_CHAR('X')
        PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C")		PORT_CODE(KEYCODE_C)        	PORT_CHAR('C')
        PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("V")		PORT_CODE(KEYCODE_V)        	PORT_CHAR('V')
        PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B")		PORT_CODE(KEYCODE_B)        	PORT_CHAR('B')
        PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("N")		PORT_CODE(KEYCODE_N)        	PORT_CHAR('N')
    PORT_START("KO6")
        PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LCKEY")	PORT_CODE(KEYCODE_F1)
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CONTR")	PORT_CODE(KEYCODE_F2)
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(")")		PORT_CODE(KEYCODE_PGDN)     	PORT_CHAR(')')		PORT_CHAR(']')
        PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STOP")	PORT_CODE(KEYCODE_F7)
        PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("INS")		PORT_CODE(KEYCODE_INSERT)
        PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("NEW ALL")	PORT_CODE(KEYCODE_ESC)
        PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("BS")		PORT_CODE(KEYCODE_BACKSPACE)
        PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CLS")		PORT_CODE(KEYCODE_DEL)
    PORT_START("KO7")
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("^")		PORT_CODE(KEYCODE_3_PAD)    	PORT_CHAR('^')
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/")		PORT_CODE(KEYCODE_SLASH)    	PORT_CHAR('/')		PORT_CHAR('}')
        PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(")		PORT_CODE(KEYCODE_PGUP)     	PORT_CHAR('(')		PORT_CHAR('[')
        PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9")		PORT_CODE(KEYCODE_9)    		PORT_CHAR('9')		PORT_CHAR('\'')
        PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8")		PORT_CODE(KEYCODE_8)        	PORT_CHAR('8')		PORT_CHAR('_')
        PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7")		PORT_CODE(KEYCODE_7)        	PORT_CHAR('7')		PORT_CHAR('@')
        PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENG")		PORT_CODE(KEYCODE_F6)
    PORT_START("KO8")
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("I")		PORT_CODE(KEYCODE_I)			PORT_CHAR('I')
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("*")		PORT_CODE(KEYCODE_ASTERISK)		PORT_CHAR('*')		PORT_CHAR('{')
        PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6")		PORT_CODE(KEYCODE_6)			PORT_CHAR('6')		PORT_CHAR('\\')
        PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5")		PORT_CODE(KEYCODE_5)			PORT_CHAR('5')		PORT_CHAR('~')
        PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4")		PORT_CODE(KEYCODE_4)			PORT_CHAR('4')		PORT_CHAR('|')
        PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("P")		PORT_CODE(KEYCODE_P)			PORT_CHAR('P')
        PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("O")		PORT_CODE(KEYCODE_O)			PORT_CHAR('O')
    PORT_START("KO9")
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("J")		PORT_CODE(KEYCODE_J)			PORT_CHAR('J')
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+")		PORT_CODE(KEYCODE_PLUS_PAD)		PORT_CHAR('+')
        PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3")		PORT_CODE(KEYCODE_3)			PORT_CHAR('3')
        PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2")		PORT_CODE(KEYCODE_2)			PORT_CHAR('2')
        PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1")		PORT_CODE(KEYCODE_1)			PORT_CHAR('1')
        PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("L")		PORT_CODE(KEYCODE_L)			PORT_CHAR('L')
        PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("K")		PORT_CODE(KEYCODE_K)			PORT_CHAR('K')
    PORT_START("KO10")
        PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SPACE")	PORT_CODE(KEYCODE_SPACE)		PORT_CHAR(' ')
        PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-")		PORT_CODE(KEYCODE_MINUS)		PORT_CHAR('-')
        PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EXE")		PORT_CODE(KEYCODE_ENTER)
        PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ANS")		PORT_CODE(KEYCODE_END)
        PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".")		PORT_CODE(KEYCODE_STOP)			PORT_CHAR('.')
        PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0")		PORT_CODE(KEYCODE_0)			PORT_CHAR('0')
        PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("??")		PORT_CODE(KEYCODE_5_PAD)
    PORT_START("KO11")
        PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SHIFT")	PORT_CODE(KEYCODE_LSHIFT)		PORT_CHAR(UCHAR_SHIFT_1)
    PORT_START("KO12")
        PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F")		PORT_CODE(KEYCODE_LALT)
    PORT_START("NULL")
        PORT_BIT(0xffff, IP_ACTIVE_HIGH, IPT_UNUSED)

#endif

#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )
UINT16 Cpb1000::getKey(void) {
    DWORD ko = 0;
    UINT16 data = 0;

    switch (m_kb_matrix) {
    case 0: return 0;
    case 13: ko = 0xffff; break;
    case 14:
    case 15: ko = 0; break;
    default: ko = (1<<(m_kb_matrix-1)); break;
    }

    if ((pKEYB->LastKey) )
    {

AddLog(LOG_KEYBOARD,tr("GetKEY : %1").arg(ko,4,16,QChar('0')));
        if (ko&1) {
//            if (KEY(K_EXTMENU))     data|=0x01;
//            if (KEY('Q'))			data|=0x02;
//            if (KEY(K_MENU))		data|=0x04;
//            if (KEY('A'))			data|=0x08;
//            if (KEY(K_BASIC))		data|=0x10;
//            if (KEY('Z'))			data|=0x20;
////            if (KEY(K_SHT))			data|=0x40;
//            if (KEY(K_CTRL))		data|=0x80;			// UP ARROW
        }

        if (ko&0x04) {
            if (KEY('Y'))			data|=0x01;
            if (KEY('T'))			data|=0x02;
            if (KEY('R'))			data|=0x04;
            if (KEY('E'))			data|=0x08;
            if (KEY('W'))			data|=0x10;
            if (KEY('Q'))			data|=0x20;
            if (KEY('U'))			data|=0x80;
//            if (KEY(K_MEMO))		data|=0x2000;
//            if (KEY(K_OUT))			data|=0x4000;
            if (KEY(K_RA))			data|=0x8000;
        }
        if (ko&0x08) {
            if (KEY('G'))			data|=0x01;
            if (KEY('F'))			data|=0x02;
            if (KEY('D'))			data|=0x04;
            if (KEY('S'))			data|=0x08;
            if (KEY('A'))			data|=0x10;
            if (KEY(K_SML))			data|=0x20;
            if (KEY('H'))			data|=0x80;
            if (KEY(K_CAL))		data|=0x2000;
//            if (KEY(K_CALC))			data|=0x4000;
            if (KEY(K_DA))			data|=0x8000;
        }
        if (ko&0x10) {
            if (KEY('N'))			data|=0x01;
            if (KEY('B'))			data|=0x02;
            if (KEY('V'))			data|=0x04;
            if (KEY('C'))			data|=0x08;
            if (KEY('X'))			data|=0x10;
            if (KEY('Z'))			data|=0x20;
            if (KEY('M'))			data|=0x80;
            if (KEY(K_MENU))		data|=0x2000;
            if (KEY(K_LA))			data|=0x4000;
            if (KEY(K_UA))			data|=0x8000;
        }

        if (ko&0x20) {
            if (KEY(K_CLR))			data|=0x01;
            if (KEY(K_BS))			data|=0x02;
//            if (KEY(K_NEWALL))		data|=0x04;
            if (KEY(K_INS))			data|=0x08;
            if (KEY(K_STOP))		data|=0x10;
            if (KEY(')'))			data|=0x20;
//            if (KEY(K_CONTRAST))	data|=0x80;
//            if (KEY(K_LCKEY))		data|=0x2000;
        }

        if (ko&0x40) {
//            if (KEY(K_ENG))			data|=0x01;
            if (KEY('7'))			data|=0x02;
            if (KEY('8'))			data|=0x04;
            if (KEY('9'))			data|=0x08;
            if (KEY('('))			data|=0x10;
            if (KEY('/'))			data|=0x20;
            if (KEY('^'))			data|=0x80;
        }

        if (ko&0x80) {
            if (KEY('O'))			data|=0x01;
            if (KEY('P'))			data|=0x02;
            if (KEY('4'))			data|=0x04;
            if (KEY('5'))			data|=0x08;
            if (KEY('6'))			data|=0x10;
            if (KEY('*'))			data|=0x20;
            if (KEY('I'))			data|=0x80;
        }

        if (ko&0x100) {
            if (KEY('K'))			data|=0x01;
            if (KEY('L'))			data|=0x02;
            if (KEY('1'))			data|=0x04;
            if (KEY('2'))			data|=0x08;
            if (KEY('3'))			data|=0x10;
            if (KEY('+'))			data|=0x20;
            if (KEY('J'))			data|=0x80;
        }

        if (ko&0x200) {
//            if (KEY(K_AA))			data|=0x01;
            if (KEY('0'))			data|=0x02;
            if (KEY('.'))			data|=0x04;
//            if (KEY(K_ANS))			data|=0x08;
            if (KEY(K_RET))			data|=0x10;
            if (KEY('-'))			data|=0x20;
            if (KEY(' '))			data|=0x80;
        }
    }



    return data;

}

void Cpb1000::setKey(UINT8 data) {
    AddLog(LOG_KEYBOARD,tr("set matrix to %1").arg(data,2,16,QChar('0')));
    m_kb_matrix = data;
}
