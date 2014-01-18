// TODO: LCD indicators see SnapFORTH vol 2 12-14
#define USEBUS
#include <QDebug>

#include "rlh1000.h"
#include "m6502/m6502.h"
#include "Inter.h"
#include "Keyb.h"
#include "Connect.h"
#include "Log.h"
#include "Lcdc_rlh1000.h"
#include "ui/dialogdasm.h"
#include "clink.h"
#include "dialoganalog.h"



Crlh1000::Crlh1000(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 1000000);
    setcfgfname(QString("rlh1000"));

    SessionHeader	= "RLH1000PKM";
    Initial_Session_Fname ="rlh1000.pkm";

    BackGroundFname	= P_RES(":/rlh1000/rlh1000.png");
    LcdFname		= P_RES(":/rlh1000/rlh1000lcd.png");
    SymbFname		= "";

    memsize		= 0x30000;
    InitMemValue	= 0x7F;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	""                                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8 , 0x2000 ,	P_RES(":/rlh1000/rlp1004a.bin")    , ""	, ROM , "Ext ROM"));
    SlotList.append(CSlot(16, 0x4000 ,	P_RES(":/rlh1000/SnapBasic.bin")    , ""	, ROM , "ROM Capsules 1"));
    SlotList.append(CSlot(16, 0x8000 ,	""                                  , ""	, RAM , "Ext RAM"));
    SlotList.append(CSlot(16, 0xC000 ,	P_RES(":/rlh1000/HHC-rom-C000-FFFF.bin"), ""	, ROM , "ROM"));
    SlotList.append(CSlot(16, 0x10000 ,	""                                  , ""	, RAM , "I/O Hard"));
    SlotList.append(CSlot(16, 0x14000 ,	P_RES(":/rlh1000/HHCbasic.bin")    , ""	, ROM , "ROM Capsules 2"));
    SlotList.append(CSlot(16, 0x18000 ,	P_RES(":/rlh1000/SnapForth.bin")    , ""	, ROM , "ROM Capsules 3"));
    SlotList.append(CSlot(16, 0x1C000 ,	""                                  , ""	, RAM , "Ext RAM"));

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

    extrinsic = 0;

    bus = new Cbus();
}

Crlh1000::~Crlh1000() {

}


bool Crlh1000::init(void)				// initialize
{

pCPU->logsw = true;
    if (!fp_log) fp_log=fopen("rlh1000.log","wt");	// Open log file
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
    CpcXXXX::init();

    pCONNECTOR	= new Cconnector(this,44,0,Cconnector::Panasonic_44,"44 pins connector",false,
                                     QPoint(0,72),
                                 Cconnector::WEST);
    publish(pCONNECTOR);

    WatchPoint.add(&pCONNECTOR_value,64,44,this,"44 pins connector");

    pTIMER->resetTimer(1);

    latchByte = 0x00;

    timercnt1=timercnt2=timercnt3=0;
    memset(&lineFD,0xff,sizeof(lineFD));
    memset(&lineFE,0xff,sizeof(lineFE));
    memset(&lineFF,0xff,sizeof(lineFF));

    return true;
}



