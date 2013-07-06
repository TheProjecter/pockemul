#ifndef RLH1000_H
#define RLH1000_H

#include "pcxxxx.h"

class Cm6502;


#define BUS_SLEEP 0
#define BUS_WRITEDATA 1
#define BUS_READDATA 2
#define BUS_QUERY 3
#define BUS_SELECT 4

class Cbus {

public:
    quint64 toUInt64();
    void fromUInt64(quint64 val);
    UINT8 getData() { return data;}
    void setData(UINT8 val) { data = val;}
    DWORD getAddr() { return addr;}
    void setAddr(DWORD val) { addr=val;}
    UINT8 getDest() { return dest;}
    void setDest(UINT8 val) { dest = val;}
    UINT8 getFunc() { return func;}
    void setFunc(UINT8 val) { func = val;}

private:
    DWORD addr;
    UINT8 data;
    UINT8 dest;
    UINT8 func;


};

class Crlh1000 : public CpcXXXX
{
    Q_OBJECT

public:
    Crlh1000(CPObject *parent = 0);
    virtual ~Crlh1000();

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d, DWORD *data);
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
    quint8 lineFE[0x100];
    quint8 lineFD[0x100];
    quint8 lineFF[0x100];
    quint8 strobe32;
    quint8 extrinsic;
    UINT8 getKey(quint8 row);
    UINT8 manageBus();
private:
    Cbus bus;
    quint8 latchByte;
    quint8 timercnt1,timercnt2,timercnt3;

};

#endif // RLH1000_H
