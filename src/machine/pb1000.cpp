

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

#define PD_RES 0x10;	// 1=reset, 0=normal_operation
#define PD_PWR 0x08;	// power control: 1=power_off, 0=power_on
#define PD_STR 0x04;	// transfer direction strobe: 1=write, 0=read
#define PD_ACK 0x01;	// transfer direction acknowledge

Cpb1000::Cpb1000(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 910000/1);
    setcfgfname(QString("pb1000"));

    SessionHeader	= "PB1000PKM";
    SessionHeaderLen= 9;
    Initial_Session_Fname ="pb1000.pkm";

    BackGroundFname	= ":/pb1000/pb-1000.png";
    LcdFname		= ":/pb1000/pb-1000lcd.png";
    SymbFname		= "";

    memsize         = 0x20000;
    InitMemValue	= 0xff;


    SlotList.clear();
    SlotList.append(CSlot(6 , 0x0000 ,	":/pb1000/rom0.bin" , ""	, ROM , "CPU ROM"));
    SlotList.append(CSlot(8 , 0x6000 ,	""					, ""	, RAM , "RAM0"));
    SlotList.append(CSlot(32, 0x8000 ,	":/pb1000/rom1.bin"	, ""	, ROM , "ROM"));
    SlotList.append(CSlot(32, 0x18000 ,	""					, ""	, RAM , "RAM1"));

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(187);
    setDYmm(177);
    setDZmm(24);

    setDX(668);//715);
    setDY(633);//465);

    Lcd_X		= 90;
    Lcd_Y		= 130;
    Lcd_DX		= 192;//168;//144 ;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 2;// * 1.18;
    Lcd_ratio_Y	= 2;// * 1.18;

    PowerSwitch = 0;

    pLCDC		= new Clcdc_pb1000(this);
    pCPU		= new CHD61700(this);
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"pb1000.map");
    pHD44352    = new CHD44352(this);

    m_kb_matrix = 0;
//    shift=fct = false;

    closed = false;
    flipping = false;
    m_angle = 0;

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

    if (off && pKEYB->LastKey == K_POW_ON)
    {
        TurnON();
        pKEYB->LastKey = 0;
    }

    CpcXXXX::run();

    if (pKEYB->LastKey) {
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"NEW KEY\n");
        ((CHD61700*)pCPU)->execute_set_input(HD61700_KEY_INT,1);
    }

}

bool Cpb1000::Chk_Adr(DWORD *d, DWORD data)
{
    if ( (*d>=0x00C00) && (*d<=0x00C0F) )	{
//        pLCDC->Refresh = true;
//        if (pCPU->fp_log) fprintf(pCPU->fp_log,"ECRITURE IO [%04X] = %02x\n",*d,data);
        AddLog(LOG_TEMP,tr("Write Port [%1] = %2").arg(*d&7).arg(data,2,16,QChar('0')));
        return(true);		// RAM area()
    }

    if ( (*d>=0x06100) && (*d<=0x061FF) )	{
//        pLCDC->Refresh = true;
//        if (pCPU->fp_log) fprintf(pCPU->fp_log,"ECRITURE [%04X] = %02x\n",*d,data);
        return(true);		// RAM area()
    }
    if ( (*d>=0x06000) && (*d<=0x07FFF) ) {
//        if (pCPU->fp_log) fprintf(pCPU->fp_log,"ECRITURE [%04X] = %02x\n",*d,data);
        return(true);		// RAM area()
    }
    if ( (*d>=0x18000) && (*d<=0x1FFFF) ) {
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"ECRITURE BANK 1 [%04X] = %02x\n",*d,data);
        return(true);		// RAM area()
    }
//    if (pCPU->fp_log) fprintf(pCPU->fp_log,"ECRITURE REJETEE [%04X] = %02x\n",*d,data);
    return false;
}

WORD Cpb1000::Get_16rPC(DWORD adr)
{
    DWORD	a;
    a=adr+1;
    return((mem[adr]<<8)+mem[a]);
}

