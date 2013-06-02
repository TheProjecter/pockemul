#ifndef _PC1450_H_
#define _PC1450_H_

#include <stdlib.h>
#include <stdio.h>


#include "pc1350.h"



class Cpc1450:public Cpc1350{						//PC1350 emulator main class
Q_OBJECT
public:
    const char*	GetClassName(){ return("Cpc1450");}


    virtual void	initExtension(void);
    virtual BYTE	Get_PortA(void);

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    UINT8	in(UINT8 address){return(1);}
    UINT8 out(UINT8 address,UINT8 value){return(1);}
	

    Cpc1450(CPObject *parent = 0);

    virtual ~Cpc1450()
	{								//[constructor]
	}
private:
};

#endif
