#ifndef PC1425_H
#define PC1425_H

#include "pc1403.h"

class Cpc1425:public Cpc1403{
public:
    const char*	GetClassName(){ return("Cpc1425");}


    virtual bool Chk_Adr(DWORD *d,DWORD data);
    virtual bool Chk_Adr_R(DWORD *d,DWORD data);

    Cpc1425(CPObject *parent=0);

    ~Cpc1425()
    {								//[constructor]
    }

    BYTE Get_PortA();
    BYTE Get_PortB();
private:
};

#endif // PC1425_H
