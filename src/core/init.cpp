 /********************************************************************************************************
 * PROGRAM      : 
 * DATE - TIME  : samedi 28 octobre 2006 - 12h42
 * AUTHOR       :  (  )
 * FILENAME     : init.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include <QtGui>
#include <QString>
#include <QPainter>

#include "init.h"
#include "mainwindowpockemul.h"
#include "common.h"

#include "dialoganalog.h"

#include "pcxxxx.h" 

#include "pc1211.h"
#include "pc1245.h"
#include "pc1250.h"
#include "pc1251.h"
#include "pc1255.h"
#include "pc1260.h"
#include "pc1280.h"

#include "pc1350.h"
#include "pc1360.h"

#include "pc2500.h"

#include "pc1401.h"
#include "pc1402.h"
#include "pc1403.h"
#include "pc1421.h"
#include "pc1450.h"
#include "pc1475.h"

#include "pc1500.h"
#include "ce150.h"

#include "pc1600.h"
#include "ce1600p.h"

#include "ce152.h"
#include "sio.h"
#include "Ce126.h"
#include "ce120p.h"
#include "ce125.h"
#include "ce140p.h"
#include "ce140f.h"
#include "clink.h"
#include "ccable.h"

#include "potar.h"
#include "cesimu.h"

#include "cx07.h"
#include "cx710.h"
#include "e500.h"
#include "g850v.h"
#include "pb1000.h"
#include "pb2000.h"
#include "md100.h"
#include "fp100.h"
#include "fp200.h"

#include "z1.h"


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

        case EMPTY	: return 0;

        case PC1211	: pPC = new Cpc1211;	pPC->setName("PC-1211");break;
        case PC1245	: pPC = new Cpc1245;	pPC->setName("PC-1245");break;
        case MC2200	: pPC = new Cmc2200;	pPC->setName("MC-2200");break;
        case PC1250	: pPC = new Cpc1250;	pPC->setName("PC-1250");break;
        case PC1251	: pPC = new Cpc1251;	pPC->setName("PC-1251");break;
        case PC1251H: pPC = new Cpc1251H;	pPC->setName("PC-1251H");break;
        case PC1255	: pPC = new Cpc1255;	pPC->setName("PC-1255");break;
        case TandyPC3:pPC = new Ctrspc3;	pPC->setName("Tandy PC-3");break;
        case TandyPC3EXT:pPC = new Ctrspc3Ext;	pPC->setName("Tandy PC-3 (4Ko)");break;

        case PC1260	: pPC = new Cpc1260;	pPC->setName("PC-1260");break;
        case PC1261	: pPC = new Cpc1261;	pPC->setName("PC-1261");break;
        case PC1262	: pPC = new Cpc1262;	pPC->setName("PC-1262");break;
        case PC1280	: pPC = new Cpc1280;	pPC->setName("PC-1280");break;

        case PC1350	: pPC = new Cpc1350;	pPC->setName("PC-1350");break;
        case PC1360	: pPC = new Cpc1360;	pPC->setName("PC-1360");break;
        case PC1401	: pPC = new Cpc1401;	pPC->setName("PC-1401");break;
        case PC1402	: pPC = new Cpc1402;	pPC->setName("PC-1402");break;
        case PC1403	: pPC = new Cpc1403;	pPC->setName("PC-1403");break;
        case PC1403H: pPC = new Cpc1403H;	pPC->setName("PC-1403H");break;
        case PC1421	: pPC = new Cpc1421;	pPC->setName("PC-1421");break;
        case PC1450	: pPC = new Cpc1450;	pPC->setName("PC-1450");break;
        case PC1475	: pPC = new Cpc1475;	pPC->setName("PC-1475");break;

        case PC1500	: pPC = new Cpc1500;	pPC->setName("PC-1500");break;
        case PC1500A: pPC = new Cpc1500A;	pPC->setName("PC-1500A");break;
        case TandyPC2:pPC = new Ctrspc2;	pPC->setName("Tandy PC-2");break;

        case PC1600 : pPC = new Cpc1600;	pPC->setName("PC-1600");break;

        case PC2500 : pPC = new Cpc2500;	pPC->setName("PC-2500");break;

        case CE125  : pPC = new Cce125;		pPC->setName("CE-125");break;
        case MP220  : pPC = new Cmp220;     pPC->setName("MP-220");break;
        case CE120P : pPC = new Cce120p;	pPC->setName("CE-120P");break;
        case CE126P : pPC = new Cce126;		pPC->setName("CE-126P");break;
        case CE123P : pPC = new Cce123;		pPC->setName("CE-123P");break;
        case CE129P : pPC = new Cce129;		pPC->setName("CE-129P");break;
        case CE140P : pPC = new Cce140p;	pPC->setName("CE-140P");break;
        case CE140F : pPC = new Cce140f;	pPC->setName("CE-140F");break;
        case CE150  : pPC = new Cce150;		pPC->setName("CE-150");break;
        case CE152  : pPC = new Cce152;		pPC->setName("CE-152");break;
        case TANDY263591: pPC = new C263591;pPC->setName("26-3591");break;

        case SerialConsole: pPC = new Csio;	pPC->setName("Serial Console");break;
        case CABLE11Pins: pPC = new Ccable;	pPC->setName("11Pins Cable");break;
        case POTAR      : pPC = new Cpotar;	pPC->setName("Potar");break;
        case Simulator  : pPC = new Ccesimu;pPC->setName("Simulator");break;

        case CE1600P: pPC = new Cce1600p;	pPC->setName("CE-1600P");break;

        case X07    : pPC = new Cx07;       pPC->setName("Canon X-07"); break;
        case X710   : pPC = new Cx710;      pPC->setName("Canon X-710"); break;
        case E500   : pPC = new Ce500;      pPC->setName("PC-E500"); break;
        case E550   : pPC = new Ce550;      pPC->setName("PC-E550"); break;
        case G850V  : pPC = new Cg850v;     pPC->setName("PC-G850V"); break;
        case PB1000 : pPC = new Cpb1000;    pPC->setName("Casio PB-1000"); break;
        case PB2000 : pPC = new Cpb2000;    pPC->setName("Casio PB-2000C"); break;
        case FP200  : pPC = new Cfp200;     pPC->setName("Casio FP-200"); break;
        case MD100  : pPC = new Cmd100;     pPC->setName("Casio MD-100"); break;
        case FP100  : pPC = new Cfp100;     pPC->setName("Casio FP-100"); break;
        case Z1     : pPC = new Cz1;        pPC->setName("Casio Z-1"); break;
        case Z1GR   : pPC = new Cz1GR;      pPC->setName("Casio Z-1GR"); break;
        default			: return 0;
    }
	AddLog(LOG_MASTER,"OK1");
	if (!pPC->init()) return 0;
 
//	int l = mainwindow->menuBar()->height();
//	mainwindow->resize(QSize(pPC->Pc_DX, pPC->Pc_DY+l));

    pPC->FinalImage = new QImage(QSize(pPC->getDX(), pPC->getDY()),QImage::Format_RGB32);


#define NBFRAMEPERSEC		20
#define FRAMERATE			(1000/NBFRAMEPERSEC)
#define TIMER_RES 30

	pPC->InitDisplay();

    pPC->Move(QPoint(0,0));
    pPC->setGeometry(0,0,pPC->getDX()*mainwindow->zoom/100,pPC->getDY()*mainwindow->zoom/100);
	pPC->show();

    return pPC;
}

void CPocketThread::run()
{
    bool pause = true;

    QElapsedTimer timer;
    timer.start();

    while(true)
    {
        //int j=0;
        //while ((j++) < 20)
        {
            pause = true;
            for (int i=0;i<listpPObject.size();i++)
            {
                CPObject *pPC = listpPObject.at(i);
                // si objet maitre
                int f = pPC->getfrequency();
                if ( f != 0)
                {
                    // test si en retard
                    qint64 cs = pPC->pTIMER->currentState();
                    if (pPC->pTIMER->state <= cs)
                    {
                        if (pPC->ioFreq > 0) {
                            int step = MIN(f / pPC->ioFreq - pPC->pTIMER->deltaStep,cs-pPC->pTIMER->state);
                            pPC->pTIMER->deltaStep = pPC->runRange(step) - step;
                        }
                        else {
                            pPC->run();

                        }
                        // WRITE the LINK BOX Connector
                        mainwindow->pdirectLink->clearlog();
                        mainwindow->pdirectLink->Output(pPC);

                        pause = false;

                        if ((mainwindow->dialoganalogic) && mainwindow->dialoganalogic->capture()) {
                            mainwindow->dialoganalogic->captureData();
                        }

                    }
                }
                if (pPC->toDestroy)
                {
                    // Unlink before destroy
                    mainwindow->slotUnlink(pPC);

                    listpPObject.removeAt(i);
                    i--;
                    emit Destroy(pPC);
                }
            }
        }
        if (pause) msleep(10);
#ifdef NEWTIMER
        mainwindow->rawclk += timer.nsecsElapsed();
        timer.restart();
#endif
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
