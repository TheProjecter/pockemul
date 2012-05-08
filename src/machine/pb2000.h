#ifndef CPB2000_H
#define CPB2000_H

#include "pb1000.h"

class CHD44352;

class Cpb2000 : public Cpb1000
{
    Q_OBJECT

public:
    Cpb2000(CPObject *parent = 0);
    virtual ~Cpb2000();

    virtual bool	LoadConfig(QXmlStreamReader *);
    virtual bool	SaveConfig(QXmlStreamWriter *);

    virtual void initExtension(void);
    virtual void TurnON();
    virtual bool init();
    virtual void Reset();
    virtual bool run();
    virtual bool Chk_Adr(DWORD *d, DWORD data);
    virtual bool Chk_Adr_R(DWORD *d, DWORD data);

    virtual bool Set_Connector(void);
    virtual bool Get_Connector(void);

    virtual UINT16 getKey();

    virtual bool UpdateFinalImage(void);
    virtual void paintEvent(QPaintEvent *event);
    virtual void TurnCLOSE();
    virtual UINT8 readPort();
    virtual WORD Get_16rPC(DWORD adr);
    virtual void MemBank(DWORD *d);

};

#endif // CPB2000_H
