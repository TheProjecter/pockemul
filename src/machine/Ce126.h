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
					
	Cce126():Cprinter(this)
	{								//[constructor]
	setfrequency( 0);
		ce126buf	= 0;
		ce126display= 0;
		bells		= 0;
		charTable = 0;
		ToDestroy	= false;
		BackGroundFname	= ":/EXT/ext/ce-126p.png";
		
		settop(10);
		setposX(0);
		pCONNECTOR	= new Cconnector11(this,true);	publish(pCONNECTOR);
		pTAPECONNECTOR	= new Cconnector(this,2,"Line in / Rec",false);	publish(pTAPECONNECTOR);
		pTIMER		= new Ctimer(this);
		Pc_DX	= 698;
		Pc_DY	= 488;
		SnapPts = QPoint(672,150);
		setPaperPos(QPoint(150,-3));
		stackBehind = true;

		ctrl_char = false;
		t = 0;
		c = 0;
	};
	
	~Cce126()
	{
	}

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



class Cce123:public Cce126
{
public:
	Cce123()
	{								//[constructor]
		BackGroundFname	= ":/EXT/ext/ce-123p.jpg";
		Pc_DX	= 708;
		Pc_DY	= 566;
		SnapPts = QPoint(215,307);
		setPaperPos(QPoint(89,185-149));

		stackBehind = true;
	};
};

class Cce129:public Cce126
{
public:
	Cce129()
	{								//[constructor]
		BackGroundFname	= ":/EXT/ext/ce-129p.jpg";
		Pc_DX	= 708;
		Pc_DY	= 566;
		SnapPts = QPoint(90,291);
		setPaperPos(QPoint(89,185-149));

		stackBehind = true;
	};
};
#endif
