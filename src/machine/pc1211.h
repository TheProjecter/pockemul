#ifndef PC1211_H
#define PC1211_H

#include "pcxxxx.h"

class Cpc1211 : public CpcXXXX
{
public:
    Cpc1211(CPObject *parent = 0);
    virtual ~Cpc1211();

    virtual bool	Chk_Adr(DWORD *d,DWORD data) {}
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data) {}
    virtual UINT8 in(UINT8 address) {}
    virtual UINT8 out(UINT8 address,UINT8 value) {}
};

#endif // PC1211_H
