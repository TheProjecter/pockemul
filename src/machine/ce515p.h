#ifndef CE515P_H
#define CE515P_H


#ifndef NO_SOUND
#include "fmod.h"
#endif

#include "Log.h"

#include "cprinter.h"
#include "pcxxxx.h"
#include "Connect.h"
#include "Inter.h"
#include "Keyb.h"



#define PEN_UP	0
#define PEN_DOWN 1

class Cprinter;

extern TransMap KeyMapce150[];
extern int KeyMapce150Lenght;



class Cce515p:public Cprinter{
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

    QImage *ce515pbuf;
    QImage *ce515pdisplay;
    QImage *ce515ppen;

    void	clearPaper(void);
    void	SaveAsText(void);
    void set_SD(BYTE val);

    Cce515p(CPObject *parent = 0):Cprinter(this)
    {
        //[constructor]
        BackGroundFname	= ":/EXT/ext/ce-150.jpg";
        PaperFname		= "ext\\ce-150paper.jpg";
        setcfgfname(QString("ce515p"));
        Paper_X = 100;  Paper_DX = 320;
        Paper_Y = 100;
        //PaperWidgetRect = QRect(80,46,167,170);
        Pc_DX	= 960;
        Pc_DY	= 320;
        SnapPts = QPoint(388,0);
        pCONNECTOR	= new Cconnector(this,5,"Internal connector 5 pins",true);	publish(pCONNECTOR);
        pTIMER		= new Ctimer(this);
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
        ce515pbuf=0;
        ce515pdisplay=0;
        needRedraw = true;
        stackBehind = true;
        setPaperPos(QPoint(0,0));

#ifndef NO_SOUND
        clac = NULL;
#endif
        StartRot = false;
        Change_Color = true;
        Sii_wait			= 0;
    }
public:



protected:
    bool		Next_Color(void);
    int			Pen_X,Pen_Y,Pen_Z,prev_Pen_X,prev_Pen_Y,prev_Pen_Z;
    bool		Pen_Status;			// UP or Down Position
    int			Pen_Color;			// Color : 0 to 3
    DWORD		Offset_Paper_View;
    BYTE		Rot;
    BYTE	t,c;

    bool		Change_Color;
#ifndef NO_SOUND
    FSOUND_SAMPLE *clac;
#endif
    bool	StartRot;
    BYTE	Direction;
    BYTE    SD;
    int		Sii_wait;

};



#endif // CE515P_H
