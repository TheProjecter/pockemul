//TODO Migrate sound to QMultimedia
//TODO: Save session with Pen position and paper image

#include <QPainter>
#include <QResource>
#include <QBitmap>
#include <QLabel>
#include <QLineEdit>

#include "common.h"
#include "ce1560.h"
#include "Lcdc_ce1560.h"
#include "buspc1500.h"
#include "clink.h"

Cce1560::Cce1560(CPObject *parent):CpcXXXX(this)
{
    Q_UNUSED(parent)

    //[constructor]
    BackGroundFname	= P_RES(":/pc1500/ce1560_1.png");
    LcdFname		= P_RES(":/pc1500/1500lcd.png");
    setcfgfname(QString("ce1560"));
    coverImage = QImage(P_RES(":/pc1500/ce1560_cover.png"));

    setDXmm(329);//Pc_DX_mm = 329;
    setDYmm(115);//Pc_DY_mm = 115;
    setDZmm(48);//Pc_DZ_mm = 48;

//    setDX(481);//Pc_DX	= 960;
//    setDY(550);//Pc_DY	= 320;
    setDX(752);//Pc_DX	= 960;
    setDY(679);//Pc_DY	= 320;

//    Lcd_X		= 48;
//    Lcd_Y		= 53;
    Lcd_X		= 188;
    Lcd_Y		= 60;
    Lcd_DX		= 192;//168;//144 ;
    Lcd_DY		= 64;
    Lcd_ratio_X	= 2;//348.0/240;
    Lcd_ratio_Y	= 2;//60.0/32;

    pLCDC		= new Clcdc_ce1560(this);
    pTIMER		= new Ctimer(this);
    pKEYB		= new Ckeyb(this,"ce1560.map");

    bus = new CbusPc1500();

    for (int i=0;i<3;i++) ps6b0108[i]  = new CS6B0108(this);

    screenOpen = true;
    screenFlipping = false;
    m_screenAngle = 180;
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
//    pCONNECTOR	= new Cconnector(this,60,0,Cconnector::Sharp_60,"Connector 60 pins",true,QPoint(380,332));	publish(pCONNECTOR);
    pCONNECTOR	= new Cconnector(this,60,0,Cconnector::Sharp_60,"Connector 60 pins",true,QPoint(62,450));	publish(pCONNECTOR);
    pEXTCONNECTOR= new Cconnector(this,60,1,Cconnector::Sharp_60,"Connector 60 pins Ext",false,QPoint(0,447),Cconnector::WEST);	publish(pEXTCONNECTOR);

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();

    return(1);

}

bool Cce1560::run(void)
{

    bool forwardBus = true;

    bus->fromUInt64(pCONNECTOR->Get_values());

    if (!bus->isEnable()) return true;

    if (bus->isME1() && (bus->getAddr()>=0xE200) && (bus->getAddr()<=0xE20F))
//        if (bus->isME1() && (bus->getAddr()>=0x0000) && (bus->getAddr()<=0x000F))
    {
        forwardBus = false;
        quint8 module = (bus->getAddr() >> 1 );
        quint8 reg = (bus->getAddr()&0x0f) - (module*2);
//        if (module >2) {
//            qWarning()<<"ERROR Write Data:"<<bus->toLog()<<"  module="<<module<<" reg="<<reg<<" data="<<bus->getData();
//            bus->setEnable(false);
//            pCONNECTOR->Set_values(bus->toUInt64());
//            return true;
//        }
        if (bus->isWrite()) {
            quint16 cmd = ((reg==0)? 0x00 : 0x100) | bus->getData()  ;
//                    qWarning()<<"Write Data:"<<bus->toLog()<<"  module="<<module<<" reg="<<reg<< "cmd:"<<cmd;

            switch (module) {
            case 0:
            case 1:
            case 2: ps6b0108[module]->instruction(cmd); break;
            case 3: ps6b0108[0]->instruction(cmd); ps6b0108[1]->instruction(cmd); break;
            case 4: ps6b0108[0]->instruction(cmd); ps6b0108[2]->instruction(cmd); break;
            case 5: ps6b0108[1]->instruction(cmd); ps6b0108[2]->instruction(cmd); break;
            case 6: ps6b0108[0]->instruction(cmd); ps6b0108[1]->instruction(cmd); ps6b0108[2]->instruction(cmd); break;
            default:
                qWarning()<<"ERROR Write Data:"<<bus->toLog()<<"  module="<<module<<" reg="<<reg<<" data="<<bus->getData();
                break;
            }


        }
        else {
            qWarning()<<"Read Data:"<<bus->getAddr()<<"="<<bus->toLog();
            quint16 cmd = ((reg==0)? 0x200 : 0x300);// | bus->getData();

            switch (module) {
            case 0:
            case 1:
            case 2:
                bus->setData(ps6b0108[module]->instruction(cmd)); break;
            case 3:
                bus->setData(ps6b0108[0]->instruction(cmd));
                bus->setData(ps6b0108[1]->instruction(cmd)); break;
            case 4:
                bus->setData(ps6b0108[0]->instruction(cmd));
                bus->setData(ps6b0108[2]->instruction(cmd));
                break;
            case 5:
                bus->setData(ps6b0108[1]->instruction(cmd));
                bus->setData(ps6b0108[2]->instruction(cmd));
                break;
            case 6:
                bus->setData(ps6b0108[0]->instruction(cmd));
                bus->setData(ps6b0108[1]->instruction(cmd));
                bus->setData(ps6b0108[2]->instruction(cmd));
                break;
            default:
                qWarning()<<"ERROR Read Data:"<<bus->toLog()<<"  module="<<module<<" reg="<<reg<<" data="<<bus->getData();
                break;
            }

            bus->setData(ps6b0108[module]->instruction(cmd));
        }

    }

    // Manage EXT Connector
    if (forwardBus) {
        // copy MainConnector to Ext Connectors
        pEXTCONNECTOR->Set_values(bus->toUInt64());
        // execute Ext
        mainwindow->pdirectLink->outConnector(pEXTCONNECTOR);
        bus->fromUInt64(pEXTCONNECTOR->Get_values());
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

bool Cce1560::LoadConfig(QXmlStreamReader *xmlIn)
{
    for (int i=0; i<3; i++) {
        ps6b0108[i]->Load_Internal(xmlIn);
    }
    return true;
}

bool Cce1560::SaveConfig(QXmlStreamWriter *xmlOut)
{
    for (int i=0; i<3; i++) {
        ps6b0108[i]->save_internal(xmlOut);
    }
    return true;
}

//extern int ask(QWidget *parent,QString msg,int nbButton);
#define KEY(c)	((pKEYB->keyPressedList.contains(TOUPPER(c)) || \
                  pKEYB->keyPressedList.contains(c) || \
                  pKEYB->keyPressedList.contains(TOLOWER(c)))?1:0)
