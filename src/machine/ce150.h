#ifndef _CE150_H_
#define _CE150_H_

#ifndef NO_SOUND
#include "fmod.h"
#endif

#include "Log.h"
#include "lh5810.h"
#include "cprinter.h"
#include "Connect.h"
#include "Inter.h"
#include "Keyb.h"


#define NO_MOVE	0
#define RI_MOVE	1
#define LE_MOVE 2
#define PEN_UP	0
#define PEN_DOWN 1

class Cprinter;

extern void DoCe150Dialog(void);
extern TransMap KeyMapce150[];
extern int KeyMapce150Lenght; 


class CLH5810_CE150:public CLH5810{
public:
	bool	step(void);
	char*	GetClassName(){ return("CLH5810_CE150");};

	CLH5810_CE150(CPObject *parent)	: CLH5810(parent){};
	~CLH5810_CE150(){};

private:
};

class Cmotor:public QObject{
public:
	int		SendPhase(int Phase);		// Next step, return direction: 0 no move, 1 Right,2 left
	long	Step;
	int		CurrentPhase;
	int		LastStepDirection;		// TRUE= Right   FALSE=Left
	Cmotor(){
		LastStepDirection = NO_MOVE;
		Step = 0;
		CurrentPhase = 0x09;
	}

private:
};

class Cce150:public Cprinter{						
public:

	bool		run(void);
	void		Print(void);
	bool		init(void);
	bool		exit(void);

	char		*PaperFname;
	int			Paper_X,Paper_Y,Paper_DX,Paper_DY;

	bool		Print_Mode;
	bool		needRedraw;
	Cconnector	*pCONNECTOR;	qint64 pCONNECTOR_value;

	CLH5810_CE150		*pLH5810;

	bool lh5810_write(void);
	bool lh5810_read(void);
	
	QImage *ce150buf;
	QImage *ce150display;
	QImage *ce150pen;

	void	clearPaper(void);
	void	SaveAsText(void);



	Cce150():Cprinter(this)
	{				
		//[constructor]
		BackGroundFname	= ":/EXT/ext/ce-150.jpg";
		PaperFname		= "ext\\ce-150paper.jpg";
		setcfgfname(QString("ce150"));
		Paper_X = 100;
		Paper_Y = 100;
		Pc_DX	= 960;
		Pc_DY	= 320;
		SnapPts = QPoint(388,0);
		pCONNECTOR	= new Cconnector60(this,true);	publish(pCONNECTOR);
		pTIMER		= new Ctimer(this);
		pLH5810		= new CLH5810_CE150(this);
		KeyMap		= KeyMapce150;
		KeyMapLenght= KeyMapce150Lenght;
		pKEYB		= new Ckeyb_ce150(this);

		Print_Mode = 0;

		Pen_X = 0;
		Pen_Y = 000;
		prev_Pen_X = 0;
		prev_Pen_Y = 0;		
		Pen_Status = PEN_UP;
		Pen_Color = 3;
		Rot = 0;
//960,320,388,0)		
		ce150buf=0;
		ce150display=0;
		needRedraw = true;
		stackBehind = true;
		setPaperPos(QPoint(0,0));
		
#ifndef NO_SOUND
		clac = NULL;
#endif
		StartRot = false;
		Change_Color = true;
	}

private:
	Cmotor		Motor_X;
	Cmotor		Motor_Y;

	int			Pen_X,Pen_Y,prev_Pen_X,prev_Pen_Y;
	bool		Pen_Status;			// UP or Down Position
	int			Pen_Color;			// Color : 0 to 3
	DWORD		Offset_Paper_View;
	BYTE		Rot;

	bool		Next_Color(void);
	bool		Change_Color;
#ifndef NO_SOUND
	FSOUND_SAMPLE *clac;
#endif
	bool	StartRot;
	BYTE	Direction;

};

#endif
