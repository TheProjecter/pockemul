#ifndef _PC1245_H_
#define _PC1245_H_

#include "pc1250.h"

class Cpc1245:public Cpc1250{
public:
    const char*	GetClassName(){ return("Cpc1245");};
	BYTE	Get_PortB(void);

	Cpc1245(CPObject *parent = 0);

private:
};
#endif