void Cce1560::ComputeKey()
{

    // Manage backdoor click
//    qWarning()<<"lastkey="<<pKEYB->LastKey<<" -"<<pKEYB->keyPressedList<<" - "<<( (currentView==FRONTview) && ( (screenOpen && KEY(0x240) ) || (!screenOpen && KEY(0x241) )));
    if  ( (currentView==FRONTview) && ( (screenOpen && KEY(0x240) ) || (!screenOpen && KEY(0x241) ))) {
        pKEYB->keyPressedList.removeAll(0x240);
        pKEYB->keyPressedList.removeAll(0x241);
        screenOpen = !screenOpen;
//        qWarning()<<"compute key screen to:"<<screenOpen;
        animateScreen();
    }

}

bool Cce1560::UpdateFinalImage(void) {

    delete FinalImage;
    FinalImage = new QImage(*BackgroundImageBackup);
    pLCDC->Refresh=true;
    CpcXXXX::UpdateFinalImage();




//    if ((currentView != FRONTview) ) {
        QPainter painter;
        painter.begin(FinalImage);

        QImage screenImage = FinalImage->copy(60,0,692,getDY()/2).mirrored(false,true);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(60,0,692,getDY()/2,Qt::transparent);

        painter.translate((getDX()-60)/2+60,getDY()/2);
        QTransform matrix2;
        matrix2.rotate(m_screenAngle, Qt::XAxis);
        painter.setTransform(matrix2,true);

        if (m_screenAngle<=90)
            screenImage = coverImage.copy(60,0,692,getDY()/2);

        painter.drawImage(-getDX()/2+60/2,0,screenImage);
        painter.end();
        mask = QPixmap::fromImage(*FinalImage).scaled(getDX()*mainwindow->zoom/100,getDY()*mainwindow->zoom/100);

        setMask(mask.mask());

        if (!forceStackOver && (m_screenAngle <= 90)) {
            forceStackOver=true;
            QList<CPObject *> list;
            mainwindow->pdirectLink->findAllObj(this,&list);
            for (int i=0;i<list.size();i++) {
                list.at(i)->raise();
            }
            list.clear();

            manageStackPos(&list);
        }
        if ( forceStackOver && (m_screenAngle > 90)) {
            forceStackOver=false;
            QList<CPObject *> list;
            mainwindow->pdirectLink->findAllObj(this,&list);
            for (int i=0;i<list.size();i++) {
                list.at(i)->raise();
            }
            list.clear();

            manageStackPos(&list);
        }




    return true;
}

void Cce1560::animateScreen() {
//    qWarning()<<"ANIMATE";
    QPropertyAnimation *animation1 = new QPropertyAnimation(this, "screenangle");
     animation1->setDuration(1500);
     if (!screenOpen) {
         animation1->setStartValue(m_screenAngle);
         animation1->setEndValue(0);
     }
     else {
         animation1->setStartValue(m_screenAngle);
         animation1->setEndValue(180);
     }

     QParallelAnimationGroup *group = new QParallelAnimationGroup;
     group->addAnimation(animation1);

     connect(animation1,SIGNAL(valueChanged(QVariant)),this,SLOT(update()));
     connect(animation1,SIGNAL(finished()),this,SLOT(endscreenAnimation()));
     screenFlipping = true;
     group->start();

}

void Cce1560::endscreenAnimation()
{
    screenFlipping = false;
    if (screenOpen) {
//        pKEYB->Keys[backdoorKeyIndex].Rect.setSize(QSize(105,145));
    }
    else {
//        pKEYB->Keys[backdoorKeyIndex].Rect.setSize(QSize(365,145));
    }
}
