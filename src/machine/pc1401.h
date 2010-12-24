#ifndef _PC1401_H
#define _PC1401_H

#include <stdlib.h>
#include <stdio.h>


#include "pcxxxx.h"
#include "sc61860.h"
#include "Keyb.h"
#include "Connect.h"

extern TransMap KeyMap1401[];
extern int KeyMap1401Lenght; 

class Cpc1401:public CpcXXXX{						//PC1403 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1401");};

	void	TurnON(void);

	virtual bool	Set_Connector(void);
	virtual bool	Get_Connector(void);
    virtual BYTE	Get_PortA(void);
    virtual void	Set_PortA(BYTE data);
    virtual void	Set_PortB(BYTE data);

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    virtual UINT8   in(UINT8 address){return(1);}
    virtual UINT8   out(UINT8 address,UINT8 value){return(1);}
	
	virtual bool CheckUpdateExtension(CExtension *ext);
			
    Cpc1401(CPObject *parent = 0);

	~Cpc1401()
	{								//[constructor]
	}
private:
    BYTE cnt;
};




#endif
