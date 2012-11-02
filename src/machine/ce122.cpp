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
    setPaperPos(QRect(89,185-149,207,149));

    delete pKEYB; pKEYB=new Ckeyb(this,"ce122.map");
}

