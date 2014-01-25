#ifndef RLP9001_H
#define RLP9001_H

#include "pobject.h"

#include "mainwindowpockemul.h"

class Crlp9001:public CPObject {
    Q_OBJECT
public:
    const char*	GetClassName(){ return("Crlp9001");}

    Cconnector	*pCONNECTOR;


    bool run(void);
    bool init(void);				//initialize
    bool exit(void);				//end
    virtual bool    SaveSession_File(QXmlStreamWriter *xmlOut);
    virtual bool	LoadSession_File(QXmlStreamReader *xmlIn);

    Crlp9001(CPObject *parent = 0, Models mod=RLP9001);
    virtual ~Crlp9001();

    bool Power;

protected:
    void paintEvent(QPaintEvent *);
protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void Rotate();
    void ROMSwitch();

private:

    bool rotate;
    bool romSwitch;
    Models model;
    quint8 bank;

};

#endif // RLP9001_H
