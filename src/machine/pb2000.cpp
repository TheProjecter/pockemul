


#include <QtGui>

#include "common.h"
#include "pb2000.h"
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

#define PD_RES 0x10;	// 1=reset, 0=normal_operation
#define PD_PWR 0x08;	// power control: 1=power_off, 0=power_on
#define PD_STR 0x04;	// transfer direction strobe: 1=write, 0=read
#define PD_ACK 0x01;	// transfer direction acknowledge

Cpb2000::Cpb2000(CPObject *parent)	: Cpb1000(parent)
{								//[constructor]
    setfrequency( (int) 910000/1);
    setcfgfname(QString("pb2000"));

    SessionHeader	= "pb2000PKM";
    SessionHeaderLen= 9;
    Initial_Session_Fname ="pb2000.pkm";

    BackGroundFname	= ":/pb2000/pb-2000.png";
    LcdFname		= ":/pb2000/pb-2000lcd.png";

    SymbFname		= "";

    memsize         = 0x60000;
    InitMemValue	= 0xff;


    SlotList.clear();
    SlotList.append(CSlot(6 , 0x0000 ,	":/pb2000/rom0.bin" , ""	, ROM , "CPU ROM"));

    SlotList.append(CSlot(32, 0x10000 ,	":/pb2000/rom1.bin"	, ""	, ROM , "ROM 1"));

    SlotList.append(CSlot(64, 0x20000 ,	""					, ""	, RAM , "RAM 0"));
    SlotList.append(CSlot(64, 0x30000 ,	""					, ""	, RAM , "RAM 1"));

    SlotList.append(CSlot(64, 0x40000 ,	":/pb2000/rom2.bin"	, ""	, ROM , "ROM 2"));
    SlotList.append(CSlot(64, 0x50000 ,	":/pb2000/rom3.bin"	, ""	, ROM , "ROM 3"));;


    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(188);
    setDYmm(83);
    setDZmm(15);

    setDX(668);//715);
    setDY(294);//465);

    Lcd_X		= 65;
    Lcd_Y		= 45;
    Lcd_DX		= 192;//168;//144 ;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 2;// * 1.18;
    Lcd_ratio_Y	= 2;// * 1.18;

    PowerSwitch = 0;

    delete pLCDC;       pLCDC		= new Clcdc_pb1000(this);
    //pCPU		= new CHD61700(this);
    //pTIMER		= new Ctimer(this);
    //pKEYB		= new Ckeyb(this,"pb2000.map");
    delete pHD44352;    pHD44352    = new CHD44352(":/pb2000/charset.bin",this);

    closed = false;

}

//bool Cpb2000::MemBank(DWORD *d) {
//    if (*d >= 0x0C12) {
////        BYTE m = 1 <<  (address >> 15);
//        //    m := byte(1) shl (cardinal(address) shr 15);
//        //    ga_reg := ptrw(memdef[GATEARRAY].storage)^;
//        //    if (Lo(ga_reg) and m) <> 0 then		{ Gate Array register $0C10 }
//        //      SelectRom := address or $70000		{ ROM2 selected }
//        //    else if (Hi(ga_reg) and m) <> 0 then	{ Gate Array register $0C11 }
//        //      SelectRom := address or $B0000		{ ROM3 selected }
//    }
//}

//function SelectRom (address: integer) : integer;
//var
//  m: byte;
//  ga_reg: word;
//begin
//  SelectRom := address;
//  if address >= $00C12 then
//  begin
//    m := byte(1) shl (cardinal(address) shr 15);
//    ga_reg := ptrw(memdef[GATEARRAY].storage)^;
//    if (Lo(ga_reg) and m) <> 0 then		{ Gate Array register $0C10 }
//      SelectRom := address or $70000		{ ROM2 selected }
//    else if (Hi(ga_reg) and m) <> 0 then	{ Gate Array register $0C11 }
//      SelectRom := address or $B0000		{ ROM3 selected }
//  end {if};
//end {SelectRom};


bool Cpb2000::Chk_Adr(DWORD *d, DWORD data)
{
    if ( (*d>=0x00C00) && (*d<=0x00C0F) )	{
//        pLCDC->Refresh = true;
//        if (pCPU->fp_log) fprintf(pCPU->fp_log,"ECRITURE IO [%04X] = %02x\n",*d,data);
        AddLog(LOG_TEMP,tr("Write Port [%1] = %2").arg(*d&7).arg(data,2,16,QChar('0')));
        return(true);		// RAM area()
    }

    *d += 0x10000;

    if ( (*d>=0x10000) && (*d<0x20000) )	return(true);		// ROM 1 area()
    if ( (*d>=0x20000) && (*d<0x30000) )	return(true);		// RAM 0 area()
    if ( (*d>=0x30000) && (*d<0x40000) )	return(true);		// RAM 1 area()

    if ( (*d>=0x80000) && (*d<0x90000) ) { *d -= 0x40000;	return(true); }		// ROM 2 area()
    if ( (*d>=0xC0000) && (*d<0xD0000) ) { *d -= 0x70000;	return(true); }		// ROM 3 area()

    return false;
}


bool Cpb2000::Chk_Adr_R(DWORD *d, DWORD data)
{
    if ( (*d>=0x00C00) && (*d<=0x00C0F) )	{
//        pLCDC->Refresh = true;
        mem[*d] = 0xff;
        AddLog(LOG_TEMP,tr("Read Port:%1").arg(*d&7));
//        if (pCPU->fp_log) fprintf(pCPU->fp_log,"LECTURE IO [%04X]\n",*d);
        return(true);		// RAM area()
    }

    if ( (*d>=0x20000) && (*d<=0x3FFFF) ) {
//        if (pCPU->fp_log) fprintf(pCPU->fp_log,"LECTURE BANK 1 [%04X]\n",*d);
        return(true);		// RAM area()
    }
//    if (pCPU->fp_log) fprintf(pCPU->fp_log,"LECTURE [%04X]\n",*d);
    return true;
}



