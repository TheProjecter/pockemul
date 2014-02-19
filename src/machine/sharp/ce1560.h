#ifndef CE1560_H
#define CE1560_H


#include "Log.h"
#include "lh5810.h"
#include "cprinter.h"
#include "Connect.h"
#include "Inter.h"
#include "Keyb.h"
#include "cmotor.h"

#include "s6b0108.h"

class CbusPc1500;

class Cce1560:public CpcXXXX{
public:

    virtual bool	Chk_Adr(UINT32 *d,UINT32 data) {}
    virtual bool	Chk_Adr_R(UINT32 *d, UINT32 *data) {}
    virtual UINT8 in(UINT8 address) {}
    virtual UINT8 out(UINT8 address,UINT8 value) {}
    virtual bool		run(void);
    virtual bool		init(void);
    virtual bool		exit(void);
    virtual bool	LoadConfig(QXmlStreamReader *xmlIn);	// Load PC Configuration
    virtual bool	SaveConfig(QXmlStreamWriter *xmlOut);	// Save PC Configuration

    Cconnector	*pCONNECTOR;	qint64 pCONNECTOR_value;
    CbusPc1500    *bus;

    CS6B0108 *ps6b0108[4];

    Cce1560(CPObject *parent = 0);
    virtual ~Cce1560();

};


#endif // CE1560_H
