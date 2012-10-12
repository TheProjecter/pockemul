#ifndef PC1211_H
#define PC1211_H

#include "pcxxxx.h"

class Cpc1211 : public CpcXXXX
{
public:
    Cpc1211(CPObject *parent = 0);
    virtual ~Cpc1211();

    virtual bool	Chk_Adr(DWORD *d,DWORD data) {return true;}
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data) {return true;}
    virtual UINT8 in(UINT8 address) {return 0;}
    virtual UINT8 out(UINT8 address,UINT8 value) {return 0;}
};

#endif // PC1211_H
