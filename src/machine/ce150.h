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
#include "cmotor.h"



#define PEN_UP	0
#define PEN_DOWN 1

class Cprinter;

extern void DoCe150Dialog(void);
extern TransMap KeyMapce150[];
extern int KeyMapce150Lenght; 


class CLH5810_CE150:public CLH5810{
public:
	bool	step(void);
    const char*	GetClassName(){ return("CLH5810_CE150");};

	CLH5810_CE150(CPObject *parent)	: CLH5810(parent){};
	~CLH5810_CE150(){};

private:
};



class Cce150:public Cprinter{						
public:

	bool		run(void);
	void		Print(void);
	bool		init(void);
	bool		exit(void);

    const char		*PaperFname;
	int			Paper_X,Paper_Y,Paper_DX,Paper_DY;

	bool		Print_Mode;
	bool		needRedraw;
	Cconnector	*pCONNECTOR;	qint64 pCONNECTOR_value;

    CLH5810		*pLH5810;

	bool lh5810_write(void);
	bool lh5810_read(void);
	
	QImage *ce150buf;
	QImage *ce150display;
	QImage *ce150pen;

	void	clearPaper(void);
	void	SaveAsText(void);

    Cmotor		Motor_X;
    Cmotor		Motor_Y;


    Cce150(CPObject *parent = 0):Cprinter(this)
	{				
		//[constructor]
		BackGroundFname	= ":/EXT/ext/ce-150.jpg";
		PaperFname		= "ext\\ce-150paper.jpg";
		setcfgfname(QString("ce150"));
        Paper_X = 100;  Paper_DX = 320;
		Paper_Y = 100;
        //PaperWidgetRect = QRect(80,46,167,170);
		Pc_DX	= 960;
		Pc_DY	= 320;
		SnapPts = QPoint(388,0);
        pCONNECTOR	= new Cconnector(this,60,"Connector 60 pins",true);	publish(pCONNECTOR);
        pTIMER		= new Ctimer(this);
        pLH5810		= new CLH5810(this);
		KeyMap		= KeyMapce150;
		KeyMapLenght= KeyMapce150Lenght;
		pKEYB		= new Ckeyb_ce150(this);

		Print_Mode = 0;

		Pen_X = 0;
		Pen_Y = 000;
        Pen_Z = 0;
		prev_Pen_X = 0;
		prev_Pen_Y = 0;	
        prev_Pen_Z = 0;
		Pen_Status = PEN_UP;
        Pen_Color = 0;
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
public:



protected:
    bool		Next_Color(void);
    int			Pen_X,Pen_Y,Pen_Z,prev_Pen_X,prev_Pen_Y,prev_Pen_Z;
	bool		Pen_Status;			// UP or Down Position
	int			Pen_Color;			// Color : 0 to 3
	DWORD		Offset_Paper_View;
	BYTE		Rot;


	bool		Change_Color;
#ifndef NO_SOUND
	FSOUND_SAMPLE *clac;
#endif
	bool	StartRot;
	BYTE	Direction;

};

#endif
