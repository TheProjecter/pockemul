#include "ce140p.h"
#include "Keyb.h"

TransMap KeyMapce140p[]={
    {1,	"FEED  ",	K_PFEED,34,234,	9}
};
int KeyMapce140pLenght = 1;


Cce140p::Cce140p(CPObject *parent):Cce515p(this) {
    //setfrequency( 0);

    BackGroundFname	= ":/EXT/ext/ce-140p.png";


    pSIOCONNECTOR = new Cconnector(this,15,"Connector 15 pins (Input)",true); publish(pSIOCONNECTOR);
    //pTIMER		= new Ctimer(this);
    KeyMap      = KeyMapce140p;
    KeyMapLenght= KeyMapce140pLenght;
    delete pKEYB; pKEYB		= new Ckeyb(this,"ce140p.map");
    Pc_DX	= 895;
    Pc_DY	= 615;
    SnapPts = QPoint(0,360);
    setPaperPos(QRect(200,46,380,170));
    stackBehind = true;

}


bool Cce140p::init(void) {

    Cce515p::init();
    WatchPoint.add(&pSIOCONNECTOR_value,64,15,this,"Serial 15pins connector");
    pSIO = new Csio(this);

    if (pSIO) pSIO->init();

    return true;
}

bool Cce140p::run(void) {
    //pSIOCONNECTOR_value = pSIOCONNECTOR->Get_values();

    return true;
}

bool Cce140p::exit(void) {

    Cce515p::exit();

    return true;
}

void Cce140p::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    BuildContextMenu(&menu);

    menu.addSeparator();

    menu.addAction(tr("Show console"),pSIO,SLOT(ShowConsole()));
    menu.addAction(tr("Hide console"),pSIO,SLOT(HideConsole()));

    menu.exec(event->globalPos () );
}
