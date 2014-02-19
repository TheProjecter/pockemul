
#include <QPainter>
#include <QResource>
#include <QMouseEvent>

#include "common.h"
#include "ce153.h"
#include "lh5810.h"
#include "buspc1500.h"
#include "Inter.h"
#include "Connect.h"
#include "Keyb.h"

Cce153::Cce153(CPObject *parent):CPObject(this)
{
    Q_UNUSED(parent)

    //[constructor]
    BackGroundFname	= P_RES(":/pc1500/ce153.png");
    setcfgfname(QString("ce153"));

    setDXmm(240);//Pc_DX_mm = 240;
    setDYmm(174);//Pc_DY_mm = 174;
    setDZmm(13);//Pc_DZ_mm = 13;

    setDX(831);//Pc_DX	= 960;
    setDY(602);//Pc_DY	= 320;


    pTIMER		= new Ctimer(this);

    pLH5810   = new CLH5810(this);
//    pKEYB		= new Ckeyb(this,"ce150.map");

    bus = new CbusPc1500();

    keyPressed = 0xff;

}

Cce153::~Cce153() {
    delete pCONNECTOR;
    delete bus;
    delete pLH5810;

}

bool Cce153::init(void)
{
    CPObject::init();

    setfrequency( 0);
    pCONNECTOR	= new Cconnector(this,60,0,Cconnector::Sharp_60,"Connector 60 pins",true,QPoint(388,72));	publish(pCONNECTOR);

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();
    pLH5810->init();

    return(1);

}

#define PA		(pLH5810->lh5810.r_opa)
#define PB		(pLH5810->lh5810.r_opb)
#define PC		(pLH5810->lh5810.r_opc)

bool Cce153::run(void)
{

    bus->fromUInt64(pCONNECTOR->Get_values());

    quint16 addr = bus->getAddr();



    if (bus->isEnable() &&
        bus->isME1() &&
        (addr >= 0x8000) &&
        (addr <= 0x800f) &&
        (bus->isWrite()) )
    {
//        qWarning()<<"Write "<<QString("%1").arg(addr,4,16,QChar('0'))<<"="<<QString("%1").arg(bus->getData(),2,16,QChar('0'));
        lh5810_write();
    }



    ////////////////////////////////////////////////////////////////////
    //	RMT ON/OFF
    ////////////////////////////////////////////////////////////////////
    // Service manual PA 1234. Take a look
//    if (pLH5810->lh5810.r_opa & 0x02)	((Cpc15XX *)pPC->pTIMER->pPC)->pce152->paused = false;	// RMT 0 ON
//    if (pLH5810->lh5810.r_opa & 0x04)	((Cpc15XX *)pPC->pTIMER->pPC)->pce152->paused = true;	// RMT 0 OFF
//    pLH5810_2->SetRegBit(CLH5810::OPB,0,true);
//    pLH5810_2->SetRegBit(CLH5810::OPB,1,true);
//    pLH5810_2->SetRegBit(CLH5810::OPB,2,true);
//    pLH5810_2->SetRegBit(CLH5810::OPB,3,false);
//    pLH5810_2->SetRegBit(CLH5810::OPB,4,true);
//    pLH5810_2->SetRegBit(CLH5810::OPB,5,true);
//    pLH5810_2->SetRegBit(CLH5810::OPB,6,true);
//    pLH5810_2->SetRegBit(CLH5810::OPB,7,true);  // Centronic BUSY ???

    if (keyPressed != 0xff) {
        pLH5810->Regs_Info(0);
//        qWarning()<<pLH5810->Regs_String;
        quint16 strobe = ((PB>>2)<<8) | PC ;
        if (strobe & (1 << (keyPressed >>4))) {
//            qWarning()<<"Match Strobe:"<<QString("%1").arg(strobe,4,16,QChar('0'))<<QString("%1").arg(keyPressed,2,16,QChar('0'));

            PA = (keyPressed+1) & 0x0f;
//            qWarning()<<"OpA="<<QString("%1").arg(PA,2,16,QChar('0'));
        }
        else PA = 0;
    }

    pLH5810->step();

//    bus->setINT(pLH5810->INT);

    if (bus->isEnable() &&
            !bus->isWrite() &&
            bus->isME1() &&
            (addr >= 0x8000) &&
            (addr <= 0x800f)) {
        lh5810_read();
        if ((addr==0x800e) && (bus->getData()!=0))
            qWarning()<<"Read "<<QString("%1").arg(addr,4,16,QChar('0'))<<"="<<QString("%1").arg(bus->getData(),2,16,QChar('0'));

    }

    bus->setEnable(false);
    pCONNECTOR->Set_values(bus->toUInt64());

    return true;
}

void Cce153::mousePressEvent(QMouseEvent *event)
{
//    qWarning()<<"CPObject::mousePressEvent"<<event;
//    CPObject::mousePressEvent(event);

    if (event->button() != Qt::LeftButton) {
        event->accept();
        return;
    }

    setFocus();

    if (event->modifiers() == Qt::MetaModifier) return;

    QPoint pts(event->x() , event->y());
    QRect kbdZone(118,170,636,382);

    if (kbdZone.contains(pts)) {
        int x = pts.x() - kbdZone.x();
        int y = pts.y() - kbdZone.y();
        int col = x / 45;
        int row = y / 38;
        keyPressed = ((col & 0xf) << 4 ) | (row & 0xf);
        qWarning()<<"Key pressed:"<<QString("%1").arg(keyPressed,2,16,QChar('0'));
    }

}

void Cce153::mouseReleaseEvent(QMouseEvent *event)
{
    keyPressed = 0xff;
    PA = 0;
}

bool Cce153::exit(void)
{

    CPObject::exit();

    return true;
}


INLINE bool Cce153::lh5810_write(void)
{
    switch (bus->getAddr()) {
    case 0x8008: pLH5810->SetReg(CLH5810::OPC,bus->getData()); break;
    case 0x8009: pLH5810->SetReg(CLH5810::G  ,bus->getData()); break;
    case 0x800A: pLH5810->SetReg(CLH5810::MSK,bus->getData()); break;
    case 0x800B: pLH5810->SetReg(CLH5810::IF ,bus->getData()); break;
    case 0x800C: pLH5810->SetReg(CLH5810::DDA,bus->getData()); break;
    case 0x800D: pLH5810->SetReg(CLH5810::DDB,bus->getData()); break;
    case 0x800E: pLH5810->SetReg(CLH5810::OPA,bus->getData()); break;
    case 0x800F: pLH5810->SetReg(CLH5810::OPB,bus->getData()); break;
    default:  break;
    }

    return true;
}


INLINE bool Cce153::lh5810_read(void)
{
    switch (bus->getAddr()) {
    case 0x8008: bus->setData( pLH5810->GetReg(CLH5810::OPC) ); break;
    case 0x8009: bus->setData( pLH5810->GetReg(CLH5810::G)); break;
    case 0x800A: bus->setData( pLH5810->GetReg(CLH5810::MSK)); break;
    case 0x800B: bus->setData( pLH5810->GetReg(CLH5810::IF)); break;
    case 0x800C: bus->setData( pLH5810->GetReg(CLH5810::DDA)); break;
    case 0x800D: bus->setData( pLH5810->GetReg(CLH5810::DDB)); break;
    case 0x800E: bus->setData( pLH5810->GetReg(CLH5810::OPA)); break;
    case 0x800F: bus->setData( pLH5810->GetReg(CLH5810::OPB)); break;
    default: /*bus->setData(0x00);*/ break;
    }

    return true;
}


