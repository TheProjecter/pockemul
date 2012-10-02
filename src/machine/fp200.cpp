#include <QtGui>
#include <QString>

#include "common.h"
#include "fp200.h"
#include "Lcdc_fp200.h"
#include "i8085.h"

#include "Inter.h"
#include "Keyb.h"
#include "cextension.h"
#include "uart.h"
#include "Connect.h"

#include "Log.h"

// TODO: MultiTouch Events

Cfp200::Cfp200(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 6144000/2);
    setcfgfname(QString("fp200"));

    SessionHeader	= "FP200PKM";
    Initial_Session_Fname ="fp200.pkm";

    BackGroundFname	= ":/fp200/fp200.png";
    LcdFname		= ":/fp200/fp200lcd.png";
    SymbFname		= "";

    memsize		= 0x10000;
    InitMemValue	= 0xFF;

    SlotList.clear();
    SlotList.append(CSlot(32 , 0x0000 ,	":/fp200/fp200rom.bin"   , ""	, ROM , "ROM"));
    SlotList.append(CSlot(8  , 0x8000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8  , 0xa000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8  , 0xc000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8  , 0xe000 ,	""                  , ""	, RAM , "RAM"));



//    KeyMap		= KeyMapfp200;
//    KeyMapLenght= KeyMapfp200Lenght;


    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(320);
    setDYmm(220);
    setDZmm(55);

    setDX(1034);
    setDY(705);

    Lcd_X		= 158;
    Lcd_Y		= 57;
    Lcd_DX		= 160;//168;//144 ;
    Lcd_DY		= 64;
    Lcd_ratio_X	= 2;// * 1.18;
    Lcd_ratio_Y	= 2;// * 1.18;

    pLCDC		= new Clcdc_fp200(this);
    pCPU		= new Ci8085(this);
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"fp200.map");
    pUART        = new Cuart(this);

    pPARConnector = new Cconnector(this,36,1,Cconnector::Centronics_36,"Parrallel Connector",false,QPoint(715,50));
    publish(pPARConnector);
    pSERConnector = new Cconnector(this,9,1,Cconnector::DIN_8,"Serial Connector",false,QPoint(0,50));
    publish(pSERConnector);

    lastKeyBufSize = 0;
    newKey = false;

//    ioFreq = 0;
    i85cpu = (Ci8085*)pCPU;
}

Cfp200::~Cfp200() {
    delete pPARConnector;
    delete pSERConnector;
    delete pUART;
}



bool Cfp200::Chk_Adr(DWORD *d, DWORD data)
{
    if ( (*d>=0x0000) && (*d<=0x7FFF) )	return(false);		// ROM area(0000-7FFF)
    if ( (*d>=0x8000) && (*d<=0x9FFF) )	{ return(true);	}
    if ( (*d>=0xA000) && (*d<=0xBFFF) )	{ return(ext_MemSlot1->ExtArray[ID_FP201]->IsChecked);	}
    if ( (*d>=0xC000) && (*d<=0xDFFF) )	{ return(ext_MemSlot2->ExtArray[ID_FP201]->IsChecked);	}
    if ( (*d>=0xE000) && (*d<=0xFFFF) )	{ return(ext_MemSlot3->ExtArray[ID_FP201]->IsChecked || ext_MemSlot3->ExtArray[ID_FP205]->IsChecked || ext_MemSlot3->ExtArray[ID_FP231CE]->IsChecked);	}

    return true;
}

bool Cfp200::Chk_Adr_R(DWORD *d, DWORD data)
{
    return true;
}

#define SOD (i85cpu->i85stat.regs.sod_state)

