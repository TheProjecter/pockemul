#ifndef CESIMU_H
#define CESIMU_H

#include <QMenu>
#include "Connect.h"
#include "pobject.h"
#include "Inter.h"
#include "init.h"

#define TICKS_BDS	(pTIMER->pPC->getfrequency()/baudrate)
class DialogSimulator;

class Ccesimu:public CPObject{
Q_OBJECT

public:
    const char*	GetClassName(){ return("Ccesimu");};


    Cconnector	*pCONNECTOR; qint64 pCONNECTOR_value;

    virtual bool run(void);

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end

    DialogSimulator *dialogconsole;

    Ccesimu(CPObject *parent = 0);

    virtual ~Ccesimu(){
        delete(pCONNECTOR);
    };
public:
    void paintEvent(QPaintEvent *);

protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void ShowDialog(void);
    void HideDialog(void);


};


#endif // CESIMU_H
