#include <QtGui> 
#include <QString>
#include <QPainter>
 
#ifndef NO_SOUND
#include "fmod.h"
#endif

#include "mainwindowpockemul.h"

#include "dialoganalog.h"
#include "dialogabout.h"
#include "dialogstartup.h"
#include "dialoglog.h"
#include "hexviewer.h"
#include "autoupdater.h"

#include "init.h"
#include "common.h"
#include "Log.h"
#include "pcxxxx.h"
#include "pc1500.h"
#include "clink.h"

//#include "lfhex/hexGui.h"

extern	MainWindowPockemul* mainwindow;

#define NBFRAMEPERSEC		20
#define FRAMERATE			(1000/NBFRAMEPERSEC)
#define TIMER_RES			5

QTime t,tf;
QTimer *timer;
QList<CPObject *> listpPObject; 
 

MainWindowPockemul::MainWindowPockemul( QWidget * parent, Qt::WFlags f) : QMainWindow(parent, f)
{
	rawclk = 0;
	setupUi(this);
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	dialoglog = 0;
	dialoganalogic = 0;
	zoom = 100;

	startKeyDrag = false;
	startPosDrag = false;

    connect(actionAbout_PockEmul,	SIGNAL(triggered()), this, SLOT(about()));
    connect(actionNew,				SIGNAL(triggered()), this, SLOT(newsession()));
    connect(actionOpen,				SIGNAL(triggered()), this, SLOT(opensession()));
    connect(actionLog_Messages,		SIGNAL(triggered()), this, SLOT(Log()));
 	connect(actionAnalogic_Port,	SIGNAL(triggered()), this, SLOT(Analogic()));    
 	connect(actionCheck_for_Updates,SIGNAL(triggered()), this, SLOT(CheckUpdates()));    

	pdirectLink = new CDirectLink;
 
#ifndef NO_SOUND
	FSOUND_Init(44100, 32, 0); 
#endif

//audio.show();


	// Create a timer for Drawing screen FRAMERATE times per seconds
	FrameTimer = new QTimer(mainwindow);
    connect(FrameTimer, SIGNAL(timeout()), this, SLOT(updateFrameTimer()));
	FrameTimer->start(FRAMERATE);
	
	timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
	timer->start(TIMER_RES);

	// Create the Pocket Thread
	PcThread = new CPocketThread(0);
	PcThread->connect(PcThread,SIGNAL(Resize(QSize,CPObject * )),this,SLOT(resizeSlot(QSize,CPObject * )));
	PcThread->connect(PcThread,SIGNAL(Destroy(CPObject * )),this,SLOT(DestroySlot(CPObject * )));
	PcThread->start();
	

}
//
void MainWindowPockemul::SendSignal_AddLogItem(QString str) { emit AddLogItem(str); }

void MainWindowPockemul::slotUnLink(QAction * action)
{
	int i;
	Cconnector * p1 = (Cconnector*) action->data().toString().toULongLong();
	for ( i = 0 ; i< pdirectLink->AConnList.size() ; i++)
	{
		if (pdirectLink->AConnList.at(i) == p1)
		{
			pdirectLink->AConnList.removeAt(i);
			pdirectLink->BConnList.removeAt(i);
			i--;
		}
	}
	for ( i = 0 ; i< pdirectLink->BConnList.size() ; i++)
	{
		if (pdirectLink->BConnList.at(i) == p1)
		{
			pdirectLink->AConnList.removeAt(i);
			pdirectLink->BConnList.removeAt(i);
			i--;
		}
	}
}

void MainWindowPockemul::slotNewLink(QAction * action)
{
	QString s = action->data().toString();
	QStringList list = s.split(":");
	
	qint64 l1 = list.at(0).toULongLong();
	qint64 l2 = list.at(1).toULongLong();
	
	Cconnector * p1 = (Cconnector*)l1;
	Cconnector * p2 = (Cconnector*)l2;
	
	pdirectLink->AConnList.append(p1);
	pdirectLink->BConnList.append(p2);
	
	AddLog(LOG_MASTER,tr("DirectLink Iinsert (%1,%2)").arg((qint64)p1).arg((qint64)p2));
}


