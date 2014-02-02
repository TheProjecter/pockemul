#include <QtGui>
#include <QTime>
#include <QFileDialog>
#include <QThread>
#include <QDebug>

#include "common.h"
#include "fluidlauncher.h"

#include "rlp4002.h"

#include "Log.h"
#include "dialoganalog.h"
#include "paperwidget.h"
#include "Keyb.h"
#include "Connect.h"
#include "bus.h"

#define DOWN    0
#define UP              1


Crlp4002::Crlp4002(CPObject *parent):CPObject(this)
{ //[constructor]
    Q_UNUSED(parent)

    setfrequency( 0);

    BackGroundFname     = P_RES(":/rlh1000/rlp4002.png");
    setcfgfname("rlp4002");

    pTIMER              = new Ctimer(this);
//    pKEYB               = new Ckeyb(this,"rlp4002.map");
    setDXmm(113);
    setDYmm(95);
    setDZmm(51);
 // Ratio = 3,57
    setDX(440);//Pc_DX  = 75;
    setDY(340);//Pc_DY  = 20;

    rotate = false;
    slotChanged = false;
    INTrequest = false;

    memsize             = 0x2000;
    InitMemValue        = 0x7f;
    SlotList.clear();
    // This ROM file stop at 0x1000. I think data aremissing at 0x1000.
    SlotList.append(CSlot(8 , 0x0000 , P_RES(":/rlh1000/Telecomputing_1.bin")    , "" , CSlot::ROM , "ROM"));

}

Crlp4002::~Crlp4002() {
    delete pCONNECTOR;
}

bool Crlp4002::run(void)
{

    static quint64 _state=0;

    Cbus bus;

    bus.fromUInt64(pCONNECTOR->Get_values());

    if (bus.getFunc()==BUS_SLEEP) return true;

    if (bus.getDest()!=0) return true;
    bus.setDest(0);

//    qWarning()<<"MODEM:"<<bus.toLog();
    if (bus.getFunc()==BUS_QUERY) {
        bus.setData(0x00);
        bus.setFunc(BUS_READDATA);
        pCONNECTOR->Set_values(bus.toUInt64());
        if (pPC->pTIMER->pPC->fp_log) fprintf(pPC->pTIMER->pPC->fp_log,"RL-P1004A BUS_QUERY\n");
        return true;
    }

    if (bus.getFunc()==BUS_SELECT) {
//        qWarning()<<"4002 BUS SELECT:"<<bus.getData();

        switch (bus.getData()) {
        case 1: Power = true; break;
        default: Power = false; break;
        }
        if (Power)
        {
            bus.setFunc(BUS_READDATA);
            bus.setData(0x01);
            pCONNECTOR->Set_values(bus.toUInt64());
        }
        return true;
    }

    switch (bus.getFunc()) {
    case BUS_LINE3: // Print buffer
        switch(bus.getData()) {
        case 84: // Modem CONNECT
            qWarning()<<"BUS_TOUCH:"<<bus.getData();
            INTrequest = true;
            break;

        case 220: // Modem OUT ON
            qWarning()<<"BUS_TOUCH:"<<bus.getData();
            INTrequest = false;
            break;
        case 252: // Print
            qWarning()<<"BUS_TOUCH:"<<bus.getData();
            INTrequest = true;
            break;
        default: qWarning()<<"BUS_TOUCH:"<<bus.getData();
            break;
        }
        bus.setFunc(BUS_ACK);
        break;
    case BUS_INTREQUEST:
        if (INTrequest) {
            qWarning()<<"INTREQUEST:true";
            bus.setINT(true);
            bus.setData(0x00);
            INTrequest = false;
        }
        else {
//            qWarning()<<"INTREQUEST:false";
            bus.setData(0xff);
        }
        bus.setFunc(BUS_READDATA);
        pCONNECTOR->Set_values(bus.toUInt64());
        return true;
        break;
    case BUS_LINE0:
        qWarning()<<"Receive data LINE 0:"<<bus.getData();
        INTrequest = true;
        bus.setFunc(BUS_ACK);
        break;
    case BUS_LINE1:
        qWarning()<<"Receive data LINE 1:"<<bus.getData();
        INTrequest = true;
        bus.setFunc(BUS_ACK);
        break;
    default: break;
    }


    if (!Power) return true;

    quint32 adr = bus.getAddr();

    switch (bus.getFunc()) {
    case BUS_SLEEP: break;
    case BUS_WRITEDATA:
//        switch (adr) {
//        case 0x3020: // flip flop K7 output
//            tapeOutput = !tapeOutput;
////            qWarning()<<pTIMER->state<<" - "<<tapeOutput;
//            bus.setData(0x00);
//            bus.setFunc(BUS_READDATA);
//            break;
//        }
        break;
    case BUS_READDATA:
        if ( (adr>=0x2000) && (adr<0x3000) ) bus.setData(mem[adr-0x2000]);
//        else if (adr == 0x3060){
//            bus.setData(tapeInput? 0x80 : 0x00);
//        }
        else bus.setData(0x7f);
        break;
    default: break;

    }

    pCONNECTOR->Set_values(bus.toUInt64());
    return true;

}


