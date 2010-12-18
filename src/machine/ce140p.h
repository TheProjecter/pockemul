#ifndef CE140P_H
#define CE140P_H

#include "sio.h"
#include "ce515p.h"

class Cce140p:public Cce515p{
Q_OBJECT
public:

    virtual bool run(void);
    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    //virtual void ExportByte(qint8);

    Csio *pSIO;
    Cconnector *pSIOCONNECTOR; qint64 pSIOCONNECTOR_value;



    Cce140p(CPObject *parent = 0);

protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
//    void ShowConsole(void);
//    void HideConsole(void);
};

#endif // CE140P_H