UINT8 Cfp200::in(UINT8 Port)
{
    UINT8 Value=0;
    quint16 tmp=0;
    Clcdc_fp200 *pLcd = (Clcdc_fp200*)pLCDC;

    if (SOD)
    {      // SOD = 1
        switch (Port)
        {
        case 0x01 : /* Read 8bits data to LCD left-half */
            Value = pLcd->Read(1);
            AddLog(LOG_CONSOLE,tr("IN [01]=[%1]=%2\n").arg(Value,2,16,QChar('0')).arg(QChar(Value).toAscii()!=0?QChar(Value):QChar(' ')));
            break;
        case 0x02 : /* Read 8bits data to LCD right-half */
            Value = pLcd->Read(2);
            AddLog(LOG_CONSOLE,tr("IN [02]=[%1]=%2\n").arg(Value,2,16,QChar('0')).arg(QChar(Value).toAscii()!=0?QChar(Value):QChar(' ')));
            break;
        case 0x08 : /* Read 6 bits data : */
            Value = (pLcd->Status << 4) | ((pLcd->Y >> 4) & 0x03);
            AddLog(LOG_CONSOLE,tr("IN [08]=[%1]\n").arg(Value,2,16,QChar('0')));
            break;
        case 0x09: /* D0-D3 for X, D4-D7 for part of Y */
            Value = (pLcd->X & 0x0f) | ((pLcd->Y & 0x0f) <<4);
            AddLog(LOG_CONSOLE,tr("IN [09]=[%1]\n").arg(Value,2,16,QChar('0')));
            break;
        case 0x20: tmp = getKey();
            Value= tmp & 0xff;
            sid = (tmp >> 8);
            if ((tmp>>8) > 0) {
                AddLog(LOG_CONSOLE,tr("SID=[%1]\n").arg(tmp>>8,2,16,QChar('0')));
            }
            break;
        default: AddLog(LOG_CONSOLE,tr("IN [%1]\n").arg(Port,2,16,QChar('0')));
            break;
        }
    }

     pCPU->imem[Port] = Value;
     return (Value);
}

UINT8 Cfp200::out(UINT8 Port, UINT8 Value)
{
    Clcdc_fp200 *pLcd = (Clcdc_fp200*)pLCDC;

    if (SOD)
    {      // SOD = 1
        switch (Port)
        {
        case 0x01 : /* Write 8bits data to LCD left-half */
            pLcd->Write(1,Value);
            AddLog(LOG_CONSOLE,tr("OUT[01]=[%1]=%2\n").arg(Value,2,16,QChar('0')).arg(QChar(Value).toAscii()!=0?QChar(Value):QChar(' ')));
            break;
        case 0x02 : /* Write 8bits data to LCD right-half */
            pLcd->Write(2,Value);
            AddLog(LOG_CONSOLE,tr("OUT[02]=[%1]=%2\n").arg(Value,2,16,QChar('0')).arg(QChar(Value).toAscii()!=0?QChar(Value):QChar(' ')));
            break;
        case 0x08 : /* write 6 bits data : */

            pLcd->Status = (Value >>4) & 0x0f;
            if (pLcd->Status==0x0b) pLcd->Y = (pLcd->Y & 0x0f) | ((Value & 0x03) << 4);
            AddLog(LOG_CONSOLE,tr("OUT[08]=[%1] Status=%2 Y=%3\n").
                   arg(Value,2,16,QChar('0')).
                   arg(pLcd->Status,2,16,QChar('0')).
                   arg(pLcd->Y,2,16,QChar('0')));

            break;
        case 0x09: /* D0-D3 for X, D4-D7 for part of Y */
            pLcd->X = Value & 0x0f;
            pLcd->Y = (pLcd->Y & 0x30) | (Value >> 4);
            AddLog(LOG_CONSOLE,tr("OUT[09]=[%1] X=%2 Y=%3\n").arg(Value,2,16,QChar('0')).
                   arg(pLcd->X,2,16,QChar('0')).
                   arg(pLcd->Y,2,16,QChar('0')));
            break;
            //    case 0x20: i85cpu->i8085_set_irq_line(I8085_RST75_LINE,0);
            //        break;
        case 0x20:break;
        case 0x21: ks = Value & 0x0f;
            break;
        default: AddLog(LOG_CONSOLE,tr("OUT[%1]=[%2]=%3\n").arg(Port,2,16,QChar('0')).arg(Value,2,16,QChar('0')).arg(QChar(Value).toAscii()!=0?QChar(Value):QChar(' ')));
            break;
        }
    }
    return 0;
}

UINT8 Cfp200::in8(UINT16 Port)
{
    return 0;
}



UINT8 Cfp200::out8(UINT16 Port, UINT8 x)
{
    return 0;
}

UINT16 Cfp200::in16(UINT16 Port)
{
    return 0;
}

UINT16 Cfp200::out16(UINT16 Port, UINT16 x)
{
    return 0;
}

