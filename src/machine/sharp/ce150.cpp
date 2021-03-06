//TODO Migrate sound to QMultimedia

#include <QPainter>
#include <QResource>

#include "common.h" 
#include "ce150.h"
#include "pc1500.h"
#include "paperwidget.h"
#include "Keyb.h"
#include "keybce150.h"
#include "clink.h"
#include "dialoganalog.h"

#define NO_MOVE	0
#define RI_MOVE	1
#define LE_MOVE 2
#define UP_MOVE 1
#define DN_MOVE 2


#define PA		(pLH5810->lh5810.r_opa)
#define PA0		((PA & 0x01) ? TRUE : FALSE )
#define PA1		((PA & 0x02) ? TRUE : FALSE )
#define PA2		((PA & 0x04) ? TRUE : FALSE )
#define PA3		((PA & 0x08) ? TRUE : FALSE )
#define PA4		((PA & 0x10) ? TRUE : FALSE )
#define PA5		((PA & 0x20) ? TRUE : FALSE )
#define PA6		((PA & 0x40) ? TRUE : FALSE )
#define PA7		((PA & 0x80) ? TRUE : FALSE )

#define PB		(pLH5810->lh5810.r_opb)
#define PB0		((PB & 0x01) ? TRUE : FALSE )
#define PB1		((PB & 0x02) ? TRUE : FALSE )
#define PB2		((PB & 0x04) ? TRUE : FALSE )
#define PB3		((PB & 0x08) ? TRUE : FALSE )
#define PB4		((PB & 0x10) ? TRUE : FALSE )
#define PB5		((PB & 0x20) ? TRUE : FALSE )
#define PB6		((PB & 0x40) ? TRUE : FALSE )
#define PB7		((PB & 0x80) ? TRUE : FALSE )

#define PC		(pLH5810->lh5810.r_opc)
#define PC0		((PC & 0x01) ? TRUE : FALSE )
#define PC1		((PC & 0x02) ? TRUE : FALSE )
#define PC2		((PC & 0x04) ? TRUE : FALSE )
#define PC3		((PC & 0x08) ? TRUE : FALSE )
#define PC4		((PC & 0x10) ? TRUE : FALSE )
#define PC5		((PC & 0x20) ? TRUE : FALSE )
#define PC6		((PC & 0x40) ? TRUE : FALSE )
#define PC7		((PC & 0x80) ? TRUE : FALSE )

#define PAb(x)	(((PA) >> (x)) & 0x01)
#define PBb(x)	(((PB) >> (x)) & 0x01)
#define PCb(x)	(((PC) >> (x)) & 0x01)

#define MACRO_ADD_LOG	AddLog(LOG_PRINTER,tr("X=%1 Y=%2 Rot=%3 Color=%4,   IF=%5").arg(Pen_X,Pen_Y,Rot,Pen_Color,pLH5810->lh5810.r_if))

Cce150::Cce150(CPObject *parent):Cprinter(this)
{
    //[constructor]
    BackGroundFname	= P_RES(":/ext/ce-150.png");
    PaperFname		= "ext\\ce-150paper.jpg";
    setcfgfname(QString("ce150"));
    Paper_X = 120;  Paper_DX = 382;//320;
    Paper_Y = 110;
    //PaperWidgetRect = QRect(80,46,167,170);

    setDXmm(329);//Pc_DX_mm = 329;
    setDYmm(115);//Pc_DY_mm = 115;
    setDZmm(48);//Pc_DZ_mm = 48;

    setDX(1146);//Pc_DX	= 960;
    setDY(382);//Pc_DY	= 320;

    pTIMER		= new Ctimer(this);
    pLH5810		= new CLH5810(this);
    KeyMap		= KeyMapce150;
    KeyMapLenght= KeyMapce150Lenght;
    pKEYB		= new Ckeyb(this,"ce150.map");

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
    setPaperPos(QRect(0,0,0,0));


    StartRot = false;
    Change_Color = true;
}

Cce150::~Cce150() {
    delete ce150buf;
    delete ce150display;
    delete ce150pen;
    delete pCONNECTOR;
    delete pLH5810;
}

