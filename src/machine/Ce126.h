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


#include "cprinter.h"
#include "Keyb.h"
#include "Connect.h"
#include "Inter.h"

#define GET_PIN(n)		pCONNECTOR->Get_pin(n)
#define SET_PIN(n,v)	pCONNECTOR->Set_pin(n,v)

class Cce126:public Cprinter{
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
	
	QImage *ce126buf;
	QImage *ce126display;
	
	void RefreshCe126(qint8 data);
	QSound *bells;
	
	bool	ToDestroy;
		
	void	settop(int value){ top = value; };
	void	setposX(int value) { posX = value; };
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
	bool	Previous_PIN_BUSY;
	bool	Previous_PIN_MT_OUT1;
	QTime	time;
	bool	ctrl_char;
	BYTE	t,c;
	qint64	run_oldstate;


};

class Cce120p:public Cce126
{
public:
    Cce120p()
    {								//[constructor]
        BackGroundFname	= ":/EXT/ext/ce-120p.png";
        setcfgfname("ce120p");
        Pc_DX	= 883;
        Pc_DY	= 568;
        SnapPts = QPoint(402,43);
        pCONNECTOR->setSnap(QPoint(400,131));
        setPaperPos(QRect(89,185-149,207,149));

        stackBehind = true;
        delete pKEYB; pKEYB=new Ckeyb(this,"ce120.map");
    }
};

class Cce123:public Cce126
{
public:
	Cce123()
	{								//[constructor]
		BackGroundFname	= ":/EXT/ext/ce-123p.jpg";
        setcfgfname("ce123p");
		Pc_DX	= 708;
		Pc_DY	= 566;
		SnapPts = QPoint(215,307);
        pCONNECTOR->setSnap(QPoint(215,397));
        setPaperPos(QRect(89,185-149,207,149));

		stackBehind = true;
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
		Pc_DX	= 708;
		Pc_DY	= 566;
        SnapPts = QPoint(88,288);
        pCONNECTOR->setSnap(QPoint(88,378));
        setPaperPos(QRect(89,185-149,207,149));
        delete pKEYB; pKEYB=new Ckeyb(this,"ce129.map");

		stackBehind = true;
	};
};

class C263591:public Cce126
{
public:

    C263591()
    {								//[constructor]
        BackGroundFname	= ":/EXT/ext/26-3591.jpg";
        setcfgfname("263591");
        Pc_DX	= 854;
        Pc_DY	= 349;
        SnapPts = QPoint(373,0);
        pCONNECTOR->setSnap(QPoint(373,90));
        setPaperPos(QRect(78,0,207,149));

        stackBehind = true;
        delete pKEYB; pKEYB=new Ckeyb(this,"263591.map");
    };

private:
};

#endif