int MainWindowPockemul::newsession()
{
    DialogStartup dialogstartup(this);
    int result = dialogstartup.exec();
	if (result)	{
		CPObject *newpPC = InitApp(result);
 		if (! newpPC) MSG_ERROR("pPC is NULL in slotStart")
 		else
 		{
 			listpPObject.append(newpPC);
		}

	}
	else
		QMessageBox::about(this, tr("Attention"),"Please choose a pocket model or Cancel");

	return 1;
}

void MainWindowPockemul::about()
{
	DialogAbout dialogabout(this);
	dialogabout.exec();
}

void MainWindowPockemul::Log()
{
        dialoglog = new DialogLog(this);
        dialoglog->show();
}


void MainWindowPockemul::Analogic()
{
        dialoganalogic = new dialogAnalog(11,this);
        dialoganalogic->show();
}

void MainWindowPockemul::CheckUpdates()
{
	CAutoUpdater dialogcheckupdate(this);
	dialogcheckupdate.exec();
}
    
void MainWindowPockemul::opensession()
{
}

void MainWindowPockemul::paintEvent(QPaintEvent *event) {}

void MainWindowPockemul::updateTimer()
{
	static int deltaTime = -1;
	
	if (deltaTime == -1) {	t.start(); }
	deltaTime = t.restart();	
	
	rawclk += deltaTime;

}

void  MainWindowPockemul::updateTimeTimer()
{
}

void  MainWindowPockemul::updateFrameTimer()
{
	static long last_state = 0;
	int statepersec;
	static long nb_state=0;
	int rate=0;
	static int OneSecTimer=0;
	long Current_State;

// Calculate emulation speed
// Normally each frame equal pPC->frequency state / NBFRAMEPERSEC


	if (! listpPObject.size()) return;
		
	static int deltaTime = -1;
	
	if (deltaTime == -1) {	tf.start(); }
	
	deltaTime = tf.elapsed();
	if (deltaTime >= 1000) tf.restart();
		
 	for (int i = 0;i < listpPObject.size(); i++)
 	{
 		CPObject* CurrentpPC = listpPObject.at(i);
		
		if (CurrentpPC->getfrequency())
		{
			Current_State = CurrentpPC->pTIMER->state;
		
			CurrentpPC->pTIMER->nb_state += (Current_State - CurrentpPC->pTIMER->last_state);
			CurrentpPC->pTIMER->last_state = Current_State;
		
			// Update ToolTip only one time per second
			if ( deltaTime >= 1000)
			{
//	AddLog(LOG_TIME,tr("Time Frame elapsed : %1 ms  nb=%2 cur=%3 last=%4").arg(deltaTime).arg(CurrentpPC->pTIMER->nb_state).arg(Current_State).arg(CurrentpPC->pTIMER->last_state));
				statepersec = (int) ( CurrentpPC->getfrequency());
                rate = (int) ((100L*CurrentpPC->pTIMER->nb_state)/((statepersec/1000)*deltaTime));
				CurrentpPC->pTIMER->nb_state=0;
				
				QString str;
				str.setNum((int)rate);
				CurrentpPC->setToolTip(str+tr("% original speed"));
			}
		}
		if (CurrentpPC->pLCDC) 
		{
			CurrentpPC->pLCDC->disp();
			if (CurrentpPC->pLCDC->Refresh) CurrentpPC->Refresh_Display = true;
		}
		if ( CurrentpPC->Refresh_Display) 
		{
			CurrentpPC->update();
			CurrentpPC->Refresh_Display= false;
		}
	}
	
	if (OneSecTimer >= 1000) OneSecTimer=0;
}

void MainWindowPockemul::mousePressEvent	( QMouseEvent *event){}
void MainWindowPockemul::mouseMoveEvent		( QMouseEvent * event ){}
void MainWindowPockemul::mouseReleaseEvent	( QMouseEvent *event){}
void MainWindowPockemul::keyReleaseEvent	( QKeyEvent * event ){}
void MainWindowPockemul::keyPressEvent		( QKeyEvent * event ){}
void MainWindowPockemul::resizeEvent		( QResizeEvent * event ){}

void MainWindowPockemul::resizeSlot( QSize size , CPObject *pObject)
{
	delete(pObject->FinalImage);
	
	int l = menuBar()->height();
	pObject->FinalImage = new QImage(size-QSize(0,l),QImage::Format_RGB32);
}

void MainWindowPockemul::DestroySlot( CPObject *pObject)
{
	pObject->exit();
	pObject->deleteLater();
}


void MainWindowPockemul::closeEvent(QCloseEvent *event)
{

}
