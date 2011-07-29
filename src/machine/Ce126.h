/********************************************************************************************************
 * PROGRAM      : test
 * DATE - TIME  : samedi 28 octobre 2006 - 12h40
 * AUTHOR       :  (  )
 * FILENAME     : Ce126.h
 * LICENSE      : GPL
 * COMMENTARY   : printer(CE-126P) emulation class
 ********************************************************************************************************/
#ifndef _CE126_H
#define _CE126_H

#include <QTime>
#include <QSound>

#include "common.h"
#include "cprinter.h"
#include "Keyb.h"
#include "Connect.h"
#include "Inter.h"


class Cce126:public Cprinter{
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
	Cconnector	*pTAPECONNECTOR;	qint64 pTAPECONNECTOR_value;
	
	QImage *ce126buf;
	QImage *ce126display;
	
	void RefreshCe126(qint8 data);
	QSound *bells;
	
	bool	ToDestroy;
		
    void	settop(int value){ top = value; }
    void	setposX(int value) { posX = value; }
int ce126_Mode;
					
    Cce126(CPObject *parent = 0);
	
	~Cce126()
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
    bool	Previous_BUSY;
    bool	Previous_MT_OUT1;
	QTime	time;
	bool	ctrl_char;
	BYTE	t,c;
	qint64	run_oldstate;
    int     code_transfer_step;
    int     device_code;
    qint64  lastState;
    int     internal_device_code;

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



class Cce123:public Cce126
{
public:
	Cce123()
	{								//[constructor]
		BackGroundFname	= ":/EXT/ext/ce-123p.jpg";
        setcfgfname("ce123p");
        setDX(708);//Pc_DX	= 708;
        setDY(566);//Pc_DY	= 566;
		SnapPts = QPoint(215,307);
        pCONNECTOR->setSnap(QPoint(215,397));
        setPaperPos(QRect(89,185-149,207,149));

        delete pKEYB; pKEYB=new Ckeyb(this,"ce123.map");
	};
};

class Cce129:public Cce126
{
public:
	Cce129()
	{								//[constructor]
		BackGroundFname	= ":/EXT/ext/ce-129p.jpg";
        setcfgfname("ce129p");
        setDX(708);//Pc_DX	= 708;
        setDY(566);//Pc_DY	= 566;
        SnapPts = QPoint(88,288);
        pCONNECTOR->setSnap(QPoint(88,378));
        setPaperPos(QRect(89,185-149,207,149));
        delete pKEYB; pKEYB=new Ckeyb(this,"ce129.map");

    }
};

class C263591:public Cce126
{
public:

    C263591()
    {								//[constructor]
        BackGroundFname	= ":/EXT/ext/26-3591.jpg";
        setcfgfname("263591");
        setDX(854);//Pc_DX	= 854;
        setDY(349);//Pc_DY	= 349;
        SnapPts = QPoint(373,0);
        pCONNECTOR->setSnap(QPoint(373,90));
        setPaperPos(QRect(78,0,207,149));

        delete pKEYB; pKEYB=new Ckeyb(this,"263591.map");
    }

private:
};

#endif
