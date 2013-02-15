#ifndef PC2001_H
#define PC2001_H

class CPObject;
class Cupd7810;
class CUPD16434;

#include "pcxxxx.h"


class Cpc2001 : public CpcXXXX
{
    Q_OBJECT

public:
    Cpc2001(CPObject *parent = 0);
    virtual ~Cpc2001();

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    virtual UINT8 in(UINT8 address);
    virtual UINT8 out(UINT8 address,UINT8 value);

    bool init();

    virtual bool run();

    virtual void Reset();

    void TurnON();
    void TurnOFF();
    bool SaveConfig(QXmlStreamWriter *xmlOut);
    bool LoadConfig(QXmlStreamReader *xmlIn);

    UINT16 getKey();

    Cupd7810 *upd7810;
    CUPD16434 *upd16434[4];

private:


};
#endif // PC2001_H
