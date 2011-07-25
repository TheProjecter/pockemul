#ifndef CE120P_H
#define CE120P_H

#include "Ce126.h"

class Cce120p:public Cce126
{
public:
    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    virtual	bool run(void);					//check access

    Cconnector	*pCONNECTOR_Ext;	qint64 pCONNECTOR_Ext_value;

    Cce120p();

};

#endif // CE120P_H