bool Cce150::init(void)
{
    CPObject::init();

    setfrequency( 0);
    pCONNECTOR	= new Cconnector(this,60,0,Cconnector::Sharp_60,"Connector 60 pins",true,QPoint(465,72));	publish(pCONNECTOR);

    WatchPoint.add(&pCONNECTOR_value,64,60,this,"Standard 60pins connector");
    WatchPoint.add((qint64 *) &(pLH5810->lh5810.r_opa),8,8,this,"LH5810 Port A");
    WatchPoint.add((qint64 *) &(pLH5810->lh5810.r_opb),8,8,this,"LH5810 Port B");
    WatchPoint.add((qint64 *) &(pLH5810->lh5810.r_opc),8,8,this,"LH5810 Port C");

    AddLog(LOG_PRINTER,tr("PRT initializing..."));

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();

    // Create CE-150 Paper Image
    ce150buf	= new QImage(QSize(320, 3000),QImage::Format_ARGB32);
    ce150display= new QImage(QSize(320, 567),QImage::Format_ARGB32);
    ce150pen	= new QImage(":/ext/ce-150pen.png");
    // Fill it blank
    clearPaper();

    // Create a paper widget
    paperWidget = new CpaperWidget(QRect(95,25,200,202),ce150buf,this);
    paperWidget->show();

    return(1);

}

INLINE bool Cce150::lh5810_write(void)
{
#if 0 // TRY TO OPTIMIZE ...
    SETREG_LH5810_OPC(pLH5810,pTIMER->pPC->Get_8(0x1B008))
    SETREG_LH5810_G(pLH5810  ,pTIMER->pPC->Get_8(0x1B009))
    SETREG_LH5810_MSK(pLH5810,pTIMER->pPC->Get_8(0x1B00A))
    SETREG_LH5810_IF(pLH5810 ,pTIMER->pPC->Get_8(0x1B00B))
    SETREG_LH5810_DDA(pLH5810,pTIMER->pPC->Get_8(0x1B00C))
    SETREG_LH5810_DDB(pLH5810,pTIMER->pPC->Get_8(0x1B00D))
    SETREG_LH5810_OPA(pLH5810,pTIMER->pPC->Get_8(0x1B00E))
    SETREG_LH5810_OPB(pLH5810,pTIMER->pPC->Get_8(0x1B00F))
#else	
    pLH5810->SetReg(CLH5810::OPC,pTIMER->pPC->Get_8(0x1B008));
    pLH5810->SetReg(CLH5810::G  ,pTIMER->pPC->Get_8(0x1B009));
    pLH5810->SetReg(CLH5810::MSK,pTIMER->pPC->Get_8(0x1B00A));
    pLH5810->SetReg(CLH5810::IF ,pTIMER->pPC->Get_8(0x1B00B));
    pLH5810->SetReg(CLH5810::DDA,pTIMER->pPC->Get_8(0x1B00C));
    pLH5810->SetReg(CLH5810::DDB,pTIMER->pPC->Get_8(0x1B00D));
    pLH5810->SetReg(CLH5810::OPA,pTIMER->pPC->Get_8(0x1B00E));
    pLH5810->SetReg(CLH5810::OPB,pTIMER->pPC->Get_8(0x1B00F));
#endif
	return(1);
}
INLINE bool Cce150::lh5810_read(void)
{
    pTIMER->pPC->Set_8(0x1B008 , pLH5810->GetReg(CLH5810::OPC));
    pTIMER->pPC->Set_8(0x1B009 , pLH5810->GetReg(CLH5810::G));
    pTIMER->pPC->Set_8(0x1B00A , pLH5810->GetReg(CLH5810::MSK));
    pTIMER->pPC->Set_8(0x1B00B , pLH5810->GetReg(CLH5810::IF));
    pTIMER->pPC->Set_8(0x1B00C , pLH5810->GetReg(CLH5810::DDA));
    pTIMER->pPC->Set_8(0x1B00D , pLH5810->GetReg(CLH5810::DDB));
    pTIMER->pPC->Set_8(0x1B00E , pLH5810->GetReg(CLH5810::CLH5810::OPA));
    pTIMER->pPC->Set_8(0x1B00F , pLH5810->GetReg(CLH5810::OPB));

	return(1);
}




