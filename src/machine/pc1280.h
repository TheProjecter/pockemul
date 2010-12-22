#ifndef PC1280_H
#define PC1280_H


#include "pc1360.h"


#include "Connect.h"
#include "Keyb.h"


class Cpc1280:public Cpc1360{						//PC1350 emulator main class
public:
    const char*	GetClassName(){ return("Cpc1280");};

    bool Chk_Adr(DWORD *d,DWORD data);
    bool Chk_Adr_R(DWORD *d,DWORD data);

    virtual bool	init(void);
    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);
    virtual void	initExtension(void);

    Cpc1280(CPObject *parent = 0);

    ~Cpc1280()
    {								//[constructor]
    }
private:
};


#endif // PC1280_H
