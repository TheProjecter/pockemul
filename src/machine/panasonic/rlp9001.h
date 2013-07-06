#ifndef RLP9001_H
#define RLP9001_H

#include "pobject.h"

class Crlp9001:public CPObject {
    Q_OBJECT
public:
    const char*	GetClassName(){ return("Crlp9001");}

    Cconnector	*pMAINCONNECTOR;


    bool run(void);
    bool init(void);				//initialize
    bool exit(void);				//end

    Crlp9001(CPObject *parent = 0);
    virtual ~Crlp9001();

    bool Power;

protected:
    void paintEvent(QPaintEvent *);
protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void Rotate();

private:

    bool rotate;

};

#endif // RLP9001_H