/*
PB0		Pen ascending signal
PB1		Pen descending signal
PB2		Color detection
PB3		Not used
PB4		Not used
PB5		Not used
PB6		Low battery
PB7		Paper feed key input
*/
bool Cce150::run(void)
{
	bool has_moved = false;
	lh5810_write();
	
	////////////////////////////////////////////////////////////////////
	//	VOLTAGE OK :-)
	//////////////////////////////////////////////////////////////////
    pLH5810->SetRegBit(CLH5810::OPB,6,FALSE);

	////////////////////////////////////////////////////////////////////
	//	PRINT MODE
	//////////////////////////////////////////////////////////////////
	if (pKEYB->LastKey==K_PRINT)
	{
		Print_Mode = ! Print_Mode;
        pLH5810->SetRegBit(CLH5810::OPA,5,Print_Mode);
        pKEYB->LastKey = 0;
	}
	
	////////////////////////////////////////////////////////////////////
	//	PAPER FEED
	//////////////////////////////////////////////////////////////////
	if (pKEYB->LastKey==K_PFEED)
	{
        pLH5810->SetRegBit(CLH5810::OPB,7,true);
		AddLog(LOG_MASTER,"Paper Feed");
	}
	else
        pLH5810->SetRegBit(CLH5810::OPB,7,false);

	////////////////////////////////////////////////////////////////////
	//	RMT ON/OFF
	////////////////////////////////////////////////////////////////////
	if (pLH5810->lh5810.r_opa & 0x02)	((Cpc15XX *)pPC->pTIMER->pPC)->pce152->paused = FALSE;	// RMT 0 ON
	if (pLH5810->lh5810.r_opa & 0x04)	((Cpc15XX *)pPC->pTIMER->pPC)->pce152->paused = TRUE;	// RMT 0 OFF


 
	Direction = Motor_X.SendPhase(PC & 0x0f);
 
	switch (Direction)
	{
		case RI_MOVE:	Pen_X++;
						has_moved=TRUE;
                        //MACRO_ADD_LOG;
						break;
		case LE_MOVE:	Pen_X--;
						has_moved=TRUE;
//						MACRO_ADD_LOG;
						if (Pen_X == -16)	StartRot = TRUE;	// rotation pin engaged
						if (Pen_X < -45)	Pen_X = -45;		// physical stop at x==-45
						if ((Pen_X == -45) && StartRot)
						{
							// 1/12 rotation more
							Rot++;
							StartRot = FALSE;
							// If the third 1/12 rotation, then put next color
							if (Rot == 3) {	Rot = 0; Next_Color(); }
						}
						break;
	}

	Direction = Motor_Y.SendPhase((PC >> 4) & 0x0f);

	switch (Direction)
	{
		case UP_MOVE:	Pen_Y--;
						has_moved=TRUE;
						MACRO_ADD_LOG;
						break;
		case DN_MOVE:	Pen_Y++;
						has_moved=TRUE;
						MACRO_ADD_LOG;
						break;
	}

	if (has_moved)
	{
		if	( (Pen_Color==0) && (Rot == 0) && (Pen_X <= -45) )
		{
            pLH5810->SetRegBit(CLH5810::OPB,2,TRUE);	// COLOR MAGNET
			AddLog(LOG_PRINTER,"Color Magnet");
		}
        else
            pLH5810->SetRegBit(CLH5810::OPB,2,FALSE);	// NO COLOR MAGNET
	}

	if (PB0) 
	{
 		if (Pen_Status==PEN_DOWN) 
		{
			has_moved=TRUE;
			Pen_Status = PEN_UP;
//			AddLog(LOG_PRINTER,"PEN UP");

//            fillSoundBuffer(0xFF);
//            fillSoundBuffer(0x00);

		}
	}
	if (PB1)
	{
		if (Pen_Status==PEN_UP) 
		{

			has_moved=TRUE;
			Pen_Status = PEN_DOWN;
//			AddLog(LOG_PRINTER,"PEN DOWN");
		}
	}

	pLH5810->step();
	
	lh5810_read();

	//---------------------------------------------------
	// Draw printer
	//---------------------------------------------------
	if (has_moved) Print();


	pCONNECTOR->Set_pin(1	,1);
	pCONNECTOR->Set_pin(30	,pLH5810->INT);

	return(1);
}

bool Cce150::Next_Color(void)
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


void Cce150::clearPaper(void)
{
	// Fill it blank
	ce150buf->fill(PaperColor.rgba());
	ce150display->fill(QColor(0,0,0,0).rgba());
    Pen_Y = 100;
}

bool Cce150::exit(void)
{

	CPObject::exit();
	
    return true;
}



void Cce150::Print(void)
{	
	QPainter painter;

    //pPC->Refresh_Display = true;
	
	if (Pen_Status==PEN_DOWN)
	{
		painter.begin(ce150buf);
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

void Cce150::SaveAsText(void)
{
	QMessageBox::warning(mainwindow, "PockEmul",
                         tr("This printer is a pure graphic printer\n") +
                         tr("Saving output as text is irrelevant") );
}

bool CLH5810_CE150::step()
{

    CLH5810::step();

	return(1);
}
