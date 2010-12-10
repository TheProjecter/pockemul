#include <QPainter>

#include "ccable.h"
#include "init.h"

bool Ccable::init(void) {
    CPObject::init();

    setfrequency( 0);

    WatchPoint.add(&pCONNECTOR_A_value,64,11,this,"11pins connector A");
    WatchPoint.add(&pCONNECTOR_B_value,64,11,this,"11pins connector B");
    return true;
}

bool Ccable::run(void)
{
    // Invert pins
    // Identification input/output pins
    // 6 : tape in
    // 7 : tape out

//    Cconnector loc_A(this,11,"Connector 11 pins",false);
//    Cconnector loc_B(this,11,"Connector 11 pins",false);
//    loc_A.values = pCONNECTOR_A->values;
//    loc_B.values = pCONNECTOR_B->values;

    pCONNECTOR_A->Set_pin(6,pCONNECTOR_B->Get_pin(7));
    pCONNECTOR_B->Set_pin(6,pCONNECTOR_A->Get_pin(7));

    pCONNECTOR_A_value = pCONNECTOR_A->Get_values();
    pCONNECTOR_B_value = pCONNECTOR_B->Get_values();

	return true;
}


bool Ccable::exit(void)				//end
{
	return true;
}

