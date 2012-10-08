#include "e500.h"
#include "sc62015.h"
#include "hd61102.h"
#include "Connect.h"
#include "Keyb.h"
#include "Lcdc_e500.h"
#include "Inter.h"
#include "Log.h"
#include "init.h"
#include "cextension.h"

//TODO: Connector output for ce-126p and ce-140f
//TODO: Real Time Clock
//TODO: UART Emulation
//TODO: Memory cards management

/*
00000   -----------------------
        |                     | you can peek ( 00000 - 000FF ) by PEEK ,
        |   None.             | but it is not memory. It is cpu's internal ram.
        |                     |
        |                     |
40000H  +---------------------+---------+---------+8KB CARD+
        |                     |32KB CARD|16KB CARD+--------+
        |   64KB CARD         |         +---------+
        |                     +---------+
        |                     |
        |                     |
50000H  +---------------------+
        |                     | PC-E500 has 32kb ( B8000 - BFFFF )
        |   BUILDIN RAM       | PC-E550 has 64kb ( B0000 - BFFFF )
        |           SLOT 1    | ram slot "S1:" and ram drive "E:"
        |                     |
C0000   -----------------------
        |                     |
        |   SYSTEM ROM        | rom slot "S3:" ( C0000 - DFFFF ) and
        |                     | rom drive "G:"
        |                     |
FFFFF   -----------------------

*/

Ce500::Ce500(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 3072000/3);
    setcfgfname(QString("e500"));

    SessionHeader	= "E500PKM";
    Initial_Session_Fname ="e500.pkm";

    BackGroundFname	= ":/e500/pc-e500.png";
    LcdFname		= ":/e500/e500lcd.png";
    SymbFname		= ":/e500/e500symb.png";

    memsize		= 0x100000;
    InitMemValue	= 0xff;
    /* ROM area(c0000-fffff) S3: */
    SlotList.clear();
    SlotList.append(CSlot(256, 0x40000 , ""             , ""            , RAM , "RAM S1"));
    SlotList.append(CSlot(256, 0x80000 , ""             , ""            , RAM , "RAM S2"));
    SlotList.append(CSlot(256, 0xC0000 , ":/e500/s3.rom", "e500/s3.rom" , ROM , "ROM S3"));


    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(200);
    setDYmm(100);
    setDZmm(14);

    setDX(715);
    setDY(357);

    Lcd_X		= 69;
    Lcd_Y		= 99;
    Lcd_DX		= 240;//168;//144 ;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 348.0/240;
    Lcd_ratio_Y	= 60.0/32;

    Lcd_Symb_X	= 69;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 79;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 348;
    Lcd_Symb_DY	= 20;
    Lcd_Symb_ratio_X	= 1;//1.18;

    pLCDC		= new Clcdc_e500(this);
    pCPU		= new Csc62015(this);
    pTIMER		= new Ctimer(this);
    pCONNECTOR	= new Cconnector(this,11,0,Cconnector::Sharp_11,"Connector 11 pins",false,QPoint(1,87));		publish(pCONNECTOR);
    pKEYB		= new Ckeyb(this,"e500.map");

    pHD61102_1  = new CHD61102(this);
    pHD61102_2  = new CHD61102(this);

    start2khz = 0;
    start4khz = 0;
    Xin=Xout=false;

}

void	Ce500::initExtension(void)
{
    AddLog(LOG_MASTER,"INIT EXT PC-E500");
    // initialise ext_MemSlot1
    ext_MemSlot1 = new CExtensionArray("RAM Slot","Add RAM Module");
    ext_MemSlot1->setAvailable(ID_CE210M,true);
    ext_MemSlot1->setAvailable(ID_CE211M,true);
    ext_MemSlot1->setAvailable(ID_CE212M,true);
    ext_MemSlot1->setAvailable(ID_CE2H16M,true);
    ext_MemSlot1->setAvailable(ID_CE2H32M,true);
    ext_MemSlot1->setAvailable(ID_CE2H64M,true);

    ext_MemSlot1->setChecked(ID_CE2H64M,true);
    addExtMenu(ext_MemSlot1);

    extensionArray[0] = ext_MemSlot1;

}

bool Ce500::init(void) {
//    pCPU->logsw = true;
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    initExtension();

    CpcXXXX::init();


    WatchPoint.remove(this);
    WatchPoint.add(&pCONNECTOR_value,64,11,this,"Standard 11pins connector");

//	if(emsmode!=0) EMS_Load();

    return true;
}

