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


extern TransMap KeyMapce150[];
extern int KeyMapce150Lenght;

enum eMODE {GRAPH,TEXT};

class CMove
{
public:
    int		X;
    int     Y;
    bool    penDown;
    bool    changeColor;
    int     oldColor;
    int     newColor;
    CMove(int x,int y,bool pendown) {
        this->X = x;
        this->Y = y;
        this->penDown = pendown;
        changeColor = false;
    }
    CMove(int oldcolor,int newcolor) {
        this->oldColor = oldcolor;
        this->newColor = newcolor;
        this->changeColor = true;
    }

};

class Cce515p:public Cprinter{
    Q_OBJECT
public:

    virtual bool		run(void);
    void		Print(CMove);
    virtual bool		init(void);
    virtual bool		exit(void);

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
    void    set_SD(quint8);
    void    drawChar(quint8 data);
    void    DrawMove(int lenght,int dir,bool penDown);

    void    ProcessEscCommand(void);
    void    ProcessGraphCommand(void);
    void    ProcessMultiPointCommand(QString);
    void    DrawLine(int xa, int ya, int xb, int yb);
    void    DrawTest(void);
    void    PaperFeed(void);

    Cce515p(CPObject *parent = 0):Cprinter(parent)
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
        Pen_Y = 100;
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
        setPaperPos(QRect(75,46,380,170));

#ifndef NO_SOUND
        clac = NULL;
#endif
        StartRot = false;
        Change_Color = true;
        Sii_wait			= 0;
    }
public:

protected slots:
    void    Command(quint8);

protected:
    bool		Next_Color(void);
    int			Pen_X,Pen_Y,Pen_Z,prev_Pen_X,prev_Pen_Y,prev_Pen_Z;
    bool		Pen_Status;			// UP or Down Position
    int			Pen_Color;			// Color : 0 to 3
    DWORD		Offset_Paper_View;
    quint8		Rot;
    quint8  	t,c;

    bool		Change_Color;
#ifndef NO_SOUND
    FSOUND_SAMPLE *clac;
#endif
    bool	StartRot;
    quint8	Direction;
    quint8    SD;
    int		Sii_wait;
    static const QString graph[];

private:
    QList<CMove> moveBuffer;
    int charSize;
    QString escCommand;
    QString graphCommand;
    bool    escMode;

    int     lenght,dir;
    bool    penDown;

    eMODE   mode;

    int orig_X;
    int orig_Y;
    int lineType;
    int mainRot;


};





#endif // CE515P_H
