#ifndef _CCABLE_H_
#define _CCABLE_H_

#include "pobject.h"
#include "Connect.h"
#include "Inter.h"

class Ccable:public CPObject{
public:
    const char*	GetClassName(){ return("Ccable");};

		
    Cconnector	*pCONNECTOR_A;  qint64 pCONNECTOR_A_value;
    Cconnector	*pCONNECTOR_B;  qint64 pCONNECTOR_B_value;

	bool run(void);
	bool init(void);				//initialize
	bool exit(void);				//end
	
	Ccable(CPObject *parent = 0)	: CPObject(this)
	{							//[constructor]
		
        pCONNECTOR_A = new Cconnector(this,11,0,"11 pins Connector A",false); publish(pCONNECTOR_A);
        pCONNECTOR_B = new Cconnector(this,11,1,"11 pins Connector B",false); publish(pCONNECTOR_B);
		setfrequency( 0);
		BackGroundFname	= ":/EXT/ext/cable.png";
		
		pTIMER		= new Ctimer(this);
        setDX(150);//Pc_DX	= 150;
        setDY(450);//Pc_DY	= 450;
    }
 
 
	
	virtual ~Ccable(){
		delete(pCONNECTOR_A);
		delete(pCONNECTOR_B);
    }

private:

};


#endif
