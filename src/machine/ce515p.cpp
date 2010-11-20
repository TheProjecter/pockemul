#include <QPainter>
#include "common.h"
#include "ce515p.h"
#include "paperwidget.h"
#include "Keyb.h"
#include "clink.h"
#include "dialoganalog.h"

#define NO_MOVE	0
#define RI_MOVE	1
#define LE_MOVE 2
#define UP_MOVE 1
#define DN_MOVE 2


#define GET_PIN(n)		pCONNECTOR->Get_pin(n)
#define SET_PIN(n,v)	pCONNECTOR->Set_pin(n,v)

#define DOWN	0
#define UP		1

#define TICKS_BDS	(pTIMER->pPC->getfrequency()/1200)
void Cce515p::set_SD(BYTE val) {
    SD = val;
}

bool Cce515p::run(void)
{
    bool bit = false;
    bool has_moved = false;


    static unsigned char	t=0,c=0,waitbitstart=1,waitbitstop=0;
    int deltastate=0;
    static long oldstate	= pTIMER->state;


    //	0	START BIT
    //	0-1	b0 0=set,1=not set
    //	0-1	b1 0=set,1=not set
    //	0-1	b2 0=set,1=not set
    //	0-1	b3 0=set,1=not set
    //	0-1	b4 0=set,1=not set
    //	0-1	b5 0=set,1=not set
    //	0-1	b6 0=set,1=not set
    //	0-1	b7 0=set,1=not set
    //	1	STOP BIT

        deltastate = pTIMER->state - oldstate;
        if (deltastate < Sii_wait){
            return(1);
        }

//        if (!(ER && RS))
//        {
//            oldstate	= pTIMER->state;
//            Sii_wait	= 0;
//            return;
//        }
    //	Sii_wait	= TICKS_BDS;
        oldstate	= pTIMER->state;
    //	oldstate	+= Sii_wait;


        if (waitbitstop && (SD==0))
        {
            waitbitstop = 0;waitbitstart=1;
    //		Bit STOP
            AddLog(LOG_PRINTER,tr("STOP BIT"));
            Sii_wait = 0;
        }
        else if (waitbitstart && SD)
        {
            waitbitstart = 0;
    //		Bit START
            AddLog(LOG_PRINTER,tr("START BIT"));
            Sii_wait	= TICKS_BDS;

        }
        else if (!waitbitstart)
        {
            t>>=1;
            if(SD==0) t|=0x80;
            AddLog(LOG_PRINTER,tr("Bit = %1").arg(SD));
            if((c=(++c)&7)==0)
            {
                AddLog(LOG_PRINTER,tr("Byte = %1").arg(t,2,16,QChar('0')));
                //ExportByte(t);
                t=0;
                waitbitstop = 1;
            }
        }


    //---------------------------------------------------
    // Draw printer
    //---------------------------------------------------
    if (has_moved) Print();


    //pCONNECTOR->Set_pin(1	,1);
    //pCONNECTOR->Set_pin(30	,pLH5810->INT);

    return(1);
}

bool Cce515p::Next_Color(void)
{
    Pen_Color++;
    Change_Color = true;
    if (Pen_Color == 4)
    {
        Pen_Color = 0;
        return(TRUE);
    }
    return(FALSE);
}


void Cce515p::clearPaper(void)
{
    // Fill it blank
    ce515pbuf->fill(PaperColor.rgba());
    ce515pdisplay->fill(QColor(0,0,0,0).rgba());
    Pen_Y = 0;
}

bool Cce515p::exit(void)
{
#ifndef NO_SOUND
    FSOUND_Sample_Free(clac);
#endif

    CPObject::exit();

    return true;
}

bool Cce515p::init(void)
{
    CPObject::init();

#ifndef NO_SOUND
    clac = FSOUND_Sample_Load(FSOUND_FREE, "clac2.wav", 0, 0, 0);
#endif

    setfrequency( 0);

    WatchPoint.add(&pCONNECTOR_value,8,5,this,"Internal connector");

    AddLog(LOG_PRINTER,tr("PRT initializing..."));

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();

    // Create CE-150 Paper Image
    ce515pbuf	= new QImage(QSize(320, 3000),QImage::Format_ARGB32);
    ce515pdisplay= new QImage(QSize(320, 567),QImage::Format_ARGB32);
    ce515ppen	= new QImage(":/EXT/ext/ce-150pen.png");
    // Fill it blank
    clearPaper();

    // Create a paper widget
    paperWidget = new CpaperWidget(QRect(80,46,167,170),ce515pbuf,this);
    paperWidget->show();

    Sii_wait = 0;

    return(1);

}

void Cce515p::Print(void)
{
    QPainter painter;

    pPC->Refresh_Display = true;

    if (Pen_Status==PEN_DOWN)
    {
        painter.begin(ce515pbuf);
        switch (Pen_Color)
        {
            case 0 : painter.setPen( Qt::black); break;
            case 1 : painter.setPen( Qt::blue); break;
            case 2 : painter.setPen( Qt::green); break;
            case 3 : painter.setPen( Qt::red); break;
        }
        painter.drawPoint( Pen_X+65, Pen_Y );
        painter.end();
    }

#if 0
    int x,y,w,h;
    RECT	destRect,srcRect;

    destRect.left	= 0;
    destRect.right	= 320;
    srcRect.left	= 0;
    srcRect.right	= 320;

    destRect.bottom = 567;
    destRect.top	= 0;

    srcRect.bottom	= Pen_Y+64;
    srcRect.top		= srcRect.bottom - 567;

    if (srcRect.top <0)
    {
        srcRect.top=0;
        destRect.top=destRect.bottom-(Pen_Y+64);
    }

    painter.begin(ce150display);
    painter.drawImage(QRectF(0,0,320,567),*ce150buf,QRectF(0,srcRect.top,320,567));	//

    x	= Pen_X+65-20;
    w	= 40;
    y	= 567 - 64;
    h	= 64;
    QRectF DestRect(x,y,w,h);

    x	= Rot * 40;
    w	= 40;
    y 	= 0;
    h	= 64;
    QRectF SrcRect(x,y,w,h);


    painter.drawImage(DestRect,*ce150pen,SrcRect);
    painter.end();
#endif
    pPC->Refresh_Display = true;

    paperWidget->setOffset(QPoint(65,Pen_Y));
}

void Cce515p::SaveAsText(void)
{
    QMessageBox::warning(mainwindow, "PockEmul",
                         tr("This printer is a pure graphic printer\n") +
                         tr("Saving output as text is irrelevant") );
}



