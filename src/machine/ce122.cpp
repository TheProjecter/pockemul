#include <QPainter>
#include <QDebug>

#include "ce122.h"
#include "Connect.h"
#include "Keyb.h"

Cce122::Cce122()
{								//[constructor]
    BackGroundFname	= ":/EXT/ext/ce-122.png";
    setcfgfname("ce122");
    setDX(1002);//Pc_DX	= 282;
    setDY(330);//Pc_DY	= 95;
    setDXmm(282);
    setDYmm(95);
    SnapPts = QPoint(372,107);
    remove(pCONNECTOR); delete pCONNECTOR;
    pCONNECTOR	= new Cconnector(this,9,0,Cconnector::Sharp_9,"Connector 9 pins",true,QPoint(372,107));	publish(pCONNECTOR);
    setPaperPos(QRect(90,0,158,151));

    delete pKEYB; pKEYB=new Ckeyb(this,"ce122.map");

}

bool Cce122::run(void)
{

    if (Power) pCONNECTOR->Set_pin(0,true);
    else pCONNECTOR->Set_pin(0,false);

    return true;
}

void Cce122::ComputeKey(void)
{

    if (pKEYB->LastKey == K_PFEED) {
        RefreshCe126(0x0d);
    }
    if (pKEYB->LastKey == K_RMT_ON) {
        rmtSwitch = true;
    }
    if (pKEYB->LastKey == K_RMT_OFF) {
        rmtSwitch = false;
    }

}

bool Cce122::UpdateFinalImage(void) {
    Cprinter::UpdateFinalImage();

    // Draw switch by 180° rotation
    QPainter painter;

    // PRINTER SWITCH
    painter.begin(FinalImage);
    painter.drawImage(257,257,BackgroundImageBackup->copy(257,257,22,30).mirrored(false,Power));

    painter.end();

    Refresh_Display = true;

    return true;
}

void Cce122::TurnON()
{
    Power = true;
    update();
}

void Cce122::TurnOFF()
{
    Power = false;
    update();
}
