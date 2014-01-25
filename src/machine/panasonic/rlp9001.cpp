#include <QtGui>


#include "common.h"
#include "Log.h"
#include "pcxxxx.h"
#include "Connect.h"

#include "rlp9001.h"
//#include "rlh1000.h"
#include "bus.h"

Crlp9001::Crlp9001(CPObject *parent ,Models mod)   : CPObject(this)
{                                                       //[constructor]


    setfrequency( 0);
    BackGroundFname     = P_RES(":/rlh1000/rlp9002.png");
    model = mod;
    setDXmm(113);
    setDYmm(95);
    setDZmm(51);
 // Ratio = 3,57
    setDX(403);//Pc_DX  = 75;
    setDY(340);//Pc_DY  = 20;

    rotate = false;
    romSwitch = false;
    SlotList.clear();
    InitMemValue = 0xff;
    bank = 0;

    switch(model) {
    case RLP9001:
        memsize      = 0x1000;
        SlotList.append(CSlot(4 , 0x0000 , "" , ""        , RAM , "RAM 4Ko"));
        break;
    case RLP9002:
        memsize      = 0x2000;
        SlotList.append(CSlot(8 , 0x0000 , "" , ""        , RAM , "RAM 8Ko"));
        break;
    case RLP9003:
        memsize      = 0x4000;
        SlotList.append(CSlot(16 , 0x0000 , "" , ""        , RAM , "RAM 16Ko"));
        break;
    case RLP9003R:
        memsize      = 0x4000;
        SlotList.append(CSlot(16 , 0x0000 , "" , ""        , RAM , "RAM 16Ko"));
        break;
    case RLP9004:
        memsize      = 0x8000;
        SlotList.append(CSlot(16 , 0x0000 , "" , ""        , RAM , "RAM 16Ko bank 1"));
        SlotList.append(CSlot(16 , 0x4000 , "" , ""        , RAM , "RAM 16Ko bank 2"));
        break;
    case RLP9005:
        memsize      = 0x20000;
        SlotList.append(CSlot(16 , 0x0000 , "" , ""        , RAM , "RAM 16Ko bank 1"));
        SlotList.append(CSlot(16 , 0x4000 , "" , ""        , RAM , "RAM 16Ko bank 2"));
        SlotList.append(CSlot(16 , 0x8000 , "" , ""        , RAM , "RAM 16Ko bank 3"));
        SlotList.append(CSlot(16 , 0xC000 , "" , ""        , RAM , "RAM 16Ko bank 4"));
        SlotList.append(CSlot(16 , 0x10000 , "" , ""        , RAM , "RAM 16Ko bank 5"));
        SlotList.append(CSlot(16 , 0x14000 , "" , ""        , RAM , "RAM 16Ko bank 6"));
        SlotList.append(CSlot(16 , 0x18000 , "" , ""        , RAM , "RAM 16Ko bank 7"));
        SlotList.append(CSlot(16 , 0x1C000 , "" , ""        , RAM , "RAM 16Ko bank 8"));
        break;
    }

}

Crlp9001::~Crlp9001(){
    delete(pCONNECTOR);
}



bool Crlp9001::run(void)
{

    Cbus bus;

    bus.fromUInt64(pCONNECTOR->Get_values());
    if (bus.getFunc()==BUS_SLEEP) return true;

    if ( (bus.getDest()!=0) && (bus.getDest()!=30)) return true;

//    bus.setDest(0);

    if (bus.getFunc()==BUS_QUERY) {
        bus.setData(0xFB);
        if (romSwitch) bus.setData(0xf7);
        bus.setFunc(BUS_READDATA);
        pCONNECTOR->Set_values(bus.toUInt64());
        return true;
    }

    if (bus.getFunc()==BUS_SELECT) {
        Power = false;
        bank=0;
//        qWarning()<<"9001 BUS SELECT:"<<bus.getData();
        if (romSwitch) {
            Power = false;
            if ( bus.getData()==128 ) {
                Power = true;
                bank = 0;
            }
        }
        else {
            switch (bus.getData()) {
            case 1: Power = true; bank = 0; break;
            case 2: if ((model == RLP9004)||(model == RLP9005)) { Power = true; bank = 1; } break;
            case 4: if (model == RLP9005) { Power = true; bank = 2; } break;
            case 8: if (model == RLP9005) { Power = true; bank = 3; } break;
            case 16: if (model == RLP9005) { Power = true; bank = 4; } break;
            case 32: if (model == RLP9005) { Power = true; bank = 5; } break;
            case 64: if (model == RLP9005) { Power = true; bank = 6; } break;
            case 128: if (model == RLP9005) { Power = true; bank = 7; } break;
            default: Power = false; break;
            }
        }

        if (Power)
        {
            bus.setFunc(BUS_READDATA);
            pCONNECTOR->Set_values(bus.toUInt64());
        }
        return true;
    }

    if (!Power) return true;



    quint32 adr = bus.getAddr();

    switch (bus.getFunc()) {
    case BUS_SLEEP: break;
    case BUS_WRITEDATA:

        switch (model) {
        case RLP9001: if((adr>=0x8000) && (adr < 0x9000)) mem[adr-0x8000] = bus.getData(); break;
        case RLP9002: if((adr>=0x8000) && (adr < 0xa000)) mem[adr-0x8000] = bus.getData(); break;
        case RLP9003: if((adr>=0x8000) && (adr < 0xc000)) mem[adr-0x8000] = bus.getData(); break;
        case RLP9003R:if((adr>=0x8000) && (adr < 0xc000)) mem[adr-0x8000] = bus.getData(); break;
        case RLP9004:
        case RLP9005: if((adr>=0x8000) && (adr < 0xc000)) mem[(adr-0x8000)+bank*0x4000] = bus.getData(); break;
        }

        break;
    case BUS_READDATA:

//        bus.setData(0x7f);
        switch (model) {
        case RLP9001: if((adr>=0x8000) && (adr < 0x9000)) bus.setData(mem[adr-0x8000]); break;
        case RLP9002: if((adr>=0x8000) && (adr < 0xa000)) bus.setData(mem[adr-0x8000]); break;
        case RLP9003: if((adr>=0x8000) && (adr < 0xc000)) bus.setData(mem[adr-0x8000]); break;
        case RLP9003R:
            if (romSwitch && (adr>=0x4000) && (adr < 0x8000)) {
//                qWarning()<<"ROM SIMUL:"<<adr<<"="<<mem[adr-0x4000+0x14];
                bus.setFunc(BUS_ACK);
                bus.setData(mem[adr-0x4000+0x14]);
            }
            else if((adr>=0x8000) && (adr < 0xc000)) bus.setData(mem[adr-0x8000]);
            break;
        case RLP9004:
        case RLP9005: if((adr>=0x8000) && (adr < 0xc000)) bus.setData(mem[adr-0x8000+bank*0x4000]); break;
        }

        break;
    case BUS_INTREQUEST:
        bus.setData(0xff);
        bus.setFunc(BUS_READDATA);
        break;
    }

    pCONNECTOR->Set_values(bus.toUInt64());
    return true;
}