bool Ce500::run(void) {
    CpcXXXX::run();



    getKey();
    Csc62015 * sc = (Csc62015*)pCPU;

    if((sc->get_imem(IMEM_IMR)&0x80)&&(sc->get_imem(IMEM_IMR) & sc->get_imem(IMEM_ISR))) {
        sc->set_reg(REG_S,sc->get_reg(REG_S)-SIZE_20-2);								//go interrupt routine
        sc->set_mem(sc->get_reg(REG_S)+2,SIZE_20,sc->get_reg(REG_P));
        sc->set_mem(sc->get_reg(REG_S)+1,SIZE_8,sc->get_reg(REG_F));
        sc->set_mem(sc->get_reg(REG_S),SIZE_8,sc->get_imem(IMEM_IMR));
        sc->opr_imem(IMEM_IMR,OPR_AND,0x7f);
        sc->set_reg(REG_P,sc->get_mem(VECT_IR,SIZE_20));
    }

    // SOUND
    //
    computeSound();

    return true;
}

INLINE void Ce500::computeSound(void)
{
    Csc62015 * sc = (Csc62015*)pCPU;

    qint64 delta;
    qint64 wait2khz = getfrequency()/1000/4;
    qint64 wait4khz = getfrequency()/1000/8;

    switch ( (sc->get_imem(IMEM_SCR)>>4) & 0x07)
    {
        case 0x00 : Xout = false;
                    start2khz = 0;
                    start4khz = 0;
                    //if (fp_log) fprintf(fp_log,"XOUT LOW\n");
                    break;

        case 0x01 : Xout = true;
                    start2khz = 0;
                    start4khz = 0;
                    //if (fp_log) fprintf(fp_log,"XOUT HIGH\n");
                    break;

        case 0x02 : // 2khz
                    start4khz = 0;
                    //if (fp_log) fprintf(fp_log,"XOUT 2Khz\n");
                    if (start2khz == 0){
                        start2khz = pTIMER->state;
                        if (fp_log) fprintf(fp_log,"XOUT 2Khz INIT\n");
                        Xout = true;
                    }
                    delta = pTIMER->state - start2khz;
                    //while
                    if ((pTIMER->state - start2khz) >= wait2khz){
                        Xout = !Xout;
                        start2khz += wait2khz;
                        if (fp_log) fprintf(fp_log,"XOUT 2Khz switch\n");
                    }
                    break;

        case 0x03 : // 4khz
                    start2khz = 0;
                    //if (fp_log) fprintf(fp_log,"XOUT 4Khz\n");
                    if (start4khz==0)
                    {
                        start4khz = pTIMER->state;
                        if (fp_log) fprintf(fp_log,"XOUT 4Khz INIT\n");
                        Xout = true;
                    }
                    delta = pTIMER->state - start4khz;
                    //while
                    if (( pTIMER->state - start4khz) >= wait4khz)
                    {
                        Xout = !Xout;
                        start4khz += wait4khz;
//                        if (fp_tmp) fprintf(fp_tmp,"%s\n",tr("switch XOUT to %1 : wait = %2  -  delta=%3  new:%4 - old:%5 ").arg(Xout).arg(wait4khz).arg(pPC->pTIMER->state - start4khz).arg(pPC->pTIMER->state).arg(start4khz).toLocal8Bit().data());


                        if (fp_log) fprintf(fp_log,"XOUT 4Khz switch\n");
                    }
                    break;

        case 0x04 : Xout = false;
                    start2khz = 0;
                    start4khz = 0;
                    //if (fp_log) fprintf(fp_log,"XOUT LOW");
                    break;

        case 0x05 : Xout = true;
                    start2khz = 0;
                    start4khz = 0;
                    //if (fp_log) fprintf(fp_log,"XOUT HIGH\n");
                    break;

        case 0x06 :
        case 0x07 : // Xin -> Xout
                    Xout = Xin;
                    start2khz = 0;
                    start4khz = 0;
                    break;
    }

    fillSoundBuffer((Xout?0xff:0x00));
}


#define GET_IMEM_BIT(adr,Bit) ((pCPU->imem[adr] & (1<<((Bit)-1))) ? 1:0)

