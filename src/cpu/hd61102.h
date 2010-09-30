#ifndef HD61102_H
#define HD61102_H

// Display Driver
#include "pobject.h"

class CHD61102:public QObject{



public:
    const char*	GetClassName(){ return("CHD61102");};
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

    BYTE    on_off;
    BYTE    displaySL;
    BYTE    Xadr;
    BYTE    Yadr;
    BYTE    status;

    void	Load_Internal(FILE *ffile);
    void	save_internal(FILE *file);

    CHD61102(CpcXXXX *parent);
//    ~CHD61102();

    void	addretrace (void);

public:

    BYTE imem[0x200];

};

#endif // HD61102_H