/*****************************************************************************/
/* Initialize Potar                                                                                                                      */
/*****************************************************************************/
bool Crlp9001::init(void)
{
    AddLog(LOG_MASTER,"RL-P9001 initializing...");

    CPObject::init();

    pCONNECTOR = new Cconnector(this,44,0,
                                    Cconnector::Panasonic_44,
                                    "44 pins conector",
                                    true,
                                    QPoint(30,72),
                                    Cconnector::WEST);
    publish(pCONNECTOR);

    Power = false;

    AddLog(LOG_MASTER,"done.\n");

    return true;
}

/*****************************************************************************/
/* Exit Potar                                                                                                                                    */
/*****************************************************************************/
bool Crlp9001::exit(void)
{
    return true;
}

bool Crlp9001::SaveSession_File(QXmlStreamWriter *xmlOut)
{
    xmlOut->writeStartElement("session");
        xmlOut->writeAttribute("version", "2.0");
        xmlOut->writeAttribute("rotate",QString("%1").arg(rotate));
        xmlOut->writeStartElement("memory");
            for (int s=0; s<SlotList.size(); s++)                               // Save Memory
            {
                if (SlotList[s].getType() == RAM)       Mem_Save(xmlOut,s);
            }
        xmlOut->writeEndElement();  // memory
    xmlOut->writeEndElement();  // session
    return true;
}

bool Crlp9001::LoadSession_File(QXmlStreamReader *xmlIn)
{
    if (xmlIn->name()=="session") {
        bool rot = xmlIn->attributes().value("rotate").toString().toInt(0,16);
        if (rotate != rot) Rotate();
        if (xmlIn->readNextStartElement() && xmlIn->name() == "memory" ) {
            AddLog(LOG_MASTER,"Load Memory");
            for (int s=0; s<SlotList.size(); s++)                               // Save Memory
            {
                if (SlotList[s].getType() == RAM) {
                    AddLog(LOG_MASTER,"    Load Slot"+SlotList[s].getLabel());
                    Mem_Load(xmlIn,s);
                }
            }
        }
    }
    return true;
}


void Crlp9001::paintEvent(QPaintEvent *event)
{
    CPObject::paintEvent(event);
}

void Crlp9001::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu *menu = new QMenu(this);
    BuildContextMenu(menu);

    menu->addAction(tr("Dump Memory"),this,SLOT(Dump()));
    menu->addSeparator();

    menu->addAction(tr("Rotate 180"),this,SLOT(Rotate()));

    if (model==RLP9003R)
        menu->addAction(tr("ROM Switch"),this,SLOT(ROMSwitch()));

    menu->popup(event->globalPos () );
    event->accept();
}

void Crlp9001::Rotate()
{
    rotate = ! rotate;

    delete BackgroundImageBackup;
    BackgroundImageBackup = CreateImage(QSize(getDX(), getDY()),BackGroundFname,false,false,rotate?180:0);
        delete BackgroundImage;
        BackgroundImage = new QImage(*BackgroundImageBackup);
        delete FinalImage;
        FinalImage = new QImage(*BackgroundImageBackup);

        pCONNECTOR->setSnap(rotate?QPoint(372,72):QPoint(30,72));

        pCONNECTOR->setDir(rotate?Cconnector::EAST:Cconnector::WEST);
        mask = QPixmap::fromImage(*BackgroundImageBackup).scaled(getDX()*mainwindow->zoom/100,getDY()*mainwindow->zoom/100);
        setMask(mask.mask());

        // adapt SNAP connector
}

void Crlp9001::ROMSwitch()
{
   romSwitch = !romSwitch;
   qWarning()<<"romSwitch:"<<romSwitch;
}
