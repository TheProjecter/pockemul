#ifndef RLH1000_H
#define RLH1000_H

#include "pcxxxx.h"

class Cm6502;

class Crlh1000 : public CpcXXXX
{
    Q_OBJECT

public:
    Crlh1000(CPObject *parent = 0);
    virtual ~Crlh1000();

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    virtual UINT8 in(UINT8 address);
    virtual UINT8 out(UINT8 address,UINT8 value);

    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);

    virtual bool init();
    virtual bool run();
    virtual void Reset();

    void TurnON();
    void TurnOFF();
    bool SaveConfig(QXmlStreamWriter *xmlOut);
    bool LoadConfig(QXmlStreamReader *xmlIn);

    Cm6502 *m6502;

private:
    quint8 latchByte;

};

#endif // RLH1000_H