bool Crlp4002::init(void)
{
    CPObject::init();

    setfrequency( 0);

    pCONNECTOR = new Cconnector(this,44,0,
                                    Cconnector::Panasonic_44,
                                    "44 pins conector",
                                    true,
                                    QPoint(30,72),
                                    Cconnector::WEST);
    publish(pCONNECTOR);
    WatchPoint.add(&pCONNECTOR_value,64,44,this,"Printer connector");
    AddLog(LOG_PRINTER,tr("PRT initializing..."));

    if(pKEYB)   pKEYB->init();
    if(pTIMER)  pTIMER->init();

    return true;
}



/*****************************************************/
/* Exit PRINTER                                                                          */
/*****************************************************/
bool Crlp4002::exit(void)
{
    CPObject::exit();
    return true;
}

bool Crlp4002::Get_Connector(void) {
    return true;
}

bool Crlp4002::Set_Connector(void) {
    return true;
}


void Crlp4002::paintEvent(QPaintEvent *event)
{
    CPObject::paintEvent(event);
}

void Crlp4002::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu *menu= new QMenu(this);

    BuildContextMenu(menu);


    menu->addAction(tr("Dump Memory"),this,SLOT(Dump()));
    menu->addSeparator();

    menu->addAction(tr("Rotate 180"),this,SLOT(Rotate()));
//    menu.addAction(tr("Hide console"),this,SLOT(HideConsole()));

    menu->popup(event->globalPos () );
    event->accept();
}

void Crlp4002::Rotate()
{
    rotate = ! rotate;

    InitDisplay();

}

extern int ask(QWidget *parent,QString msg,int nbButton);
#define KEY(c)	( pKEYB->keyPressedList.contains(TOUPPER(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(TOLOWER(c)))
void Crlp4002::ComputeKey()
{
    if (KEY(0x240)) {
        pKEYB->keyPressedList.removeAll(0x240);
        int _response = 0;
        BYTE* capsule = &mem[0];
        if (!SlotList[0].isEmpty() || (capsule[0]!=0x7f)) {
            _response=ask(this,
                          "The "+SlotList[0].getLabel()+ " capsule is already plugged in this slot.\nDo you want to unplug it ?",
                          2);
        }

        if (_response == 1) {
            SlotList[0].setEmpty(true);

            memset((void *)capsule ,0x7f,0x4000);
            SlotList[0].setLabel(QString("ROM bank 1"));

            slotChanged = true;
        }
        if (_response==2) return;

        FluidLauncher *launcher = new FluidLauncher(mainwindow,
                                                    QStringList()<<P_RES(":/pockemul/configExt.xml"),
                                                    FluidLauncher::PictureFlowType,
                                                    "Panasonic_Capsule");
        connect(launcher,SIGNAL(Launched(QString,CPObject *)),this,SLOT(addModule(QString,CPObject *)));
        launcher->show();
    }
}