bool Cpb1000::Chk_Adr_R(DWORD *d, DWORD data)
{
    if ( (*d>=0x00C00) && (*d<=0x00C0F) )	{
//        pLCDC->Refresh = true;
        mem[*d] = 0xff;
        AddLog(LOG_TEMP,tr("Read Port:%1").arg(*d&7));
//        if (pCPU->fp_log) fprintf(pCPU->fp_log,"LECTURE IO [%04X]\n",*d);
        return(true);		// RAM area()
    }
    if ( (*d>=0x18000) && (*d<=0x1FFFF) ) {
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"LECTURE BANK 1 [%04X]\n",*d);
        return(true);		// RAM area()
    }
//    if (pCPU->fp_log) fprintf(pCPU->fp_log,"LECTURE [%04X]\n",*d);
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
}

void Cpb1000::TurnON(void){
    CpcXXXX::TurnON();
    Reset();

}


void Cpb1000::Reset()
{

    CpcXXXX::Reset();

}

bool Cpb1000::LoadConfig(QXmlStreamReader *xmlIn)
{

    return true;
}

bool Cpb1000::SaveConfig(QXmlStreamWriter *xmlOut)
{

    return true;
}

void Cpb1000::paintEvent(QPaintEvent *event)
{
    if (flipping) {
        QPainter painter;

        UpdateFinalImage();

        painter.begin(this);

        if (FinalImage)
        {
            painter.translate(this->width()/2,355);
//            AddLog(LOG_MASTER,tr("angle%1").arg(m_angle));
            painter.drawImage(QPoint(-this->width()/2,-355),
                              FinalImage->scaled(this->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation),
                              QRect(0,0,this->width(),355));
            QTransform matrix;
            matrix.rotate(m_angle, Qt::XAxis);
            painter.setTransform(matrix,true);
//            painter.translate(0,355);
            painter.drawImage(QPoint(-this->width()/2,0),
                              FinalImage->scaled(this->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation),
                              QRect(0,355,this->width(),this->height()-355));

        }
        painter.end();
    }
    else {
        CPObject::paintEvent(event);
    }
}

#define toupper( a )	(  ((a >= 'a' && a <= 'z') ? a-('a'-'A') : a ) )
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

void Cpb1000::setKey(UINT8 data) {
    AddLog(LOG_KEYBOARD,tr("set matrix to %1").arg(data,2,16,QChar('0')));
    m_kb_matrix = data;
}

//void Cpb1000::keyPressEvent(QKeyEvent *event) {
//    switch (event->modifiers()) {
//        case Qt::ShiftModifier : shift = true; event->accept();qWarning("SHIFT");break;
//        case Qt::AltModifier:   fct = true; event->accept();qWarning("FCT");break;
////        case Qt::ControlModifier: ctrl = true; break;
//    }
//    event->ignore();
//}
//void Cpb1000::keyReleaseEvent(QKeyEvent *event)
//{
//    shift=fct = false;
//    event->ignore();
//}
UINT8 Cpb1000::readPort()
{
//    AddLog(LOG_TEMP,"Read Port");
    return 0x00;
}

void Cpb1000::writePort(UINT8 data)
{
    AddLog(LOG_TEMP,tr("Write Port:%1").arg(data,2,16,QChar('0')));
}

void Cpb1000::lcdControlWrite(UINT8 data) {
    pLCDC->redraw = true;
    pHD44352->control_write(data);
}
void Cpb1000::lcdDataWrite(UINT8 data) {
    pLCDC->redraw = true;
    pHD44352->data_write(data);
}
UINT8 Cpb1000::lcdDataRead() {
    return pHD44352->data_read();
}


void Cpb1000::TurnCLOSE(void) {
    // Animate close
    closed = !closed;

    QPropertyAnimation *animation = new QPropertyAnimation(this, "angle");
     animation->setDuration(3000);
     if (closed) {
         animation->setStartValue(0);
         animation->setEndValue(180);
     }
     else {
         animation->setStartValue(180);
         animation->setEndValue(0);

     }
     connect(animation,SIGNAL(valueChanged(QVariant)),this,SLOT(update()));
     animation->start();
    flipping = true;

    // creation de deux widget haut et bas
    // animation recul du tout
    // animation widget du bas
//    QTransform matrix;
//        matrix.rotate(45, Qt::YAxis);
//        this->setTransform(matrix);
    AddLog(LOG_MASTER,"CLOSE");
    // Animate Exit
}

void Cpb1000::setAngle(int value) {
    this->m_angle = value;
}

