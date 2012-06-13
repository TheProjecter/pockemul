#ifndef CF79107PJ_H
#define CF79107PJ_H

#include <QObject>
#include "common.h"
class CpcXXXX;
class QXmlStreamReader;
class QXmlStreamWriter;



class CCF79107PJ:public QObject{

public:

    typedef struct
    {
        quint64 X;

    } REGS;

    const char*	GetClassName(){ return("CCF79107PJ");}
    CpcXXXX		*pPC;

    bool	init(void);						//initialize
    bool	exit(void);						//end
    void	Reset(void);
    bool	step(void);
    bool    instruction(BYTE cmd);

    void	Load_Internal(QXmlStreamReader *);
    void	save_internal(QXmlStreamWriter *);

    CCF79107PJ(CpcXXXX *parent,int clk);
    virtual ~CCF79107PJ();

private:
    REGS r;

};


#endif // CF79107PJ_H
