#include "ce140p.h"
#include "Keyb.h"

TransMap KeyMapce140p[]={
    {1,	"FEED  ",	K_PFEED,34,234,	9}
};
int KeyMapce140pLenght = 1;


Cce140p::Cce140p(CPObject *parent):Cce515p(this) {
    setfrequency( 0);

    BackGroundFname	= ":/EXT/ext/ce-140p.png";


    pSIOCONNECTOR = new Cconnector(this,15,"Connector 15 pins (Input)",true); publish(pSIOCONNECTOR);
    pTIMER		= new Ctimer(this);
    KeyMap      = KeyMapce140p;
    KeyMapLenght= KeyMapce140pLenght;
    delete pKEYB; pKEYB		= new Ckeyb(this,"ce140p.map");
    Pc_DX	= 640;
    Pc_DY	= 441;
    SnapPts = QPoint(5,250);
    setPaperPos(QPoint(150,-3));
    stackBehind = true;

}


bool Cce140p::init(void) {


    //WatchPoint.add(&pSIOCONNECTOR_value,64,15,this,"Serial 15pins connector");

}

bool Cce140p::run(void) {
    //pSIOCONNECTOR_value = pSIOCONNECTOR->Get_values();


}

bool Cce140p::exit(void) {

}


