#include <QPainter>

#include "mainwindowpockemul.h"
#include "dialoganalog.h"

#include "ccable.h"
#include "init.h"

//FIXME works between 1251 & 1262 but not with same frequency ... strange


extern MainWindowPockemul *mainwindow;

Ccable::Ccable(CPObject *parent)	: CPObject(this)
{							//[constructor]

    pCONNECTOR_A = new Cconnector(this,11,0,"11 pins Connector A",true,QPoint(135,6)); publish(pCONNECTOR_A);
    pCONNECTOR_B = new Cconnector(this,11,1,"11 pins Connector B",true,QPoint(135,300)); publish(pCONNECTOR_B);
    setfrequency( 0);
    BackGroundFname	= ":/EXT/ext/cable.png";

    pTIMER		= new Ctimer(this);
    setDX(150);
    setDY(450);
}


bool Ccable::init(void) {
    CPObject::init();


    WatchPoint.add(&pCONNECTOR_A_value,64,11,this,"11pins connector A");
    WatchPoint.add(&pCONNECTOR_B_value,64,11,this,"11pins connector B");
    return true;
}
//********************************************************/
// PIN_MT_OUT2	1
// PIN_GND		2
// PIN_VGG		3
// PIN_BUSY		4
// PIN_D_OUT	5
// PIN_MT_IN	6
// PIN_MT_OUT1	7
// PIN_D_IN		8
// PIN_ACK		9
// PIN_SEL2		10
// PIN_SEL1		11
//********************************************************/
bool Ccable::run(void)
{
    // access both timer by connectors
    // the run function is fired by the fastest one


    // Invert pins
    // Identification input/output pins
    // 6 : tape in
    // 7 : tape out

//    Cconnector loc_A(this,11,"Connector 11 pins",false);
//    Cconnector loc_B(this,11,"Connector 11 pins",false);
//    loc_A.values = pCONNECTOR_A->values;
//    loc_B.values = pCONNECTOR_B->values;

    pCONNECTOR_A->Set_pin(4,pCONNECTOR_B->Get_pin(9));
    pCONNECTOR_B->Set_pin(4,pCONNECTOR_A->Get_pin(9));

    pCONNECTOR_A->Set_pin(8,pCONNECTOR_B->Get_pin(5));
    pCONNECTOR_B->Set_pin(8,pCONNECTOR_A->Get_pin(5));

    pCONNECTOR_A->Set_pin(6,pCONNECTOR_B->Get_pin(7));
    pCONNECTOR_B->Set_pin(6,pCONNECTOR_A->Get_pin(7));

    pCONNECTOR_A_value = pCONNECTOR_A->Get_values();
    pCONNECTOR_B_value = pCONNECTOR_B->Get_values();

    if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 8;
//    pTIMER->state+=1;
	return true;
}


bool Ccable::exit(void)				//end
{
	return true;
}

