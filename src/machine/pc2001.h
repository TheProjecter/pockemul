#ifndef PC2001_H
#define PC2001_H

class CPObject;
class Cupd7907;
class CUPD16434;
class CPD1990AC;
class Cconnector;

#include "pcxxxx.h"


class Cpc2001 : public CpcXXXX
{
    Q_OBJECT

public:
    Cpc2001(CPObject *parent = 0);
    virtual ~Cpc2001();

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d, DWORD *data);
    virtual UINT8 in(UINT8 address);
    virtual UINT8 out(UINT8 address,UINT8 value);
    virtual UINT16 out16(UINT16 address,UINT16 value);

    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);

    quint16 kstrobe;

    bool init();

    virtual bool run();

    virtual void Reset();

    void TurnON();
    void TurnOFF();
    bool SaveConfig(QXmlStreamWriter *xmlOut);
    bool LoadConfig(QXmlStreamReader *xmlIn);

    UINT16 getKey();

    Cupd7907 *upd7907;
    CUPD16434 *upd16434[4];
    CPD1990AC *pd1990ac;

    Cconnector	*pTAPECONNECTOR;	qint64 pTAPECONNECTOR_value;
    Cconnector	*pPRINTERCONNECTOR;	qint64 pPRINTERCONNECTOR_value;
private:
    quint8 portB;
    quint8 sendToPrinter;

};
#endif // PC2001_H