bool Cfp200::init()
{
//    pCPU->logsw = true;
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();
    initExtension();
    Reset();
    Cetl = false;
    sid = 0;

    pUART->init();
    pUART->pTIMER = pTIMER;

    return true;
}

void	Cfp200::initExtension(void)
{
    AddLog(LOG_MASTER,"INIT EXT FP-200");
    // initialise ext_MemSlot1
    ext_MemSlot1 = new CExtensionArray("RAM Slot 1","Add RAM Module");
    ext_MemSlot2 = new CExtensionArray("RAM Slot 2","Add RAM Module");
    ext_MemSlot3 = new CExtensionArray("RAM/ROM Slot 3","Add RAM or ROM Module");
    ext_MemSlot1->setAvailable(ID_FP201,true); ext_MemSlot1->setChecked(ID_FP201,false);
    ext_MemSlot2->setAvailable(ID_FP201,true); ext_MemSlot2->setChecked(ID_FP201,false);
    ext_MemSlot3->setAvailable(ID_FP201,true); ext_MemSlot3->setChecked(ID_FP201,false);
    ext_MemSlot3->setAvailable(ID_FP205,true);
    ext_MemSlot3->setAvailable(ID_FP231CE,true);

    addExtMenu(ext_MemSlot1);
    addExtMenu(ext_MemSlot2);
    addExtMenu(ext_MemSlot3);
    extensionArray[0] = ext_MemSlot1;
    extensionArray[1] = ext_MemSlot2;
    extensionArray[2] = ext_MemSlot3;
}

bool Cfp200::run()
{
    if (ks==5) i85cpu->i8085_set_SID(Cetl?0:1);
    if (ks==6) i85cpu->i8085_set_SID(pKEYB->isShift?0:1);
    if (ks==7) i85cpu->i8085_set_SID(pKEYB->LastKey==0x03?0:1);        // BREAK
//    if (ks==8) i85cpu->i8085_set_SID(1);        // GRAPH
    if (ks==9) i85cpu->i8085_set_SID(pKEYB->isCtrl?0:1);

    if (pKEYB->LastKey>0) {

        i85cpu->i8085_set_irq_line(I8085_RST75_LINE,1);
    }
    else
        i85cpu->i8085_set_irq_line(I8085_RST75_LINE,0);




    CpcXXXX::run();

    //TODO Copy data to UART: Baudrate

    pUART->Set_CS(true);        // for test purpose

    pUART->run();

    pSERConnector->Set_pin(3,pUART->Get_SD());      // TxD
    pSERConnector->Set_pin(4,pUART->Get_RD());      // RxD
    pSERConnector->Set_pin(6,pUART->Get_CS());      // CTS
    pSERConnector->Set_pin(8,pUART->Get_RS());      // RTS

}

void Cfp200::Reset()
{
    CpcXXXX::Reset();

}

void Cfp200::TurnON()
{
    CpcXXXX::TurnON();
    pCPU->Reset();
}

void Cfp200::TurnOFF()
{
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = ASK;
}

bool Cfp200::SaveConfig(QXmlStreamWriter *xmlOut)
{

    return true;
}

bool Cfp200::LoadConfig(QXmlStreamReader *xmlIn)
{

    return true;
}

#define KEY(c)	( pKEYB->keyPressedList.contains(toupper(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(tolower(c)))