bool Crlh1000::run() {

    // 0x5802 : Shift 0x80   ???
    // 0x5820 : 2nd 0x80     ???
    // timercnt = timer (1/256 sec)

    //TODO: change this !!!
    if (dialogdasm)
        dialogdasm->imem=false;

#ifndef QT_NO_DEBUG
    if (pCPU->get_PC()==0xc854) m6502->set_PC(0xc856);
#endif



     CpcXXXX::run();

     if (bus->getINT()) {
         qWarning()<<"INT RECEIVED FROM BUS";
         m6502->write_signal(101,1,1);
         bus->setINT(false);
     }

    if (pKEYB->LastKey>0) { m6502->write_signal(101,1,1); }

    if (pTIMER->usElapsedId(1)>=3906) {
//        if (timercnt1!=0)
        {
            timercnt1--;
            if ((timercnt1==0)&&(timercnt2==0)&&(timercnt3==0)) {
                m6502->write_signal(101,1,1);
                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ ROM KBD TIMER\n");
            }
            if (timercnt1==0xff) {
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

    Set_Connector();
    return true;
}

#define LINEID (islineFC?"FC":islineFD?"FD":islineFE?"FE":islineFF?"FF":"??")

bool Crlh1000::Chk_Adr(UINT32 *d, UINT32 data)
{

    if(*d < 0x2000) {
        if (pCPU->fp_log) {
            sprintf(Log_String,"%s Write[%04x]=%02x ",Log_String,*d,data);
        }

        if (*d==0x4e) {
            // print char
            // send data to the bus
            if (extrinsic!=0xff) {
                bus->setDest(extrinsic);
                bus->setAddr(*d);
                bus->setData(data);
                bus->setFunc(BUS_WRITEDATA);
                manageBus();
                //        if (fp_log) fprintf(fp_log,"  AFTER DEST=%i  \n",bus->getDest());

                return true;
            }
        }
        return true; /* RAM */
    }


    if((*d>=0x2000) && (*d < 0x4000)) { // ROM
        if (fp_log) {
            fprintf(fp_log,"**ERROR** WRITE ROM [%04X]=%02X ",*d,data);
            pCPU->Regs_Info(1);
            fprintf(fp_log," %s\n",pCPU->Regs_String);
        }
        if (pCPU->fp_log) {
            sprintf(Log_String,"%s Write ROM[%04x]=%02x ",Log_String,*d,data);
        }
        return false;
    }

    if (*d==0x58FE) {
        latchByte = data;
        if ((latchByte & 0x40) == 0) {
            // HALT CPU
            pCPU->halt = true;
//            qWarning()<<"CPU HALT";
            AddLog(LOG_CONSOLE,"CPU HALT\n");
            if (fp_log) fprintf(fp_log,"\nCPU HALT\n");
            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\nCPU HALT\n");
        }
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n ROM latchByte=%02X\n",data);

        return false;
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

            return false;
        }
        else {
            // KBD I/O Mapping
            if (*d==0x58FB) { timercnt1=data; return false; }
            if (*d==0x58FC) { timercnt2=data; return false; }
            if (*d==0x58FD) { timercnt3=data; return false; }
            if (*d==0x58FF) {                 return false; }

// Write 47FD 47FE 47FF

            // 47FF : write from ext, read by nucleus

            if ( (*d>=0x47FC) && (*d<=(0x47FF+0xff))) {
                if (pCPU->fp_log) sprintf(Log_String,"%s,WRITE ROM LINE [%04X]=%02X ",Log_String,*d,data);
                bool islineFC = ((*d-0x47FC)%4==0);
                bool islineFD = ((*d-0x47FD)%4==0);
                bool islineFE = ((*d-0x47FE)%4==0);
                bool islineFF = ((*d-0x47FF)%4==0);

                if (islineFF) {
                    quint8 t = (*d-0x47FF)/4;
                    lineFF[t] = data;
                }
                if (islineFD) {
                    quint8 t = (*d-0x47FD)/4;
                    lineFD[t] = data;
                }
                if (islineFE) {
                    quint8 t = (*d-0x47FE)/4;
                    lineFE[t] = data;

                    bus->setDest(t);
                    if (fp_log) fprintf(fp_log,"BUS_SELECT DEST=%i data=%02x \n",bus->getDest(),bus->getData());
                    bus->setData(data);
                    bus->setFunc(BUS_SELECT);
                    manageBus();
                    if (bus->getFunc()==BUS_READDATA) extrinsic=t;//bus->getDest();
//                    if (fp_log) fprintf(fp_log," AFTER DEST=%i data \n",bus->getDest());

                    if (fp_log) {
                        fprintf(fp_log,"WRITE LINE%s [%2i]=%i",LINEID,t,data);
                        for (int i=0;i<=32;i++) {
                            if (i%4 == 0) fprintf(fp_log," ");
                            fprintf(fp_log,"%i",lineFE[i]);

                        }
                        pCPU->Regs_Info(1);
                        fprintf(fp_log," %s\n",pCPU->Regs_String);
                    }
                }

                return false;
            }

            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n WRITE ROM [%02X]=%02X\n",*d,data);

            if (fp_log) {
                fprintf(fp_log,"WRITE ROM [%04X]=%02X ",*d,data);
                pCPU->Regs_Info(1);
                fprintf(fp_log," %s\n",pCPU->Regs_String);
            }

            return false;
        }

        if (fp_log) {
            fprintf(fp_log,"WRITE ROM UNHANDLED [%04X]=%i",*d,data);
            for (int i=0;i<=32;i++) {
                if (i%4 == 0) fprintf(fp_log," ");
                fprintf(fp_log,"%i",lineFE[i]);

            }
            pCPU->Regs_Info(1);
            fprintf(fp_log," %s\n",pCPU->Regs_String);
        }
        return false;

    }
    if((*d>=0x8000) && (*d < 0xC000)) {
        if (fp_log) {
            fprintf(fp_log,"WRITE RAM [%04X]=%02X ",*d,data);
            pCPU->Regs_Info(1);
            fprintf(fp_log," %s\n",pCPU->Regs_String);
        }

        if (fp_log) fprintf(fp_log,"BUS_WRITEDATA DEST=%i  data=%02X\n",bus->getDest(),data);
        if (extrinsic!=0xff) {
            bus->setDest(extrinsic);
            bus->setAddr(*d);
            bus->setData(data);
            bus->setFunc(BUS_WRITEDATA);
            manageBus();
            //        if (fp_log) fprintf(fp_log,"  AFTER DEST=%i  \n",bus->getDest());

            return false;
        }

        return false; /* RAM */
    }


    return false;
}

bool Crlh1000::Chk_Adr_R(UINT32 *d, UINT32 *data)
{
    if (*d < 0x2000) {
        if (pCPU->fp_log) {
            sprintf(Log_String,"%s Read[%04x]=%02x ",Log_String,*d,mem[*d]);
        }
        return true;   // RAM
    }

    if((*d>=0x2000) && (*d < 0x4000)) { // ROM
        *data = ReadBusMem(BUS_READROM,*d);
        return false;
    }

    if((*d>=0x4000) && (*d <= 0x7FFF)) {
        if ((latchByte & 0x04)==0) {
            // I/O mapping

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
                if ( (*d>=0x5800) && (*d<=(0x5800+0xfa))) {
                    quint8 t = *d - 0x5800;
                    *data = getKey(t);
                    return false;
                }

                // EXT management
                if ( (*d>=0x47FC) && (*d<=(0x47FF+0xff))) {
                    bool islineFC = ((*d-0x47FC)%4==0);
                    bool islineFD = ((*d-0x47FD)%4==0);
                    bool islineFE = ((*d-0x47FE)%4==0);
                    bool islineFF = ((*d-0x47FF)%4==0);
                    quint8 t=0;

                    if (islineFC) {
                        // return &FF : Scan next line
                        // return &FB :

                        t =(*d-0x47FC)/4;

                        bus->setDest(t);
                        if (fp_log) fprintf(fp_log,"BUS_QUERY DEST=%i  ",bus->getDest());
                        bus->setData(0xff);
                        bus->setFunc(BUS_QUERY);
                        manageBus();
                        if (fp_log) fprintf(fp_log,"  data=%02X  \n",bus->getData());

                        *data = bus->getData();

                    }
                    if (islineFD) {
                        t = (*d-0x47FD)/4;
                        *data = lineFD[t];
                    }
                    if (islineFE) {
                        t = (*d-0x47FE)/4;
                        *data = lineFE[t];
                    }
                    if (islineFF) {
                        t = (*d-0x47FF)/4;
                        *data = 0xff;//lineFF[t];
                        bus->setDest(t);
                        if (fp_log) fprintf(fp_log,"BUS_QUERY_LINE DEST=%i  ",bus->getDest());
                        bus->setData(0xff);
                        bus->setFunc(BUS_INTREQUEST);
                        manageBus();
                        if (fp_log) fprintf(fp_log,"  data=%02X  \n",bus->getData());

                        *data = bus->getData();
                    }

                    if (fp_log) {
                        fprintf(fp_log,"READ LINE%s (%2i) data=%i ",LINEID,t,*data);
                        pCPU->Regs_Info(1);
                        fprintf(fp_log," %s\n",pCPU->Regs_String);
                    }
                    if (pCPU->fp_log) sprintf(Log_String,"%s,READ LINE%s [%i]=%04X ",Log_String,LINEID,t,*data);
                    return false;
                }
            }
            if (*d==0x58FA) { *data=0x80; return false; }
            if (*d==0x58FB) { *data=timercnt1; return false; }
            if (*d==0x58FC) { *data=timercnt2; return false; }
            if (*d==0x58FD) { *data=timercnt3; return false; }
            if (*d==0x58FF) {  *data = pKEYB->LastKey; return false; }

            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\n READ ROM UNHANDLED[%04X]\n",*d);
            if (fp_log) {
                fprintf(fp_log,"READ ROM [%04X]  ",*d);
                pCPU->Regs_Info(1);
                fprintf(fp_log," %s\n",pCPU->Regs_String);
            }

            return false;
        }
        else {
            UINT32 offset = 0;
            switch (latchByte & 0x03) {
            case 0x00 : offset = 0; break;
            case 0x01 : offset = 0x10000; break;
            case 0x02 : offset = 0x14000; break;
            case 0x03 :
                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\nEXT CAPSULE\n");
                offset = 0; break;//*data=0xff; return false; // External ROM ????
            default: break;
            }
            *d += offset;
            return true;
        }

        return true; /* ROM */
    }


    if((*d>=0x8000) && (*d < 0xC000)) {
        *data = ReadBusMem(BUS_READDATA,*d);
        return false; /* RAM */
    }

    return true;
}

UINT8 Crlh1000::ReadBusMem(BUS_FUNC f,UINT32 adr) {
    UINT8 data=0xff;

    if (fp_log) {
        fprintf(fp_log,"BUS=%02X READ RAM [%04X] ",bus->getDest(),adr);
        pCPU->Regs_Info(1);
        fprintf(fp_log," %s\n",pCPU->Regs_String);
    }
    if (fp_log) fprintf(fp_log,"BUS_READDATA DEST=%i  ",bus->getDest());
    if (extrinsic!=0xff) {
        bus->setDest(extrinsic);
        bus->setAddr(adr);
        bus->setData(0xff);
        bus->setFunc(f);
        manageBus();
        if (fp_log) fprintf(fp_log," DATA=%02X  \n",bus->getData());

        if (bus->getFunc()==BUS_READDATA) {

            data = bus->getData();
            if (fp_log) {
                fprintf(fp_log,"***READ RAM [%04X]=%02X",adr,data);
                pCPU->Regs_Info(1);
                fprintf(fp_log," %s\n",pCPU->Regs_String);
            }
        }
        else data = 0xff;
    }
    return data;
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
    // transfert busValue to Connector
    pCONNECTOR->Set_values(bus->toUInt64());
    return true;
}

bool Crlh1000::Get_Connector()
{

    bus->fromUInt64(pCONNECTOR->Get_values());

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
    quint8 data = 0;

//    if (fp_log) fprintf(fp_log,"READ KEYBOARD [%2i] ",row);
    if ((pKEYB->LastKey) )
    {
        if (row&0x01) {
            if (KEY(K_F5)) data |= 0x01;    // CLEAR
            if (KEY('1')) data |= 0x02;
            if (KEY('2')) data |= 0x04;
            if (KEY('3')) data |= 0x08;
            if (KEY('4')) data |= 0x10;
            if (KEY('5')) data |= 0x20;
            if (KEY('6')) data |= 0x40;
            if (KEY('7')) data |= 0x80;
        }
        if (row&0x02) {
            if (KEY('8')) data |= 0x01;
            if (KEY('9')) data |= 0x02;
            if (KEY('0')) data |= 0x04;
            if (KEY(K_HELP)) data |= 0x08;
            if (KEY(K_UA)) data |= 0x10;
            if (KEY(K_F6)) data |= 0x20; // IO
            if (KEY('.')) data |= 0x40;
            if (KEY(K_CTRL)) data |= 0x80; // 2nd SFT
        }

        if (row&0x04) {
            if (KEY('M')) data |= 0x01;
            if (KEY('Q')) data |= 0x02;
            if (KEY('W')) data |= 0x04;
            if (KEY('E')) data |= 0x08;
            if (KEY('R')) data |= 0x10;
            if (KEY('T')) data |= 0x20;
            if (KEY('Y')) data |= 0x40;
            if (KEY('U')) data |= 0x80;
        }

        if (row&0x08) {
            if (KEY('I')) data |= 0x01;
            if (KEY('O')) data |= 0x02;
            if (KEY('P')) data |= 0x04;
            if (KEY(K_LA)) data |= 0x08;
            if (KEY(K_STP)) data |= 0x10;
            if (KEY(K_RA)) data |= 0x20;
            if (KEY(K_F2)) data |= 0x40;
            if (KEY(K_F3)) data |= 0x80;
        }

        if (row&0x10) {
            if (KEY('N')) data |= 0x01;
            if (KEY('A')) data |= 0x02;
            if (KEY('S')) data |= 0x04;
            if (KEY('D')) data |= 0x08;
            if (KEY('F')) data |= 0x10;
            if (KEY('G')) data |= 0x20;
            if (KEY('H')) data |= 0x40;
            if (KEY('J')) data |= 0x80;
        }

        if (row&0x20) {
            if (KEY('K')) data |= 0x01;
            if (KEY('L')) data |= 0x02;
            if (KEY(K_F1)) data |= 0x04;
            if (KEY(K_SEARCH)) data |= 0x08;
            if (KEY(K_DA)) data |= 0x10;
            if (KEY(K_ROTATE)) data |= 0x20;
            if (KEY(K_INS)) data |= 0x40;
            if (KEY(K_SHT)) data |= 0x80;
        }

        if (row&0x40) {
            if (KEY('B')) data |= 0x01;
            if (KEY(K_C1)) data |= 0x02;
            if (KEY(K_C2)) data |= 0x04;
            if (KEY(',')) data |= 0x08;
            if (KEY('?')) data |= 0x10;
            if (KEY(' ')) data |= 0x20;
            if (KEY(K_C3)) data |= 0x40;
            if (KEY(K_C4)) data |= 0x80;
        }

        if (row&0x80) {
            if (KEY(K_LOCK)) data |= 0x01;
            if (KEY('Z')) data |= 0x02;
            if (KEY('X')) data |= 0x04;
            if (KEY('C')) data |= 0x08;
            if (KEY('V')) data |= 0x10;
            if (KEY(K_OF)) data |= 0x20;
            if (KEY(K_DEL)) data |= 0x40;
            if (KEY(K_RET)) data |= 0x80;
        }

    }

    if (KEY('Z')) {
        qWarning()<<"LOG";
        pCPU->logsw = true;
        pCPU->Check_Log();
        if (!fp_log) fp_log=fopen("rlh1000.log","wt");	// Open log file
    }
    return data;

}