bool Ce500::Set_Connector(void)
{
    for (int i=1;i<=8;i++) {
        pCONNECTOR->Set_pin(i	,GET_IMEM_BIT(IMEM_EOL,i));
    }
    for (int i=1;i<=3;i++) {
        pCONNECTOR->Set_pin(8+i	,GET_IMEM_BIT(IMEM_EOH,i));
    }

    return(1);
}

bool Ce500::Get_Connector(void)
{
//	Set_Port_Bit(PORT_B,8,pCONNECTOR->Get_pin(PIN_D_IN));	// DIN	:	IB7
//    pCPU->setImemBit(IMEM_EIL,8,pCONNECTOR->Get_pin(PIN_D_IN));
    pCPU->setImemBit(IMEM_EIL,8,pCONNECTOR->Get_pin(PIN_ACK));
//	pCPU->Set_Xin(pCONNECTOR->Get_pin(PIN_MT_IN));

    return(1);
}

void Ce500::disp(qint8 cmd,DWORD data)
{
    switch(cmd){
    case   6:							/* LCDC 2 write data */
        pHD61102_2->instruction(0x100 | data);
        pLCDC->Refresh = true;
        break;
    case 0xa:							/* LCDC 1 write data */
        pHD61102_1->instruction(0x100 | data);
        pLCDC->Refresh = true;
        break;
    case   4:							/* LCDC 2 inst */
        pHD61102_2->instruction(data);
        pLCDC->Refresh = true;
        break;
    case   8:							/* LCDC 1 inst */
        pHD61102_1->instruction(data);
        pLCDC->Refresh = true;
        break;
    case   0:							/* LCDC 1&2 inst */
        pHD61102_1->instruction(data);
        pHD61102_2->instruction(data);
        pLCDC->Refresh = true;
        break;
    case   2:							/* LCDC 1&2 write data */
        pHD61102_1->instruction(0x100|data);
        pHD61102_2->instruction(0x100|data);
        pLCDC->Refresh = true;
        break;
    case   7:							/* LCDC 2 read data */
        mem[0x2007] = pHD61102_2->instruction(0x300);
        break;
    case 0xb:							/* LCDC 1 read data */
        mem[0x200b] = pHD61102_1->instruction(0x300);
        break;
    }
}

/*---------------------------------------------------------------------------*/
/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/

void Ce500::MemMirror(DWORD *d) {

    if ((ext_MemSlot1->ExtArray[ID_CE210M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE211M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE212M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE2H16M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE2H32M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE2H64M]->IsChecked) &&
            (*d>=0x40000) && (*d<=0xB7FFF))
    {
        if (ext_MemSlot1->ExtArray[ID_CE2H64M]->IsChecked) {
            *d = (*d & 0xffff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE2H32M]->IsChecked) {
            *d = (*d & 0x7fff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE2H16M]->IsChecked) {
            *d = (*d & 0x3fff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE212M]->IsChecked) {
            *d = (*d & 0x1fff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE211M]->IsChecked) {
            *d = (*d & 0xfff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE210M]->IsChecked) {
            *d = (*d & 0x7ff) | 0x40000;
        }
    }
    // 32Ko internal
    else if ( (*d>=0x80000) && (*d<=0xB7FFF)) {
//        quint32 tmp = *d;
        *d = (*d & 0x7fff) | 0xB8000;
//        AddLog(LOG_MASTER,QString("adr;%1 -> %2").arg(tmp,6,16,QChar('0')).arg(*d,6,16,QChar('0')));
    }
}

void Ce550::MemMirror(DWORD *d)
{
    if ((ext_MemSlot1->ExtArray[ID_CE210M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE211M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE212M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE2H16M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE2H32M]->IsChecked ||
         ext_MemSlot1->ExtArray[ID_CE2H64M]->IsChecked) &&
            (*d>=0x40000) && (*d<=0xB7FFF))
    {
        if (ext_MemSlot1->ExtArray[ID_CE2H64M]->IsChecked) {
            *d = (*d & 0xffff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE2H32M]->IsChecked) {
            *d = (*d & 0x7fff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE2H16M]->IsChecked) {
            *d = (*d & 0x3fff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE212M]->IsChecked) {
            *d = (*d & 0x1fff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE211M]->IsChecked) {
            *d = (*d & 0xfff) | 0x40000;
        }
        else
        if (ext_MemSlot1->ExtArray[ID_CE210M]->IsChecked) {
            *d = (*d & 0x7ff) | 0x40000;
        }
    }
    else
    // 64Ko internal
    if ( (*d>=0x80000) && (*d<=0xAFFFF)) {
        *d = (*d & 0xffff) | 0xB0000;
    }
}

