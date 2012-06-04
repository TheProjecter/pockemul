#ifndef Z1_H
#define Z1_H

class CPObject;
#include "pcxxxx.h"

class Cz1 : public CpcXXXX
{
    Q_OBJECT

public:
    Cz1(CPObject *parent = 0);
    virtual ~Cz1();

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    UINT8 in(UINT8 address);
    UINT8 out(UINT8 address,UINT8 value);
    bool init();


    virtual bool run();

    virtual void Reset();

    void TurnON();
    void TurnOFF();
    bool SaveConfig(QXmlStreamWriter *xmlOut);
    bool LoadConfig(QXmlStreamReader *xmlIn);

private:
    UINT16 ks;

};

class Cz1GR:public Cz1{
};

#endif // Z1_H
