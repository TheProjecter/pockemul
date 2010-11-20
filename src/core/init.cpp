 /********************************************************************************************************
 * PROGRAM      : 
 * DATE - TIME  : samedi 28 octobre 2006 - 12h42
 * AUTHOR       :  (  )
 * FILENAME     : init.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include <string.h> 
#include <QtGui>
#include <QString>
#include <QPainter>

#include "init.h"
  
#include "common.h"

#include "dialoganalog.h"
 
#include "pcxxxx.h" 
#include "pc1245.h"
#include "pc1250.h"
#include "pc1251.h"
#include "pc1255.h"
#include "pc1260.h"

#include "pc1350.h"
#include "pc1360.h"

#include "pc2500.h"
 
#include "pc1401.h"
#include "pc1402.h"
#include "pc1403.h"
#include "pc1450.h"
#include "pc1475.h"

#include "pc1500.h"
#include "ce150.h"

#include "pc1600.h"
#include "ce1600p.h"

#include "ce152.h"
#include "sio.h"
#include "Ce126.h"
#include "ce125.h"
#include "clink.h"
#include "ccable.h"

#include "potar.h"



extern QList<CPObject *> listpPObject;  
extern QTimer *timer;

CWatchPoint WatchPoint;

bool	UpdateDisplayRunning;
CPocketThread* PcThread;
QList<CPocketThread*> listPcThread;	



CPObject * InitApp(int idPC )
{
CPObject *pPC=0;
	  
	switch (idPC) 
	{

		case 0	: return 0;
		case 10	: pPC = new Cpc1245;	pPC->setName("PC-1245");break;
		case 11	: pPC = new Cpc1250;	pPC->setName("PC-1250");break;
		case 12	: pPC = new Cpc1251;	pPC->setName("PC-1251");break;
		case 33	: pPC = new Cpc1251H;	pPC->setName("PC-1251H");break;
		case 13	: pPC = new Cpc1255;	pPC->setName("PC-1255");break;
		case 14	: pPC = new Ctrspc3;	pPC->setName("Tandy PC-3");break;
 
		case 15	: pPC = new Cpc1260;	pPC->setName("PC-1260");break;
		case 16	: pPC = new Cpc1261;	pPC->setName("PC-1261");break;
		case 17	: pPC = new Cpc1262;	pPC->setName("PC-1262");break;

		case 18	: pPC = new Cpc1350;	pPC->setName("PC-1350");break;
		case 19	: pPC = new Cpc1360;	pPC->setName("PC-1360");break;
		case 20	: pPC = new Cpc1401;	pPC->setName("PC-1401");break;
		case 21	: pPC = new Cpc1402;	pPC->setName("PC-1402");break;
		case 22	: pPC = new Cpc1403;	pPC->setName("PC-1403");break;
		case 23:  pPC = new Cpc1403H;	pPC->setName("PC-1403H");break;
		case 24	: pPC = new Cpc1450;	pPC->setName("PC-1450");break;
		case 25	: pPC = new Cpc1475;	pPC->setName("PC-1475");break;

		case 30	: pPC = new Cpc1500;	pPC->setName("PC-1500");break;
		case 31 : pPC = new Cpc1500A;	pPC->setName("PC-1500A");break;
		case 32 : pPC = new Ctrspc2;	pPC->setName("Tandy PC-2");break;

        case 34 : pPC = new Cpc1600;	pPC->setName("PC-1600");break;

        case 35 : pPC = new Cpc2500;	pPC->setName("PC-2500");break;

        case 101: pPC = new Cce125;		pPC->setName("CE-125");break;
		case 102: pPC = new Cce126;		pPC->setName("CE-126P");break;
		case 103: pPC = new Cce123;		pPC->setName("CE-123P");break;
		case 104: pPC = new Cce129;		pPC->setName("CE-129P");break;
		case 107: pPC = new Cce150;		pPC->setName("CE-150");break;
		case 108: pPC = new Cce152;		pPC->setName("CE-152");break;
		case 109: pPC = new C263591;	pPC->setName("26-3591");break;
 
		case 105: pPC = new Csio;		pPC->setName("Serial Console");break;
		case 106: pPC = new Ccable;		pPC->setName("11Pins Cable");break;
        case 111: pPC = new Cpotar;		pPC->setName("Potar");break;

        case 110: pPC = new Cce1600p;		pPC->setName("CE-1600P");break;
 		
		default			: return 0;
	}
	AddLog(LOG_MASTER,"OK1");
	if (!pPC->init()) return 0;
 
	int l = mainwindow->menuBar()->height();
//	mainwindow->resize(QSize(pPC->Pc_DX, pPC->Pc_DY+l));
	
	pPC->FinalImage = new QImage(QSize(pPC->Pc_DX, pPC->Pc_DY),QImage::Format_RGB32);
	

#define NBFRAMEPERSEC		20
#define FRAMERATE			(1000/NBFRAMEPERSEC)
#define TIMER_RES 30

	pPC->InitDisplay();

	pPC->Move(QPoint(0,l));	
	pPC->show();

    return pPC;
}


void CPocketThread::run()
{
	bool pause = true;

	while(true)
	{
		pause = true;
		for (int i=0;i<listpPObject.size();i++)
		{
			CPObject *pPC = listpPObject.at(i);
			if (pPC->getfrequency() != 0)
			{
				if (pPC->pTIMER->CheckSpeed())
				{
					pPC->run();
					// WRITE the LINK BOX Connector
                    mainwindow->pdirectLink->clearlog();
                    mainwindow->pdirectLink->Output(pPC);
					pause = false;
				}
				if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->captureData();
			}
			if (pPC->toDestroy)
			{
                // TODO - unlink before destroy

				listpPObject.removeAt(i);
				i--;
				emit Destroy(pPC);
			}
		}
		if (pause) usleep(1);	
	}
}	

void CPocketThread::SendSignal_AddLogItem(QString str)
{
	emit AddLogItem(str);
}

void CPocketThread::SendSignal_Resize(QSize size,CPObject *pPObject)
{
	emit Resize(size,pPObject);
}
