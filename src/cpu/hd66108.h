#ifndef HD66108_H
#define HD66108_H

#include <QFile>


// Display Driver
#include "pobject.h"

typedef struct {
    BYTE    on_off;
    BYTE    displaySL;
    BYTE    Xadr;
    BYTE    Yadr;
    BYTE    status;
    BYTE    imem[0x200];
} HD66108info;

class CHD66108:public QObject{



public:
    const char*	GetClassName(){ return("CHD66108");}
    CpcXXXX		*pPC;

    bool	init(void);						//initialize
    bool	exit(void);						//end
    void	Reset(void);
    bool	step(void);
    BYTE    instruction(qint16 cmd);

    void    cmd_on_off(qint16 cmd);
    void    cmd_displaySL(qint16 cmd);
    void    cmd_setX(qint16 cmd);
    void    cmd_setY(qint16 cmd);
    BYTE    cmd_status(qint16 cmd);
    void    cmd_write(qint16 cmd);
    BYTE    cmd_read(qint16 cmd);

    BYTE    get8(qint16 adr);
    void    set8(qint16 adr,BYTE val);

    HD66108info info;
    bool    updated;

    void	Load_Internal(QXmlStreamReader *);
    void	save_internal(QXmlStreamWriter *);

    CHD66108(CpcXXXX *parent);
    virtual ~CHD66108();

    void	addretrace (void);


};

#endif // HD66108_H