bool Ce500::Chk_Adr(DWORD *d,DWORD data)
{
    quint32 tmp = *d;
    MemMirror(d);

    if ( (tmp>=0x40000) && (tmp<=0xBFFFF) && (pCPU->fp_log)) fprintf(pCPU->fp_log,"\nRAM WRITE: [%06X] -> [%06X]=%02X\n",tmp,*d,data);

    if ( (*d>=0x00000) && (*d<=0x3FFFF)) {
        if((*d&0x6000)==0x2000){
            *d&=0x200f; disp(*d&15,data);//lcdc.access=1; lcdc.lcdcadr=*d&15;
            return(1-(*d&1));			/* LCDC (0200x) */
        }
        return 1;
    }

    if ( (*d>=0x40000) && (*d<=0x4FFFF)) return (ext_MemSlot1->ExtArray[ID_CE210M]->IsChecked ||
                                                 ext_MemSlot1->ExtArray[ID_CE211M]->IsChecked ||
                                                 ext_MemSlot1->ExtArray[ID_CE212M]->IsChecked ||
                                                 ext_MemSlot1->ExtArray[ID_CE2H16M]->IsChecked ||
                                                 ext_MemSlot1->ExtArray[ID_CE2H32M]->IsChecked ||
                                                 ext_MemSlot1->ExtArray[ID_CE2H64M]->IsChecked);
    if ( (*d>=0x80000) && (*d<=0xB7FFF)) {
        AddLog(LOG_RAM,QString("adr;%1").arg(*d,6,16,QChar('0')));
    }
    if ( (*d>=0xB8000) && (*d<=0xBFFFF)) return 1;
    if ( (*d>=0xC0000) && (*d<=0xFFFFF)) return 0;

//    if(*d>0xbffff) return(0);			/* ROM area(c0000-fffff) S3: */
//    if(*d>0x7ffff) return(1);			/* RAM area(80000-bffff) S1: */
//    if(*d>0x3ffff) return(1);			/* RAM area(40000-7ffff) S2: */


#if 0

    if(*d>0x1ffff){
//        if(sc.e6) return(0);			/* ROM area(20000-3ffff) ->E650/U6000 */
//        else{
//            *d=BASE_128[GetBank()]+(*d&0x1ffff);
//            return(1-(sc.emsmode>>4));		/* RAM area(20000-3ffff) EMS */
//        }
    }
    if(*d>0x0ffff){
        *d=BASE_64[GetBank()]+(*d&0xffff);
        return(1-(sc.emsmode>>4));			/* RAM area(10000-1ffff) EMS */
    }
    if((*d&0xf000)==0xe000){
        *d&=0xf005; ssfdc.access=1; ssfdc.adr=*d;
        return(1);					/* SSFDC (0e00x) */
    }

    if((*d&0x3000)==0x1000){
        *d&=0x103f; rtc.access=1; rtc.adr=*d&31;
        return((*d&0x10)==0);		/* CLOCK (010xx) */
    }
#endif
    return false;
}

/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/
bool Ce500::Chk_Adr_R(DWORD *d,DWORD data)
{
    quint32 tmp = *d;
    MemMirror(d);
    if ( (tmp>=0x40000) && (tmp<=0xBFFFF) && (pCPU->fp_log)) fprintf(pCPU->fp_log,"\nRAM READ : [%06X] -> [%06X]\n",tmp,*d);

//    if ( (*d>=0xB0000) && (*d<=0xB7FFF)) { *d += 0x8000;return 1;}


    if(*d>0xbffff) return(1);			/* ROM area(c0000-fffff) S3: */
    if(*d>0x7ffff) return(1);			/* RAM area(80000-bffff) S1: */
    if(*d>0x3ffff) return(1);			/* RAM area(40000-7ffff) S2: */

    if((*d&0x6000)==0x2000){
        *d&=0x200f; disp(*d&15,data);//pLCDC->SetDirtyBuf(pLCDC->SetDirtyBuf(*d & 15));
        return(1);//-(*d&1));			/* LCDC (0200x) */
    }
#if 0
    if(*d>0x1ffff){
        if(sc.e6) return(0);			/* ROM area(20000-3ffff) ->E650/U6000 */
        else{
            *d=BASE_128[GetBank()]+(*d&0x1ffff);
            return(1-(sc.emsmode>>4));		/* RAM area(20000-3ffff) EMS */
        }
    }
    if(*d>0x0ffff){
        *d=BASE_64[GetBank()]+(*d&0xffff);
        return(1-(sc.emsmode>>4));			/* RAM area(10000-1ffff) EMS */
    }
    if((*d&0xf000)==0xe000){
        *d&=0xf005; ssfdc.access=1; ssfdc.adr=*d;
        return(1);					/* SSFDC (0e00x) */
    }

    if((*d&0x3000)==0x1000){
        *d&=0x103f; rtc.access=1; rtc.adr=*d&31;
        return((*d&0x10)==0);		/* CLOCK (010xx) */
    }
#endif
    return true;
}



