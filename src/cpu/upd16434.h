#ifndef UPD16434_H
#define UPD16434_H

// Display Driver
#include "pobject.h"

typedef struct {
    BYTE    on_off;
    BYTE    dataPointer;
    BYTE    Xadr;
    BYTE    Yadr;
    BYTE    status;
    BYTE    imem[0x80];
} UPD16434info;

class CUPD16434:public QObject{



public:
    const char*	GetClassName(){ return("CUPD16434");}
    CpcXXXX		*pPC;

    bool	init(void);						//initialize
    bool	exit(void);						//end
    void	Reset(void);
    bool	step(void);
    BYTE    instruction(quint8 cmd);
    BYTE    data(quint8 data);

    BYTE    cmd_LDPI(quint8 cmd);
    BYTE    cmd_WRCURS(quint8 cmd);
    BYTE    cmd_CLCURS(quint8 cmd);
    BYTE    cmd_SCMR(quint8 cmd);
    BYTE    cmd_SCML(quint8 cmd);
    BYTE    cmd_SANDM(quint8 cmd);
    BYTE    cmd_SORM(quint8 cmd);
    BYTE    cmd_SWM(quint8 cmd);
    BYTE    cmd_SRM(quint8 cmd);
    BYTE    cmd_BSET(quint8 cmd);
    BYTE    cmd_BRESET(quint8 cmd);
    BYTE    cmd_SMM(quint8 cmd);
    BYTE    cmd_SFF(quint8 cmd);
    BYTE    cmd_DISPON(quint8 cmd);
    BYTE    cmd_DISPOFF(quint8 cmd);
    BYTE    cmd_STOP(quint8 cmd);


    UPD16434info info;
    bool    updated;


    void	Load_Internal(QXmlStreamReader *);
    void	save_internal(QXmlStreamWriter *);

    CUPD16434(CpcXXXX *parent);
    virtual ~CUPD16434();

    void	addretrace (void);


};

#endif // UPD16434_H
