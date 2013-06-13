#include <QPainter>


#include "common.h"
#include "Log.h"
#include "pcxxxx.h"
#include "Connect.h"

#include "rlp6001.h"

Crlp6001::Crlp6001(CPObject *parent )	: CPObject(this)
{							//[constructor]


    setfrequency( 0);
    BackGroundFname	= P_RES(":/rlh1000/rlp6001.png");

    setDXmm(85);
    setDYmm(318);
    setDZmm(51);
 // Ratio = 3,57
    setDX(303);//Pc_DX	= 75;
    setDY(1135);//Pc_DY	= 20;

}

Crlp6001::~Crlp6001(){
    delete(pMAINCONNECTOR);

    for (int i=0;i<6;i++) delete(pEXTCONNECTOR[i]);
}


bool Crlp6001::run(void)
{


    return true;
}




/*****************************************************************************/
/* Initialize Potar															 */
/*****************************************************************************/
bool Crlp6001::init(void)
{
    AddLog(LOG_MASTER,"RL-P6001 initializing...");

    CPObject::init();

    pMAINCONNECTOR = new Cconnector(this,44,0,Cconnector::Panasonic_44,"44 pins conector",false,QPoint(20,0)); publish(pMAINCONNECTOR);

    for (int i=0;i<6;i++) {
        pEXTCONNECTOR[i] = new Cconnector(this,44,0,Cconnector::Panasonic_44,"44 pins conector",true,QPoint(20,0));
        publish(pEXTCONNECTOR[i]);

    }

    AddLog(LOG_MASTER,"done.\n");
    return true;
}

/*****************************************************************************/
/* Exit Potar																	 */
/*****************************************************************************/
bool Crlp6001::exit(void)
{
    return true;
}


void Crlp6001::paintEvent(QPaintEvent *event)
{
    CPObject::paintEvent(event);
}

void Crlp6001::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    BuildContextMenu(&menu);

//    menu.addSeparator();

//    menu.addAction(tr("Show console"),this,SLOT(ShowConsole()));
//    menu.addAction(tr("Hide console"),this,SLOT(HideConsole()));

    menu.exec(event->globalPos () );
}

