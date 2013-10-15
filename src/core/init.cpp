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

#ifdef P_AVOID
#include "libavoid.h"
#endif

#include "common.h"

#include "dialoganalog.h"


#include "pcxxxx.h" 
#include "Inter.h"
#include "clink.h"

#include "sharp/pc1211.h"
#include "sharp/pc1245.h"
#include "sharp/pc1250.h"
#include "sharp/pc1251.h"
#include "sharp/pc1255.h"
#include "sharp/pc1260.h"
#include "sharp/pc1280.h"

#include "sharp/pc1350.h"
#include "sharp/pc1360.h"

#include "sharp/pc2500.h"

#include "sharp/pc1401.h"
#include "sharp/pc1402.h"
#include "sharp/pc1403.h"
#include "sharp/pc1421.h"
#include "sharp/pc1450.h"
#include "sharp/pc1475.h"
#include "sharp/pc1425.h"

#include "sharp/pc1500.h"
#include "sharp/ce150.h"

#include "sharp/pc1600.h"
#include "sharp/ce1600p.h"

#include "sharp/ce152.h"
#include "sio.h"
#include "sharp/ce122.h"
#include "sharp/Ce126.h"
#include "sharp/ce120p.h"
#include "sharp/ce125.h"
#include "sharp/ce140p.h"
#include "sharp/ce140f.h"
#include "ccable.h"

#include "potar.h"
#include "cesimu.h"

#include "cx07.h"
#include "cx710.h"
#include "sharp/e500.h"
#include "sharp/g850v.h"
#include "casio/pb1000.h"
#include "casio/pb2000.h"
#include "casio/md100.h"
#include "casio/fp100.h"
#include "casio/fp40.h"
#include "casio/fp200.h"

#include "casio/z1.h"
#include "pc2001.h"
#include "pc2021.h"

#include "general/lbc1100.h"
#include "general/cl1000.h"

#include "tpc8300.h"
#include "tp83.h"
#include "panasonic/rlh1000.h"
#include "panasonic/rlp6001.h"
#include "panasonic/rlp9001.h"
#include "panasonic/rlp1004a.h"

#include "ti/ti57.h"
#include "hp/hp41.h"
#include "hp/hp82143A.h"

#include "postit.h"

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
        case PC1425	: pPC = new Cpc1425;	pPC->setName("PC-1425");break;

        case PC1500	: pPC = new Cpc1500;	pPC->setName("PC-1500");break;
        case PC1500A: pPC = new Cpc1500A;	pPC->setName("PC-1500A");break;
        case TandyPC2:pPC = new Ctrspc2;	pPC->setName("Tandy PC-2");break;

        case PC1600 : pPC = new Cpc1600;	pPC->setName("PC-1600");break;

        case PC2500 : pPC = new Cpc2500;	pPC->setName("PC-2500");break;

        case CE122  : pPC = new Cce122;		pPC->setName("CE-122");break;
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
        case CE127R : pPC = new Cce127r;	pPC->setName("CE-127R");break;
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
        case FP40   : pPC = new Cfp40;      pPC->setName("Casio FP-40"); break;
        case FX890P : pPC = new Cz1(0,FX890P);  pPC->setName("Casio FX-890P"); break;
        case Z1     : pPC = new Cz1;            pPC->setName("Casio Z-1"); break;
        case Z1GR   : pPC = new Cz1(0,Z1GR);    pPC->setName("Casio Z-1GR"); break;

        case PC2001   : pPC = new Cpc2001;      pPC->setName("Nec PC-2001"); break;
        case PC2021   : pPC = new Cpc2021;      pPC->setName("Nec PC-2021"); break;
        case PC2081   : pPC = new Cpc2081;      pPC->setName("Nec PC-2081"); break;
        case LBC1100  : pPC = new Clbc1100;     pPC->setName("General LBC-1100"); break;

        case CL1000  : pPC = new Ccl1000;       pPC->setName("General CL-1000"); break;
        case TPC8300 : pPC = new Ctpc8300;      pPC->setName("SANCO TPC-8300"); break;
        case TP83    : pPC = new Ctp83;         pPC->setName("SANCO TP-83"); break;

        case RLH1000 : pPC = new Crlh1000;      pPC->setName("Panasonic HHC RL-H1000"); break;
        case RLP6001 : pPC = new Crlp6001;      pPC->setName("Panasonic RL-P6001"); break;
        case RLP9001 : pPC = new Crlp9001;      pPC->setName("Panasonic RL-P9001"); break;
        case RLP1004A : pPC = new Crlp1004a;      pPC->setName("Panasonic RL-P1004A"); break;

        case POSTIT : pPC = new Cpostit;      pPC->setName("Post-it"); break;

        case TI57 : pPC = new Cti57;      pPC->setName("TI-57"); break;
        case HP41 : pPC = new Chp41;      pPC->setName("HP-41"); break;
    case HP82143A : pPC = new Chp82143A;      pPC->setName("HP-82143A Printer"); break;


        default			: return 0;
    }
    qWarning()<<"init";
	AddLog(LOG_MASTER,"OK1");
    int dx = pPC->getDX()*mainwindow->zoom/100;
    int dy = pPC->getDY()*mainwindow->zoom/100;
#ifdef AVOID
    Avoid::Rectangle rectangle(Avoid::Point(-10.0, -10.0), Avoid::Point(dx+20, dy+20));
    mainwindow->shapeRefList[pPC] = new Avoid::ShapeRef(mainwindow->router, rectangle);
    mainwindow->router->addShape(mainwindow->shapeRefList[pPC]);
#endif
	if (!pPC->init()) return 0;
    qWarning()<<"init ok";

//	int l = mainwindow->menuBar()->height();
//	mainwindow->resize(QSize(pPC->Pc_DX, pPC->Pc_DY+l));

    pPC->FinalImage = new QImage(QSize(pPC->getDX(), pPC->getDY()),QImage::Format_RGB32);


#define NBFRAMEPERSEC		20
#define FRAMERATE			(1000/NBFRAMEPERSEC)
#define TIMER_RES 30

	pPC->InitDisplay();





    pPC->Move(QPoint(0,0));
    pPC->setGeometry(0,0,dx,dy);
	pPC->show();



    return pPC;
}

void CPocketThread::run()
{
    bool pause = true;

#ifdef NEWTIMER
    QElapsedTimer timer;
    timer.start();
#endif

#ifdef EMSCRIPTEN
    int j=0;
    while((j++)<1000)
#else
    while(true)
#endif
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
                    quint64 cs = pPC->pTIMER->currentState();
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
#ifdef EMSCRIPTEN
        if (pause) return;
#else
        if (pause) msleep(10);
#endif
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
