#ifndef _PC1250_H
#define _PC1250_H

#include <stdlib.h>
#include <stdio.h>

#include "extension.h"
#include "pcxxxx.h"
#include "sc61860.h"
#include "Keyb.h"
#include "Connect.h"

#define PS_OFF	0
#define PS_RUN	0
#define PS_RSV	1
#define PS_PRO	2

// WARNING -- TO MODIFY !!!!!!!!!!!!!
//#define KEY(c)	( asfocus && ( GetAsyncKeyState(c) || (toupper(pPC->pKEYB->LastKey)==toupper(c)) || pPC->pKEYB->LastKey == c) )
#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )

class Cpc1250:public CpcXXXX{						//PC1250 emulator main class

public:
    const char*	GetClassName(){ return("Cpc1250");};

	void	TurnON(void);

	
    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);
    virtual BYTE	Get_PortA(void);
	virtual BYTE	Get_PortB(void);

	bool	InitDisplay(void);
	bool	CompleteDisplay(void);

	bool	Mem_Mirror(DWORD *d);
    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    UINT8 in(UINT8 address){return(1);};
    UINT8 out(UINT8 address,UINT8 value){return(1);};

	qint8		PowerSwitch;
	virtual bool	LoadExtra(QFile *);
	virtual bool	SaveExtra(QFile *);

	Cpc1250(CPObject *parent = 0);

	virtual ~Cpc1250()
	{								//[constructor]
	}
protected:
	BYTE previous_key;
};



class Ctrspc3:public Cpc1250{						//PC1250 emulator main class
public:

	Ctrspc3(CPObject *parent = 0)	: Cpc1250(this)
	{								//[constructor]
		setcfgfname("trspc3");

		SessionHeader	= "TRSPC3PKM";
		SessionHeaderLen= 9;
		Initial_Session_Fname ="trspc3.pkm";

        BackGroundFname	= ":/PC1250/pc1250/pc31k.png";
	}


};

#endif


