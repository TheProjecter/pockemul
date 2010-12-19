#ifndef CE140P_H
#define CE140P_H


#include "sio.h"
#include "ce515p.h"

class DialogConsole;

class Cce140p:public Cce515p{
Q_OBJECT
public:

    virtual bool run(void);
    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    //virtual void ExportByte(qint8);
    virtual void ComputeKey(void);
    virtual void UpdateFinalImage(void);

    Csio *pSIO;
    Cconnector *pSIOCONNECTOR; qint64 pSIOCONNECTOR_value;



    Cce140p(CPObject *parent = 0);
protected:
    void paintEvent(QPaintEvent *);
protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void CommandSlot(qint8);
//    void ShowConsole(void);
//    void HideConsole(void);
private:
    bool    printerSwitch;
};

#endif // CE140P_H
