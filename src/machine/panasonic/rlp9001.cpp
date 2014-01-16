#include <QtGui>


#include "common.h"
#include "Log.h"
#include "pcxxxx.h"
#include "Connect.h"

#include "rlp9001.h"
//#include "rlh1000.h"
#include "bus.h"

Crlp9001::Crlp9001(CPObject *parent )   : CPObject(this)
{                                                       //[constructor]


    setfrequency( 0);
    BackGroundFname     = P_RES(":/rlh1000/rlp9002.png");

    setDXmm(113);
    setDYmm(95);
    setDZmm(51);
 // Ratio = 3,57
    setDX(403);//Pc_DX  = 75;
    setDY(340);//Pc_DY  = 20;

    rotate = false;

    memsize                     = 0x4000;
    InitMemValue        = 0xff;

    SlotList.clear();
    SlotList.append(CSlot(16 , 0x0000 , ""                                  , ""        , RAM , "RAM 16Ko"));

}

Crlp9001::~Crlp9001(){
    delete(pMAINCONNECTOR);
}


bool Crlp9001::run(void)
{
    Cbus bus;

    bus.fromUInt64(pMAINCONNECTOR->Get_values());

    if (bus.getDest()!=0) return true;

    bus.setDest(0);
    if (bus.getFunc()==BUS_QUERY) {
        bus.setData(0xFB);
        bus.setFunc(BUS_READDATA);
        pMAINCONNECTOR->Set_values(bus.toUInt64());
        return true;
    }

    if (bus.getFunc()==BUS_SELECT) {
        if (bus.getData()==1){
            Power=true;
        }
        if (bus.getData()==0) {
            Power = false;
        }
        bus.setFunc(BUS_READDATA);
        pMAINCONNECTOR->Set_values(bus.toUInt64());
        return true;
    }

    if (!Power) return true;

    quint32 adr = bus.getAddr();

    switch (bus.getFunc()) {
    case BUS_SLEEP: break;
    case BUS_WRITEDATA: if((adr>=0x8000) && (adr < 0xC000)) mem[adr-0x8000] = bus.getData();
        break;
    case BUS_READDATA: if((adr>=0x8000) && (adr < 0xC000)) bus.setData(mem[adr-0x8000]);
        break;
    }

    pMAINCONNECTOR->Set_values(bus.toUInt64());
    return true;
}




/*****************************************************************************/
/* Initialize Potar                                                                                                                      */
/*****************************************************************************/
bool Crlp9001::init(void)
{
    AddLog(LOG_MASTER,"RL-P9001 initializing...");

    CPObject::init();

    pMAINCONNECTOR = new Cconnector(this,44,0,
                                    Cconnector::Panasonic_44,
                                    "44 pins conector",
                                    true,
                                    QPoint(30,72),
                                    Cconnector::WEST);
    publish(pMAINCONNECTOR);

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

    //menu->exec(event->globalPos () );
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

        pMAINCONNECTOR->setSnap(rotate?QPoint(372,72):QPoint(30,72));

        pMAINCONNECTOR->setDir(rotate?Cconnector::EAST:Cconnector::WEST);
        mask = QPixmap::fromImage(*BackgroundImageBackup).scaled(getDX()*mainwindow->zoom/100,getDY()*mainwindow->zoom/100);
        setMask(mask.mask());

        // adapt SNAP connector
}