void Crlp4002::addModule(QString item,CPObject *pPC)
{
    Q_UNUSED(pPC)

    qWarning()<<"Add Module:"<< item;

    int _res = 0;
    QString moduleName;
    if (item=="TELECOMP_I") moduleName = P_RES(":/rlh1000/Telecomputing_1.bin");
    if (item=="PANACAPSFILE") {
        moduleName = QFileDialog::getOpenFileName(
                    mainwindow,
                    tr("Choose a Capsule file"),
                    ".",
                    tr("Module File (*.bin)"));
//        customModule = CSlot::CUSTOM_ROM;
    }

    if (moduleName.isEmpty()) return;

    bool result = true; // check this is a true capsule

    qWarning()<<"loaded:"<<_res;
    if (result) {
        SlotList[0].setEmpty(false);
        SlotList[0].setResID(moduleName);
        Mem_Load(0);
        // Analyse capsule
        // 0x01 = 'C'
        // 0x01 - 0x28 : Copyright
        // 0x2C : title lenght
        // 0x2D - .. : title

        BYTE* capsule = &mem[0];
        if (capsule[1]=='C') {
            QString copyright = QString::fromLocal8Bit(QByteArray((const char*)&capsule[1],0x26));
            QString title  = QString::fromLocal8Bit(QByteArray((const char*)&capsule[0x2d],capsule[0x2c]));
            qWarning()<<"title:"<<title;
            SlotList[0].setLabel(title);
        }

        slotChanged = true;
    }
}


bool Crlp4002::SaveSession_File(QXmlStreamWriter *xmlOut)
{
    xmlOut->writeStartElement("session");
        xmlOut->writeAttribute("version", "2.0");
        xmlOut->writeAttribute("rotate",QString("%1").arg(rotate));
        xmlOut->writeStartElement("memory");
            for (int s=0; s<SlotList.size(); s++)                               // Save Memory
            {
                switch (SlotList[s].getType()) {
                case CSlot::RAM:
                case CSlot::CUSTOM_ROM: Mem_Save(xmlOut,s); break;
                default: break;
                }
            }
        xmlOut->writeEndElement();  // memory
    xmlOut->writeEndElement();  // session
    return true;
}

bool Crlp4002::LoadSession_File(QXmlStreamReader *xmlIn)
{
    if (xmlIn->name()=="session") {
        bool rot = xmlIn->attributes().value("rotate").toString().toInt(0,16);
        if (rotate != rot) Rotate();
        if (xmlIn->readNextStartElement() && xmlIn->name() == "memory" ) {
            AddLog(LOG_MASTER,"Load Memory");
            for (int s=0; s<SlotList.size(); s++)                               // Save Memory
            {
                switch (SlotList[s].getType()) {
                case CSlot::RAM:
                case CSlot::CUSTOM_ROM:
                    AddLog(LOG_MASTER,"    Load Slot"+SlotList[s].getLabel());
                    Mem_Load(xmlIn,s); break;
                default: break;
                }
            }
        }
    }
    return true;
}

bool Crlp4002::InitDisplay(void)
{

//    CPObject::InitDisplay();
    slotChanged = true;

    delete BackgroundImageBackup;
    BackgroundImageBackup = CreateImage(QSize(getDX(), getDY()),BackGroundFname,false,false,rotate?180:0);
    delete BackgroundImage;
    BackgroundImage = new QImage(*BackgroundImageBackup);
    delete FinalImage;
    FinalImage = new QImage(*BackgroundImageBackup);

    pCONNECTOR->setSnap(rotate?QPoint(406,72):QPoint(34,72));

    pCONNECTOR->setDir(rotate?Cconnector::EAST:Cconnector::WEST);
    mask = QPixmap::fromImage(*BackgroundImageBackup).scaled(getDX()*mainwindow->zoom/100,getDY()*mainwindow->zoom/100);
    setMask(mask.mask());

    return true;
}
