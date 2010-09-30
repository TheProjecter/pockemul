#ifndef _CCABLE_H_
#define _CCABLE_H_

#include "pobject.h"
#include "Connect.h"
#include "Inter.h"

class Ccable:public CPObject{
public:
    const char*	GetClassName(){ return("Ccable");};

		
	Cconnector	*pCONNECTOR_A;
	Cconnector	*pCONNECTOR_B;

	bool run(void);
	bool init(void);				//initialize
	bool exit(void);				//end
	
	Ccable(CPObject *parent = 0)	: CPObject(this)
	{							//[constructor]
		
		
        pCONNECTOR_A = new Cconnector(this,11,"Connector 11 pins",false); publish(pCONNECTOR_A);
        pCONNECTOR_B = new Cconnector(this,11,"Connector 11 pins",false); publish(pCONNECTOR_B);
		setfrequency( 0);
		BackGroundFname	= ":/EXT/ext/cable.png";
		
		pTIMER		= new Ctimer(this);
		Pc_DX	= 150;
		Pc_DY	= 450;
//		SnapPts = QPoint(672,150);
		stackBehind = true;
 	};
 
 
	
	virtual ~Ccable(){
		delete(pCONNECTOR_A);
		delete(pCONNECTOR_B);
	};

private:

};


#endif
