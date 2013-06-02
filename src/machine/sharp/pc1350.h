#ifndef _PC1350_H
#define _PC1350_H

#include <stdlib.h>
#include <stdio.h>



#include "pcxxxx.h"



extern BYTE scandef_pc1350[];

class Cpc13XX:public CpcXXXX{						//PC1350 emulator main class
Q_OBJECT
public:
    virtual const char*	GetClassName(){ return("Cpc13XX");}

	virtual void	Set_PortA(BYTE data);
	virtual BYTE	Get_PortA(void);
	virtual bool	init(void);
    virtual void	initExtension(void){}
    virtual UINT8   in(UINT8 address){return(1);}
    virtual UINT8   out(UINT8 address,UINT8 value){return(1);}
    virtual bool    CheckUpdateExtension(CExtension *ext);
	
    Cpc13XX(CPObject *parent = 0);

	virtual ~Cpc13XX()
	{								//[constructor]
	}
private:
    BYTE cnt;
};

class Cpc1350:public Cpc13XX{						//PC1350 emulator main class
Q_OBJECT
public:
    virtual BYTE	Get_PortA(void);
    virtual BYTE	Get_PortB(void);
    virtual void	Set_PortB(BYTE data);
    virtual void	Set_PortF(BYTE data);
		
	virtual bool	Chk_Adr(DWORD *d,DWORD data);
	virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
	virtual void	initExtension(void);
    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);
    virtual bool	run(void);				// emulator main

    Cpc1350(CPObject *parent = 0);

    virtual ~Cpc1350()
	{								//[constructor]
	}

};
#endif
