//TODO Migrate sound to QMultimedia
//TODO: Save session with Pen position and paper image

#include <QPainter>
#include <QResource>

#include "common.h"
#include "ce1560.h"
#include "Lcdc_ce1560.h"
#include "buspc1500.h"

Cce1560::Cce1560(CPObject *parent):CpcXXXX(this)
{
    Q_UNUSED(parent)

    //[constructor]
    BackGroundFname	= P_RES(":/pc1500/ce1560.png");
    LcdFname		= P_RES(":/pc1500/1500lcd.png");
    setcfgfname(QString("ce1560"));

    setDXmm(329);//Pc_DX_mm = 329;
    setDYmm(115);//Pc_DY_mm = 115;
    setDZmm(48);//Pc_DZ_mm = 48;

    setDX(481);//Pc_DX	= 960;
    setDY(550);//Pc_DY	= 320;

    Lcd_X		= 48;
    Lcd_Y		= 53;
    Lcd_DX		= 192;//168;//144 ;
    Lcd_DY		= 64;
    Lcd_ratio_X	= 2;//348.0/240;
    Lcd_ratio_Y	= 2;//60.0/32;

    pLCDC		= new Clcdc_ce1560(this);
    pTIMER		= new Ctimer(this);
//    pKEYB		= new Ckeyb(this,"ce150.map");

    bus = new CbusPc1500();

    for (int i=0;i<3;i++) ps6b0108[i]  = new CS6B0108(this);

}

Cce1560::~Cce1560() {
    delete pCONNECTOR;
    delete bus;

    for (int i=0;i<3;i++) delete ps6b0108[i];
}

bool Cce1560::init(void)
{
    CpcXXXX::init();

    setfrequency( 0);
    pCONNECTOR	= new Cconnector(this,60,0,Cconnector::Sharp_60,"Connector 60 pins",true,QPoint(380,332));	publish(pCONNECTOR);

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();

    return(1);

}

bool Cce1560::run(void)
{
    bus->fromUInt64(pCONNECTOR->Get_values());

    if (!bus->isEnable()) return true;

    if (bus->isME1() && (bus->getAddr()<=0x000F))
    {

        if (bus->isWrite()) {
            quint8 module = (bus->getAddr() >> 1 ) & 0x03;
            quint8 reg = (bus->getAddr()&0x07) - (module*2);

            quint16 cmd = ((reg==0)? 0x0 : 0x100) | bus->getData();
//                    qWarning()<<"Write Data:"<<bus->toLog()<<"  module="<<module<<" reg="<<reg<< "cmd:"<<cmd;

            ps6b0108[module]->instruction(cmd);
        }
        else {
            qWarning()<<"Read Data:"<<bus->getAddr()<<"="<<bus->toLog();
            bus->setData(0x10);
        }

    }
    bus->setEnable(false);
    pCONNECTOR->Set_values(bus->toUInt64());

    return true;
}


bool Cce1560::exit(void)
{

    CPObject::exit();

    return true;
}



