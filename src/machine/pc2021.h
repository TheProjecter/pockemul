#ifndef PC2021_H
#define PC2021_H


#include <QTime>
#include <QSound>

#include "common.h"
#include "cprinter.h"

#include "Inter.h"

class Cconnector;

class Cpc2021:public Cprinter{
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

    QImage *pc2021buf;
    QImage *pc2021display;

    void Refreshpc2021(qint8 data);
    QSound *bells;

    bool	ToDestroy;

    void	settop(int value){ top = value; }
    void	setposX(int value) { posX = value; }
int pc2021_Mode;

    Cpc2021(CPObject *parent = 0);

    virtual ~Cpc2021();

public:
    bool    rmtSwitch;
    int     internal_device_code;
    void	Printer(qint8 d);			//printer emulator
    QImage *charTable;
    int margin;
    int		top;

private:


    void	pulldownsignal(void);


    int		posX;
    QByteArray	TextBuffer;
    bool	Previous_BUSY;
    bool	Previous_MT_OUT1;
    QTime	time;
    bool	ctrl_char;
    BYTE	t,c;
    qint64	run_oldstate;
    int     code_transfer_step;
    int     device_code;
    qint64  lastState;


public:
    bool MT_OUT2;
    bool BUSY;
    bool D_OUT;
    bool MT_IN;
    bool MT_OUT1;
    bool D_IN;
    bool ACK;
    bool SEL2;
    bool SEL1;

};



#endif // PC2021_H