#define toupper( a )	(  ((a >= 'a' && a <= 'z') ? a-('a'-'A') : a ) )
#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )
UINT16 Cpb2000::getKey(void) {
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
            if (KEY(K_POW_OFF))          data|=0x20;
            if (KEY(K_BRK))         data|=0x80;
        }

        if (ko&0x02) {
            if (KEY(':'))			data|=0x01;
            if (KEY(';'))			data|=0x02;
            if (KEY('='))			data|=0x04;
            if (KEY('&'))			data|=0x08;
            if (KEY('$'))			data|=0x10;
            if (KEY('"'))			data|=0x20;
            if (KEY(','))			data|=0x80;
            if (KEY(K_MEMO_IN))		data|=0x2000;
            if (KEY(K_IN))			data|=0x4000;
            if (KEY(K_RET))			data|=0x8000;
        }
        if (ko&0x04) {
            if (KEY('Y'))			data|=0x01;
            if (KEY('T'))			data|=0x02;
            if (KEY('R'))			data|=0x04;
            if (KEY('E'))			data|=0x08;
            if (KEY('W'))			data|=0x10;
            if (KEY('Q'))			data|=0x20;
            if (KEY('U'))			data|=0x80;
            if (KEY(K_MEMO))		data|=0x2000;
            if (KEY(K_OUT))			data|=0x4000;
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
            if (KEY(K_CAL))         data|=0x2000;
            if (KEY(K_CALC))		data|=0x4000;
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
            if (KEY(K_NEWALL))		data|=0x04;
            if (KEY(K_INS))			data|=0x08;
            if (KEY(K_STOP))		data|=0x10;
            if (KEY(')'))			data|=0x20;
//            if (KEY(K_CONTRAST))	data|=0x40;
//            if (KEY(K_CONTRAST))	data|=0x80;
            if (KEY(K_LCKEY))		data|=0x2000;
//            if (KEY(K_CONTRAST))	data|=0x4000;
//            if (KEY(K_CONTRAST))	data|=0x8000;
        }

        if (ko&0x40) {
//            if (KEY(K_ENG))			data|=0x01;
            if (KEY('7'))			data|=0x02;
            if (KEY('8'))			data|=0x04;
            if (KEY('9'))			data|=0x08;
            if (KEY('('))			data|=0x10;
            if (KEY('/'))			data|=0x20;
            if (KEY('^'))			data|=0x80;
            if (KEY(K_TS_00))       data|=0x1000;
            if (KEY(K_TS_01))       data|=0x2000;
            if (KEY(K_TS_02))       data|=0x4000;
            if (KEY(K_TS_03))       data|=0x8000;
        }

        if (ko&0x80) {
            if (KEY('O'))			data|=0x01;
            if (KEY('P'))			data|=0x02;
            if (KEY('4'))			data|=0x04;
            if (KEY('5'))			data|=0x08;
            if (KEY('6'))			data|=0x10;
            if (KEY('*'))			data|=0x20;
            if (KEY('I'))			data|=0x80;
            if (KEY(K_TS_10))       data|=0x1000;
            if (KEY(K_TS_11))       data|=0x2000;
            if (KEY(K_TS_12))       data|=0x4000;
            if (KEY(K_TS_13))       data|=0x8000;
        }

        if (ko&0x100) {
            if (KEY('K'))			data|=0x01;
            if (KEY('L'))			data|=0x02;
            if (KEY('1'))			data|=0x04;
            if (KEY('2'))			data|=0x08;
            if (KEY('3'))			data|=0x10;
            if (KEY('+'))			data|=0x20;
            if (KEY('J'))			data|=0x80;
            if (KEY(K_TS_20))       data|=0x1000;
            if (KEY(K_TS_21))       data|=0x2000;
            if (KEY(K_TS_22))       data|=0x4000;
            if (KEY(K_TS_23))       data|=0x8000;
        }

        if (ko&0x200) {
//            if (KEY(K_AA))			data|=0x01;
            if (KEY('0'))			data|=0x02;
            if (KEY('.'))			data|=0x04;
            if (KEY(K_ANS))			data|=0x08;
            if (KEY(K_RET))			data|=0x10;
            if (KEY('-'))			data|=0x20;
            if (KEY(' '))			data|=0x80;
            if (KEY(K_TS_30))       data|=0x1000;
            if (KEY(K_TS_31))       data|=0x2000;
            if (KEY(K_TS_32))       data|=0x4000;
            if (KEY(K_TS_33))       data|=0x8000;
        }

        if (ko&0x400) {
            if (pKEYB->isShift)     data|=0x40;
            if (KEY(K_SHT))			data|=0x40;
            if (KEY(K_SHT2))		data|=0x40;
        }
        if (ko&0x800) {
            if (pKEYB->isCtrl)      data|=0x40;
            if (KEY(K_F1))			data|=0x40;
        }
    }

if (pCPU->fp_log) fprintf(pCPU->fp_log,"%02X\n",data);

    return data;

}

void Cpb2000::TurnCLOSE(void) { }



void Cpb2000::paintEvent(QPaintEvent *event)
{
    CPObject::paintEvent(event);
}