//#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )
quint16 Cfp200::getKey()
{

    quint16 data=0;
    UINT16 strobe = (0x01 << (ks));

//    i85cpu->i8085_set_SID(0);
    if (ks == 0x0B) strobe = 0xffff;

//    if ((pKEYB->LastKey>0))// && ks )
    {
        if (strobe & 0x01) {
            if (KEY('7'))			data|=0x10;
            if (KEY('U'))			data|=0x20;
            if (KEY('J'))			data|=0x40;
            if (KEY('M'))			data|=0x80;
        }
        if (strobe & 0x02) {
            if (KEY('8'))			data|=0x10;
            if (KEY('I'))			data|=0x20;
            if (KEY('K'))			data|=0x40;
            if (KEY('/'))			data|=0x80;
        }
        if (strobe & 0x04) {
            if (KEY('9'))			data|=0x10;
            if (KEY('O'))			data|=0x20;
            if (KEY('L'))			data|=0x40;
            if (KEY('.'))			data|=0x80;
        }
        if (strobe & 0x08) {
            if (KEY('0'))			data|=0x10;
            if (KEY('P'))			data|=0x20;
            if (KEY(','))			data|=0x40;
            if (KEY(';'))			data|=0x80;
        }
        if (strobe & 0x10) {

            if (KEY(K_SML))			data|=0x01;
            if (KEY(K_INS))			data|=0x04;
            if (KEY(K_CLR))			data|=0x04;
            if (KEY(K_F0))			data|=0x08;
            if (KEY('1'))			data|=0x10;
            if (KEY('Q'))			data|=0x20;
            if (KEY('A'))			data|=0x40;
            if (KEY('Z'))			data|=0x80;
            // K SWITCH
//            data|=0x100;
//                    i85cpu->i8085_set_SID(1);
        }
        if (strobe & 0x20) {
            if (KEY(K_RA))			data|=0x04;
            if (KEY(K_F1))			data|=0x08;
            if (KEY('2'))			data|=0x10;
            if (KEY('W'))			data|=0x20;
            if (KEY('S'))			data|=0x40;
            if (KEY('X'))			data|=0x80;
            // CETL BASIC SWITCH
            if (KEY(K_BASIC))       {
                Cetl = false;
                data|=0x100;
            }
            if (KEY(K_CETL))       {
                Cetl = true;
            }

        }
        if (strobe & 0x40) {
            if (KEY(' '))			data|=0x01;
            if (KEY('^'))			data|=0x02;
            if (KEY(K_LA))			data|=0x04;
            if (KEY(K_F2))			data|=0x08;
            if (KEY('3'))			data|=0x10;
            if (KEY('E'))			data|=0x20;
            if (KEY('D'))			data|=0x40;
            if (KEY('C'))			data|=0x80;

            if (pKEYB->isShift) data|=0x100;
//            i85cpu->i8085_set_SID(pKEYB->isShift?1:0);
        }
        if (strobe & 0x80) {
            if (KEY('@'))			data|=0x01;
            if (KEY('-'))			data|=0x02;
            if (KEY(K_DA))			data|=0x04;
            if (KEY(K_F3))			data|=0x08;
            if (KEY('4'))			data|=0x10;
            if (KEY('R'))			data|=0x20;
            if (KEY('F'))			data|=0x40;
            if (KEY('V'))			data|=0x80;
        }
        if (strobe & 0x100)  {
            if (KEY('['))			data|=0x01;
            if (KEY(':'))			data|=0x02;
            if (KEY(K_UA))			data|=0x04;
            if (KEY(K_F4))			data|=0x08;
            if (KEY('5'))			data|=0x10;
            if (KEY('T'))           data|=0x20;
            if (KEY('G'))			data|=0x40;
            if (KEY('B'))			data|=0x80;
        }
        if (strobe & 0x200) {
            if (KEY(K_RET))			data|=0x01;
//            if (KEY(K_SML))			data|=0x04;
            if (KEY('_'))			data|=0x08;
            if (KEY('6'))			data|=0x10;
            if (KEY('Y'))           data|=0x20;
            if (KEY('H'))			data|=0x40;
            if (KEY('N'))			data|=0x80;

            if (pKEYB->isCtrl) data|=0x100;
//            i85cpu->i8085_set_SID(pKEYB->isCtrl?1:0);
        }


//        if (fp_log) fprintf(fp_log,"Read key [%02x]: strobe=%02x result=%02x\n",pKEYB->LastKey,ks,data^0xff);

    }
//                data|=0x20;
    if (data>0) {
//        i85cpu->i8085_set_RST75(1);
        AddLog(LOG_KEYBOARD,tr("KEY PRESSED=%1").arg(data,2,16,QChar('0')));
    }
    return data;//^0xff;

}

//void Cfp200::keyReleaseEvent(QKeyEvent *event)
//{
////if (event->isAutoRepeat()) return;

//    if (pCPU->fp_log) fprintf(pCPU->fp_log,"\nKEY RELEASED= %c\n",event->key());
//    CpcXXXX::keyReleaseEvent(event);
//}


//void Cfp200::keyPressEvent(QKeyEvent *event)
//{
////    if (event->isAutoRepeat()) return;

//    if (pCPU->fp_log) fprintf(pCPU->fp_log,"\nKEY PRESSED= %c\n",event->key());
//    CpcXXXX::keyPressEvent(event);
//}

