#ifndef CE140F_H
#define CE140F_H

/********************************************************************************************************
 * PROGRAM      : test
 * DATE - TIME  : samedi 28 octobre 2006 - 12h40
 * AUTHOR       :  (  )
 * FILENAME     : Ce126.h
 * LICENSE      : GPL
 * COMMENTARY   : printer(CE-126P) emulation class
 ********************************************************************************************************/

#include <QTime>
#include <QSound>


#include "cprinter.h"
#include "Keyb.h"
#include "Connect.h"
#include "Inter.h"

#define GET_PIN(n)		(pCONNECTOR->Get_pin(n))
#define SET_PIN(n,v)	pCONNECTOR->Set_pin(n,v)

class Cce140f:public Cprinter{
public:

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    virtual	bool run(void);					//check access

    virtual void clearPaper(void);
    virtual void SaveAsText(void);

    virtual void ComputeKey(void);

    //virtual void resizeEvent ( QResizeEvent * );

    Cconnector	*pCONNECTOR;		qint64 pCONNECTOR_value;
    Cconnector	*pTAPECONNECTOR;	qint64 pTAPECONNECTOR_value;

    QImage *ce140fbuf;
    QImage *ce140fdisplay;

    void RefreshCe140f(qint8 data);
    QSound *bells;

    bool	ToDestroy;

    void	settop(int value){ top = value; }
    void	setposX(int value) { posX = value; }
int ce140f_Mode;

    Cce140f(CPObject *parent = 0);

    ~Cce140f()
    {
    }

public:
    bool    rmtSwitch;

private:
    void	Printer(qint8 d);			//printer emulator
    QImage *charTable;
    void	pulldownsignal(void);

    int		top;
    int		posX;
    QByteArray	TextBuffer;
    bool	Previous_PIN_BUSY;
    bool	Previous_PIN_MT_OUT1;
    QTime	time;
    bool	ctrl_char;
    BYTE	t,c;
    qint64	run_oldstate;
    int     code_transfer_step;

};


#endif // CE140F_H