BYTE Ce500::Get_PortA()
{
    return 0;
}

BYTE Ce500::Get_PortB()
{
    return 0;
}

void Ce500::TurnON()
{
    if (!Power && pKEYB->LastKey == K_BRK) {
        AddLog(LOG_MASTER,"TURN ON");
        if (!hardreset) {
            Initial_Session_Load();
        }
        else hardreset = false;
        off = 0;
        Power = true;
        PowerSwitch = PS_RUN;
        if (pLCDC) pLCDC->TurnON();
        this->Reset();
    }
}

void Ce500::TurnOFF()
{
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
}

bool Ce500::LoadExtra(QFile *)
{
    return true;
}

bool Ce500::SaveExtra(QFile *)
{
    return true;
}

UINT8 Ce500::in(UINT8 address)
{
    return 0;
}

UINT8 Ce500::out(UINT8 address, UINT8 value)
{
    return 0;
}

#if 0

E5KeyTbl	E5_KeyTbl[]={
        0x00,0x01,		/* (��) */				/* 0 */
        0x00,0x02,		/* Q */
        0x00,0x04,		/* (MENU) */
        0x00,0x08,		/* A */
        0x00,0x10,		/* (BASIC) */
        0x00,0x20,		/* Z */
        0x00,0x40,		/* (SHIFT) */
        0x00,0x80,		/* (CTRL) */
        0x01,0x01,		/* W */
        0x01,0x02,		/* E */
        0x01,0x04,		/* S */					/* 10 */
        0x01,0x08,		/* D */
        0x01,0x10,		/* X */
        0x01,0x20,		/* C */
        0x01,0x40,		/* (CAPS) */
        0x01,0x80,		/* (KANA) */
        0x02,0x01,		/* R */
        0x02,0x02,		/* T */
        0x02,0x04,		/* F */
        0x02,0x08,		/* G */
        0x02,0x10,		/* V */					/* 20 */
        0x02,0x20,		/* B */
        0x02,0x40,		/* (SPACE) */
        0x02,0x80,		/* (DOWN) */
        0x03,0x01,		/* Y */
        0x03,0x02,		/* U */
        0x03,0x04,		/* H */
        0x03,0x08,		/* J */
        0x03,0x10,		/* N */
        0x03,0x20,		/* M */
        0x03,0x40,		/* (UP) */				/* 30 */
        0x03,0x80,		/* (LEFT) */
        0x04,0x01,		/* I */
        0x04,0x02,		/* O */
        0x04,0x04,		/* K */
        0x04,0x08,		/* L */
        0x04,0x10,		/* , */
        0x04,0x20,		/* ; */
        0x04,0x40,		/* (RIGHT) */
        0x04,0x80,		/* (RETURN) */
        0x05,0x01,		/* (RCL) */				/* 40 */
        0x05,0x02,		/* (hyp) */
        0x05,0x04,		/* (HEX) */
        0x05,0x08,		/* (EXP) */
        0x05,0x10,		/* 7 */
        0x05,0x20,		/* 4 */
        0x05,0x40,		/* 1 */
        0x05,0x80,		/* 0 */
        0x06,0x01,		/* (STO) */
        0x06,0x02,		/* (sin) */
        0x06,0x04,		/* (DEG) */				/* 50 */
        0x06,0x08,		/* (Y^x) */
        0x06,0x10,		/* 8 */
        0x06,0x20,		/* 5 */
        0x06,0x40,		/* 2 */
        0x06,0x80,		/* (+/-) */
        0x07,0x01,		/* (C.CE) */
        0x07,0x02,		/* (cos) */
        0x07,0x04,		/* (ln) */
        0x07,0x08,		/* (ROOT) */
        0x07,0x10,		/* 9 */					/* 60 */
        0x07,0x20,		/* 6 */
        0x07,0x40,		/* 3 */
        0x07,0x80,		/* . */
        0x08,0x01,		/* (<->) */
        0x08,0x02,		/* (tan) */
        0x08,0x04,		/* (log) */
        0x08,0x08,		/* (X^2) */
        0x08,0x10,		/* / */
        0x08,0x20,		/* * */
        0x08,0x40,		/* - */					/* 70 */
        0x08,0x80,		/* + */
        0x09,0x01,		/* ) */
        0x09,0x02,		/* (FSE) */
        0x09,0x04,		/* (1/X) */
        0x09,0x08,		/* ( */
        0x09,0x10,		/* (DEL) */
        0x09,0x20,		/* (BS) */
        0x09,0x40,		/* (INS) */
        0x09,0x80,		/* = */
        0x0a,0x01,		/* P */					/* 80 */
        0x0a,0x02,		/* (2ndF) */
        0x0a,0x04,		/* (PF5) */
        0x0a,0x08,		/* (PF4) */
        0x0a,0x10,		/* (PF3) */
        0x0a,0x20,		/* (PF2) */
        0x0a,0x40,		/* (PF1) */
        0x0a,0x80,	/*  */
        0x0b,0x01,		/* (OFF) */
        0x0b,0x02,		/* (ON) */
        0x0b,0x04,	/*  */						/* 90 */
        0x0b,0x08,	/*  */
        0x0b,0x10,	/*  */
        0x0b,0x20,	/* <LOW BATT> */
        0x0b,0x40,	/* <DEBUG> */
        0x0b,0x80,	/* <QUIT> */
        0x00,0x00};		/* [default] */			/* 96 */

