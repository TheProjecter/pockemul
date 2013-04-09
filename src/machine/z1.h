#ifndef Z1_H
#define Z1_H

class CPObject;
class CHD66108;
class Ci80x86;
class CCF79107PJ;
class Cctronics;

#include "pcxxxx.h"

class Cz1 : public CpcXXXX
{
    Q_OBJECT

public:
    Cz1(CPObject *parent = 0);
    virtual ~Cz1();

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    virtual UINT8 in(UINT8 address);
    virtual UINT8 out(UINT8 address,UINT8 value);
    virtual UINT8 in8(UINT16 Port);
    virtual UINT8 out8(UINT16 Port,UINT8 x);
    virtual UINT16 in16(UINT16 Port);
    virtual UINT16 out16(UINT16 Port, UINT16 x);
    virtual bool Get_Connector();
    virtual bool Set_Connector();
    virtual void Get_CentConnector();
    virtual void Set_CentConnecor();
    virtual void Get_SIOConnector();
    virtual void Set_SIOConnector();
    bool init();

    CHD66108 *pHD66108;
    CCF79107PJ *pFPU;
    Cctronics *pCENT;

    virtual bool run();

    virtual void Reset();

    void TurnON();
    void TurnOFF();
    bool SaveConfig(QXmlStreamWriter *xmlOut);
    bool LoadConfig(QXmlStreamReader *xmlIn);

    UINT16 getKey();

    Ci80x86 *i86cpu;

    Cconnector *pCENTCONNECTOR;    qint64      pCENTCONNECTOR_value;



private:
    UINT16 ks;
    UINT16 eoi;
    UINT8 io_b8;
    UINT16 timer0Control;
    UINT16 timer1Control;
    UINT16 timer2Control;

    int intPulseId;
    bool lastIntPulse;
    int lastKeyBufSize;
    bool newKey;


};

class Cz1GR:public Cz1{
};

#endif // Z1_H
