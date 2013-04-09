#ifndef PRINTERCTRONICS_H
#define PRINTERCTRONICS_H


#include <QTime>
#include <QSound>

#include "common.h"
#include "cprinter.h"

#include "Inter.h"

class Cconnector;

class CprinterCtronics:public Cprinter{
public:

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    virtual	bool run(void);					//check access

    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);

    virtual void clearPaper(void);
    virtual void SaveAsText(void);

    virtual void ComputeKey(void);

    //virtual void resizeEvent ( QResizeEvent * );

    Cconnector	*pCONNECTOR;		qint64 pCONNECTOR_value;
    Cconnector *pSavedCONNECTOR;

    QImage *pc2021buf;
    QImage *pc2021display;


    bool	ToDestroy;

    void	settop(int value){ top = value; }
    void	setposX(int value) { posX = value; }
int pc2021_Mode;

    CprinterCtronics(CPObject *parent = 0);

    virtual ~CprinterCtronics();

    bool GoDown(int pin);
    bool GoUp(int pin);
    bool Change(int pin);

public:
    bool printerACK;
    bool printerBUSY;
    bool    rmtSwitch;
    int     internal_device_code;
    virtual void	Printer(qint8 d);			//printer emulator
    QImage *charTable;
    int margin;
    int		top;

private:




    int		posX;
    QByteArray	TextBuffer;
    bool	ctrl_char;
    BYTE	t,c;
    qint64	run_oldstate;
    qint64  lastState;


public:

};

#endif // PRINTERCTRONICS_H
