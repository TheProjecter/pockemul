#include "ce120p.h"
#include "dialoganalog.h"

// TODO Add support for extended 11 pins connector

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

    bool res = Cce126::init();
    WatchPoint.add(&pCONNECTOR_Ext_value,64,11,this,"Ext. 11pins connector");

    return res;
}

bool Cce120p::exit(void) {

    return Cce126::exit();
}


bool Cce120p::Get_Connector(void) {
    MT_OUT2	= GET_PIN(PIN_MT_OUT2);
    BUSY    = GET_PIN(PIN_BUSY);
    D_OUT	= GET_PIN(PIN_D_OUT);
    MT_IN	= GET_PIN(PIN_MT_IN);
    MT_OUT1	= GET_PIN(PIN_MT_OUT1);
    D_IN	= GET_PIN(PIN_D_IN);
    ACK		= GET_PIN(PIN_ACK);
    SEL2	= GET_PIN(PIN_SEL2);
    SEL1	= GET_PIN(PIN_SEL1);

    return true;
}

bool Cce120p::Set_Connector(void) {
    SET_PIN(PIN_MT_OUT2,MT_OUT2);
    SET_PIN(PIN_BUSY,BUSY);
    SET_PIN(PIN_D_OUT,D_OUT);
    SET_PIN(PIN_MT_IN,MT_IN);
    SET_PIN(PIN_MT_OUT1,MT_OUT1);
    SET_PIN(PIN_D_IN,D_IN);
    SET_PIN(PIN_ACK,ACK);
    SET_PIN(PIN_SEL2,SEL2);
    SET_PIN(PIN_SEL1,SEL1);

    return true;
}

bool Cce120p::run(void) {
    pCONNECTOR_Ext_value = pCONNECTOR_Ext->Get_values();

    Get_Connector();


    Cce126::run();

    pCONNECTOR_Ext->Set_values(pCONNECTOR->Get_values());

    Set_Connector();

    return true;
}