#endif

#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )


BYTE Ce500::getKey()
{
    UINT8 data=0;

    DWORD ks = (pCPU->imem[IMEM_KOH]<<8)+pCPU->imem[IMEM_KOL];

//    if ((pKEYB->LastKey) )
    {
        AddLog(LOG_KEYBOARD,tr("GetKEY : %1").arg(ks,4,16,QChar('0')));
        if (ks&1) {
            if (KEY(K_EXTMENU))     data|=0x01;
            if (KEY('Q'))			data|=0x02;
            if (KEY(K_MENU))		data|=0x04;
            if (KEY('A'))			data|=0x08;
            if (KEY(K_BASIC))		data|=0x10;
            if (KEY('Z'))			data|=0x20;
            if (pKEYB->isShift)		data|=0x40;
            if (KEY(K_CTRL))		data|=0x80;			// UP ARROW
        }
        if (ks&2) {
            if (KEY('W'))			data|=0x01;
            if (KEY('E'))			data|=0x02;
            if (KEY('S'))			data|=0x04;			// OFF
            if (KEY('D'))			data|=0x08;
            if (KEY('X'))			data|=0x10;
            if (KEY('C'))			data|=0x20;
            if (KEY(K_SML))			data|=0x40;
            if (KEY(K_KANA))		data|=0x80;
        }
        if (ks&4) {
            if (KEY('R'))			data|=0x01;
            if (KEY('T'))			data|=0x02;
            if (KEY('F'))			data|=0x04;
            if (KEY('G'))			data|=0x08;
            if (KEY('V'))			data|=0x10;
            if (KEY('B'))			data|=0x20;
            if (KEY(' '))			data|=0x40;
            if (KEY(K_DA))			data|=0x80;
        }
        if (ks&8) {
            if (KEY('Y'))			data|=0x01;
            if (KEY('U'))			data|=0x02;
            if (KEY('H'))			data|=0x04;
            if (KEY('J'))			data|=0x08;
            if (KEY('N'))			data|=0x10;
            if (KEY('M'))			data|=0x20;
            if (KEY(K_UA))			data|=0x40;
            if (KEY(K_LA))			data|=0x80;
        }
        if (ks&0x10) {
            if (KEY('I'))			data|=0x01;			// +
            if (KEY('O'))			data|=0x02;			// *
            if (KEY('K'))			data|=0x04;			// /
            if (KEY('L'))			data|=0x08;
            if (KEY(','))			data|=0x10;			// Key F2
            if (KEY(';'))			data|=0x20;
            if (KEY(K_RA))			data|=0x40;
            if (KEY(K_RET))			data|=0x80;
        }
        if (ks&0x20) {
            if (KEY(K_RCL))			data|=0x01;			// =
            if (KEY(K_HYP))			data|=0x02;			// LEFT ARROW
            if (KEY(K_HEX))			data|=0x04;
            if (KEY(K_EXP))			data|=0x08;
            if (KEY('7'))			data|=0x10;
            if (KEY('4'))			data|=0x20;
            if (KEY('1'))			data|=0x40;
            if (KEY('0'))			data|=0x80;
        }
        if (ks&0x40) {
            if (KEY(K_STO))			data|=0x01;			// R ARROW
            if (KEY(K_SIN))			data|=0x02;			// MODE
            if (KEY(K_DEG))			data|=0x04;			// CLS
            if (KEY(K_POT))			data|=0x08;
            if (KEY('8'))			data|=0x10;
            if (KEY('5'))			data|=0x20;
            if (KEY('2'))			data|=0x40;
            if (KEY(K_SIGN))			data|=0x80;
        }
        if (ks&0x80) {
            if (KEY(K_CCE))			data|=0x01;
            if (KEY(K_COS))			data|=0x02;
            if (KEY(K_LN))			data|=0x04;
            if (KEY(K_ROOT))		data|=0x08;
            if (KEY('9'))			data|=0x10;			// Key F4
            if (KEY('6'))			data|=0x20;
            if (KEY('3'))			data|=0x40;
            if (KEY('.'))			data|=0x80;			// DOWN ARROW
        }
        if (ks&0x100) {
            if (KEY(K_STAT))		data|=0x01;
            if (KEY(K_TAN))			data|=0x02;
            if (KEY(K_LOG))			data|=0x04;
            if (KEY(K_SQR))         data|=0x08;
            if (KEY('/'))			data|=0x10;			// Key F4
            if (KEY('*'))			data|=0x20;
            if (KEY('-'))			data|=0x40;
            if (KEY('+'))			data|=0x80;			// DOWN ARROW
        }
        if (ks&0x200) {
            if (KEY(')'))			data|=0x01;
            if (KEY(K_FSE))			data|=0x02;
            if (KEY(K_1X))			data|=0x04;
            if (KEY('('))   		data|=0x08;
            if (KEY(K_DEL))			data|=0x10;			// Key F4
            if (KEY(K_BS))			data|=0x20;
            if (KEY(K_INS))			data|=0x40;
            if (KEY('='))			data|=0x80;			// DOWN ARROW
        }
        if (ks&0x400) {
            if (KEY('P'))			data|=0x01;
            if (KEY(K_SHT2))		data|=0x02;
            if (KEY(K_F5))			data|=0x04;
            if (KEY(K_F4))			data|=0x08;
            if (KEY(K_F3))			data|=0x10;
            if (KEY(K_F2))			data|=0x20;
            if (KEY(K_F1))			data|=0x40;			// Key F4

        }
        if (ks&0x800) {
            if (KEY(K_POW_OFF))		data|=0x01;
            if (KEY(K_BRK))		data|=0x02;
        }
//        if (fp_log) fprintf(fp_log,"Read key [%02x]: strobe=%02x result=%02x\n",pKEYB->LastKey,ks,data^0xff);

    }

    if (KEY(K_BRK)) {
//        ((Csc62015*)pCPU)->opr_imem(IMEM_ISR,OPR_OR,INT_ONKEY);
        pCPU->setImemBit(IMEM_SSR,4,1);
    }
    else
        pCPU->setImemBit(IMEM_SSR,4,0);
//    else
    if(data) {
        ((Csc62015*)pCPU)->opr_imem(IMEM_ISR,OPR_OR,INT_KEY);	// set status to ISR
    }
    pCPU->imem[IMEM_KI] = data;					//set data to ki
    return data^0xff;start2khz = 0;
    start4khz = 0;

}



Ce550::Ce550(CPObject *parent):Ce500(parent)
{
    setcfgfname(QString("e550"));

    SessionHeader	= "E550PKM";
    Initial_Session_Fname ="e550.pkm";

    BackGroundFname	= ":/e500/pc-e550.png";

    Lcd_X		= 70;
    Lcd_Y		= 96;

    Lcd_Symb_X	= 70;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 76;//(int) (35 * 1.18);

}


bool Ce550::Chk_Adr(DWORD *d, DWORD data)
{
    return Ce500::Chk_Adr(d,data);
}

bool Ce550::Chk_Adr_R(DWORD *d, DWORD data)
{
    return Ce500::Chk_Adr_R(d,data);
}



