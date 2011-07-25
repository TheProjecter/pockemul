#include "ce120p.h"

Cce120p::Cce120p()
{								//[constructor]
    BackGroundFname	= ":/EXT/ext/ce-120p.png";
    setcfgfname("ce120p");
    setDX(883);//Pc_DX	= 883;
    setDY(568);//Pc_DY	= 568;
    SnapPts = QPoint(402,43);
    pCONNECTOR->setSnap(QPoint(400,131));

    pCONNECTOR_Ext = new Cconnector(this,11,1,"Connector 11 pins Ext.",false,QPoint(0,370));	publish(pCONNECTOR_Ext);

    setPaperPos(QRect(89,185-149,207,149));

    delete pKEYB; pKEYB=new Ckeyb(this,"ce120.map");
}


bool Cce120p::init(void) {

    return Cce126::init();
}

bool Cce120p::exit(void) {

    return Cce126::exit();
}

bool Cce120p::run(void) {
    pCONNECTOR_Ext_value = pCONNECTOR_Ext->Get_values();
    Cce126::run();
    pCONNECTOR_Ext->Set_values(pCONNECTOR->Get_values());

    return true;
}
