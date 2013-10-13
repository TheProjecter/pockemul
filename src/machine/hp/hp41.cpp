// *********************************************************************
//    Copyright (c) 1989-2002  Warren Furlow
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// *********************************************************************

// *********************************************************************
// HP41.cpp : implementation file
// *********************************************************************
#if 1
#include <math.h>
#include <QDebug>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QPainter>

#include "hp41.h"
#include "hp41Cpu.h"
#include "hp41mod.h"
#include "Lcdc_hp41.h"
#include "Keyb.h"
#include "Inter.h"
#include "init.h"
#include "Connect.h"

#include "mainwindowpockemul.h"
extern MainWindowPockemul *mainwindow;



/****************************/
// Constructor
/****************************/
Chp41::Chp41(CPObject *parent):CpcXXXX(parent)
{

    setfrequency( (int) 6262*56);  // 80µs per cycle
    setcfgfname(QString("hp41"));

    SessionHeader	= "HP41PKM";
    Initial_Session_Fname ="hp41.pkm";

    BackGroundFname	= P_RES(":/hp41/hp41.png");
    LcdFname		= P_RES(":/hp41/hp41lcd.png");
    SymbFname		= "";

    TopFname = P_RES(":/hp41/top.png");

    TopImage=LeftImage=RightImage=BottomImage=BackImage = 0;
    memsize		= 0x2000;
    InitMemValue	= 0x00;

    SlotList.clear();
        SlotList.append(CSlot(8 , 0x0000 ,"", ""	, RAM , "RAM"));

    setDXmm(78);
    setDYmm(142);
    setDZmm(30);

    setDX(279);
    setDY(508);

    Lcd_X		= 30;
    Lcd_Y		= 43;
    Lcd_DX		= 220;
    Lcd_DY		= 40;
    Lcd_ratio_X	= 1;
    Lcd_ratio_Y	= 1;

    Lcd_Symb_X	= 55;
    Lcd_Symb_Y	= 41;
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;

    pTIMER		= new Ctimer(this);
    pLCDC		= new Clcdc_hp41(this);
    pCPU		= new Chp41Cpu(this);    hp41cpu = (Chp41Cpu*)pCPU;
    pKEYB		= new Ckeyb(this,"hp41.map");

    flipping = false;
    currentView = FRONTview;
}


/****************************/
// destructor
/****************************/
Chp41::~Chp41()
  {
  if (GetTrace())
    StopTrace();
  SetSoundMode(eSoundNone);

  DeInitTimer();
  ExecuteEvent=0;

  // free modules and rom pages
  while (!ModuleList.isEmpty()) {
      ModuleHeader *pModule=ModuleList.takeFirst();
      delete pModule;
  }
  for (int page=0;page<=0xf;page++)
    for (int bank=1;bank<=4;bank++)
      FreePage(page,bank);

//  free(pRAM);

  SetKeyboard(eKeyboardNone,0,0,0);
  }


bool Chp41::InitDisplay(void)
{
    if (!TopFname.isEmpty()) TopImage = CreateImage(FaceRect(TOPview),TopFname);
    if (!LeftFname.isEmpty()) LeftImage = CreateImage(FaceRect(LEFTview),LeftFname);
    if (!RightFname.isEmpty()) RightImage = CreateImage(FaceRect(RIGHTview),RightFname);
    if (!BottomFname.isEmpty()) BottomImage = CreateImage(FaceRect(BOTTOMview),BottomFname);

    CPObject::InitDisplay();

    return(1);
}


#define RANGEPERCENT 20
#define THIN 5

Direction Chp41::borderClick(QPoint pt) {
    int x1 = this->width()*(50 - RANGEPERCENT/2) /100;
    int x2 = this->width()*(50 + RANGEPERCENT/2) /100;
    int y1 = this->height()*(50 - RANGEPERCENT/2) /100;
    int y2 = this->height()*(50 + RANGEPERCENT/2) /100;

    if ( (pt.x()>=x1) && (pt.x()<=x2) ) {
        if (pt.y() <= this->height() *THIN / 100) return TOPdir;
        if (pt.y() >= this->height() *(100-THIN) / 100) return BOTTOMdir;
    }
    if ( (pt.y()>=y1) && (pt.y()<=y2) ) {
        if (pt.x() <= this->width() *THIN / 100) return LEFTdir;
        if (pt.x() >= this->width() *(100-THIN) / 100) return RIGHTdir;
    }

    return NONEdir;

}

void Chp41::setAngle(int value) {
    this->m_angle = value;
}

void Chp41::setZoom(qreal value)
{
    this->m_zoom = value;
}

QSize Chp41::FaceRect(View v) {
    float _ratio = this->getDX()/this->getDXmm();

    switch (v) {

    case FRONTview:
    case BACKview: return QSize(this->getDX(),this->getDY());
    case TOPview:
    case BOTTOMview: return QSize(this->getDX(),this->getDZmm()*_ratio);
    case LEFTview:
    case RIGHTview: return QSize(this->getDZmm()*_ratio,this->getDY());
    }
    return QSize(0,0);
}

void Chp41::flip(Direction dir) {
    // IF CONNECTED to printer , exit
//    QList<CPObject *> ConList;
//    mainwindow->pdirectLink->findAllObj(this,&ConList);
//    if (!ConList.isEmpty()) return;
    // Animate close


    targetSize = FaceRect(targetView);
    currentFlipDir = dir;

    qWarning()<<"targetdir:"<<targetSize;

    QPropertyAnimation *animation1 = new QPropertyAnimation(this, "angle");
    QPropertyAnimation *animation2 = new QPropertyAnimation(this, "zoom");
     animation1->setDuration(1500);
     animation2->setDuration(1500);

     switch (currentFlipDir) {
     case TOPdir:
         animation1->setStartValue(0);
         animation1->setEndValue(90);
         animation2->setKeyValueAt(0.0,1.0);
         animation2->setKeyValueAt(0.5,.75);
         animation2->setKeyValueAt(1.0,1.0);
         animationView1 = currentView;
         animationView2 = targetView;
         clearMask();
         break;
     case BOTTOMdir:
         animation1->setStartValue(90);
         animation1->setEndValue(0);
         animation2->setKeyValueAt(0,1.0);
         animation2->setKeyValueAt(0.5,.75);
         animation2->setKeyValueAt(1,1.0);
         animationView1 = targetView;
         animationView2 = currentView;
         clearMask();
     }

     QParallelAnimationGroup *group = new QParallelAnimationGroup;
     group->addAnimation(animation1);
     group->addAnimation(animation2);

     connect(animation1,SIGNAL(valueChanged(QVariant)),this,SLOT(update()));
     connect(animation1,SIGNAL(finished()),this,SLOT(endAnimation()));
     flipping = true;
     group->start();

}
#define RATIO .25
void Chp41::paintEvent(QPaintEvent *event)
{
    if (flipping)
    {

        UpdateFinalImage();

        QPainter painter;


        if (FinalImage)
        {
            int w = FaceRect(animationView1).width() * mainwindow->zoom/100.0;//this->width();
            int h = FaceRect(animationView1).height() * mainwindow->zoom/100.0;//this->height();
            int wt = FaceRect(animationView2).width() * mainwindow->zoom/100.0;
            int ht = FaceRect(animationView2).height()* mainwindow->zoom/100.0;
//            qWarning()<<"angle:"<<m_angle;
            painter.begin(this);
            painter.translate(w/2 ,ht * m_angle/90);
            QTransform matrix;
            matrix.scale(m_zoom,m_zoom);
            painter.setTransform(matrix,true);
            QTransform matrix2;
            matrix2.rotate(-m_angle, Qt::XAxis);
            painter.setTransform(matrix2,true);
            painter.drawImage(QPoint(-w/2,0),
                              getViewImage(animationView1)->scaled(QSize(w,h*(90 -m_angle)/90),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)
                              );

            painter.end();

            painter.begin(this);
            painter.translate(w/2 ,ht * m_angle/90);
            painter.setTransform(matrix,true);
            QTransform matrix3;
            matrix3.rotate(90-m_angle, Qt::XAxis);
            painter.setTransform(matrix3,true);

            painter.drawImage(QPoint(-w/2,-ht * m_angle/90),
                              getViewImage(animationView2)->scaled(QSize(w,ht * m_angle/90),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)
                                  );
            painter.end();

        }
    }
    else {
        CPObject::paintEvent(event);
    }
}

void Chp41::endAnimation(){
    currentView = targetView;
    currentFlipDir = NONEdir;
    flipping = false;

    changeGeometry(this->posx(),this->posy(),FaceRect(currentView).width()*mainwindow->zoom/100.0,FaceRect(currentView).height()*mainwindow->zoom/100.0);

}

void Chp41::mousePressEvent(QMouseEvent *event) {
    // if click on the border
    // width ?

    Direction dir = borderClick(event->pos());

    targetView = currentView;
    switch (currentView) {
    case FRONTview:
        switch (dir) {
        case TOPdir: targetView = TOPview; break;
        case LEFTdir: targetView = LEFTview; break;
        case RIGHTdir: targetView = RIGHTview; break;
        case BOTTOMdir: targetView = BOTTOMview; break;
        }
        break;
    case TOPview:
        switch (dir) {
        case TOPdir: targetView = BACKview; break;
        case LEFTdir: targetView = LEFTview; break;
        case RIGHTdir: targetView = RIGHTview; break;
        case BOTTOMdir: targetView = FRONTview; break;
        }
        break;
    case LEFTview:
        switch (dir) {
        case TOPdir: targetView = TOPview; break;
        case LEFTdir: targetView = BACKview; break;
        case RIGHTdir: targetView = FRONTview; break;
        case BOTTOMdir: targetView = BOTTOMview; break;
        }
        break;
    case RIGHTview:
        switch (dir) {
        case TOPdir: targetView = TOPview; break;
        case LEFTdir: targetView = FRONTview; break;
        case RIGHTdir: targetView = BACKview; break;
        case BOTTOMdir: targetView = BOTTOMview; break;
        }
        break;
    case BOTTOMview:
        switch (dir) {
        case TOPdir: targetView = FRONTview; break;
        case LEFTdir: targetView = LEFTview; break;
        case RIGHTdir: targetView = RIGHTview; break;
        case BOTTOMdir: targetView = BACKview; break;
        }
        break;
    case BACKview:
        switch (dir) {
        case TOPdir: targetView = BOTTOMview; break;
        case LEFTdir: targetView = RIGHTview; break;
        case RIGHTdir: targetView = LEFTview; break;
        case BOTTOMdir: targetView = TOPview; break;
        }
        break;

    }

    if ( (targetView != currentView) && getViewImage(targetView) ) {
        QSize _s = FaceRect(currentView).expandedTo(FaceRect(targetView));
        changeGeometry(this->posx(),this->posy(),_s.width()*mainwindow->zoom/100.0,_s.height()*mainwindow->zoom/100.0);
        flip(dir);
    }

    CPObject::mousePressEvent(event);

}

QImage * Chp41::getViewImage(View v) {
    switch (v) {
    case FRONTview: return FinalImage;
    case TOPview: return TopImage;
    case LEFTview: return LeftImage;
    case RIGHTview: return RightImage;
    case BOTTOMview: return BottomImage;
    case BACKview: return BackImage;
    }
    return 0;
}

bool Chp41::init()
{
    //pCPU->logsw = true;



    CpcXXXX::init();

    WatchPoint.remove(this);

    // trace
    hLogFile=NULL;
    InstSetIndex=0;
    TEFIndex=0;
    fTrace=false;
    memset(szTraceOut,0,sizeof(szTraceOut));

    eKeyboard=0;
    eFont=0;
    UseAltPunc=0;

    // activity dot
    Indicator=0;
    RectIndicator = QRect(3,3,7,7);

    // Connectors
    pConnector0 = new Cconnector(this,15,0,Cconnector::hp41,"HP-41 Module 0",false,QPoint(715,50));
    publish(pConnector0);
    pConnector1 = new Cconnector(this,15,0,Cconnector::hp41,"HP-41 Module 1",false,QPoint(715,50));
    publish(pConnector1);
    pConnector2 = new Cconnector(this,15,0,Cconnector::hp41,"HP-41 Module 2",false,QPoint(715,50));
    publish(pConnector2);
    pConnector3 = new Cconnector(this,15,0,Cconnector::hp41,"HP-41 Module 3",false,QPoint(715,50));
    publish(pConnector3);

    WatchPoint.remove(this);
    WatchPoint.add(&pConnector0_value,64,15,this,"HP-41 Module 0");
    WatchPoint.add(&pConnector1_value,64,15,this,"HP-41 Module 1");
    WatchPoint.add(&pConnector2_value,64,15,this,"HP-41 Module 2");
    WatchPoint.add(&pConnector3_value,64,15,this,"HP-41 Module 3");

    // ROM variables
    for (int page=0;page<=0xf;page++)
    {
        for (int bank=1;bank<=4;bank++)
            PageMatrix[page][bank-1]=NULL;
        active_bank[page]=1;
    }
    pCurPage=NULL;
    CurPage=0;
    NextActualBankGroup=1;


    PC_LAST=0;
    PC_TRACE=0;

    // display registers and variables
    UpdateDisplay=1;
    UpdateAnnun=1;
    DisplayOn=0;
    memset(DIS_C_REG,0,sizeof(DIS_C_REG));
    memset(DIS_B_REG,0,sizeof(DIS_B_REG));
    memset(DIS_A_REG,0,sizeof(DIS_A_REG));
    DIS_ANNUN_REG=0;
    Contrast=DEFAULT_CONTRAST;  // default halfnut value
    //  FontColor=0;                // black

    // instruction delay
    //  QueryPerformanceFrequency(&PCPerf);
    memset(PCCount,0,sizeof(PCCount));
    InstrNSec=0;

    // control and state variables
    MinCLRKEY=0;
    fEnableCLRKEY=false;
    ExecuteEvent=0;
    ProcInterval=0;
    ProcCycles=0;
    MemModules=0;
    XMemModules=0;
    fPrinter=false;
    fCardReader=false;
    fTimer=false;
    fWand=false;
    fHPIL=false;
    fInfrared=false;
    SoundMode=eSoundNone;
    fRunEnable=false;
    fBreakPtsEnable=false;
    fBreak=false;
    nBreakPts=0;

    ModuleHeader *pModuleNew;
    int nRes=LoadMOD(pModuleNew,P_RES(":/hp41/MOD/NUT-CX.MOD"));
    LoadMOD(pModuleNew,P_RES(":/hp41/MOD/PRINTER.MOD"));
    hp41cpu->set_PC(0);
    qWarning()<<Chp41Mod(P_RES(QString(":/hp41/MOD/NUT-CX.MOD"))).output_mod_info(1,1);
    qWarning()<<"Load Module:"<<nRes;

//    StartTrace();

    pTIMER->resetTimer(0);

   return true;
}

void Chp41::TurnOFF()
{
    ASKYN _save = mainwindow->saveAll;
    mainwindow->saveAll = YES;
    CpcXXXX::TurnOFF();
    mainwindow->saveAll = _save;
}

bool Chp41::run()
{


    if (pTIMER->usElapsedId(0)>=10000) {
        pTIMER->resetTimer(0);
        TimerProc();
    }
    SetKeyDown();
//    qWarning()<<"hp41::run";
    //    if (!fRunEnable)
    //        return;
    //    if (ExecuteEvent)
    //        return;
    //    if (!ProcInterval || !ProcCycles)  // if unintialized
    //        return;

    //  // compute delay for 1 HP41 machine cycle - necessary for correct operation of speaker sound & interval timer
    //  InstrNSec=(UINT64)( ((double)PCPerf.QuadPart*(double)DEFAULT_INST_SPEED/1e6));
    //  WaveSound.SetDuration((DWORD)DEFAULT_INST_SPEED);

    //  // compute delay for 1 HP41 machine cycle - necessary for correct operation of speaker sound & interval timer
    //  // scales delay according to ProcInterval, where the default is the same as the real machine
    //  InstrNSec=(UINT64)( ((double)PCPerf.QuadPart*(double)DEFAULT_INST_SPEED/1e6)*((double)ProcInterval/(double)DEFAULT_PROC_INTERVAL) );
    //  WaveSound.SetDuration( (DWORD)( (double)DEFAULT_INST_SPEED*(double)ProcInterval/(double)DEFAULT_PROC_INTERVAL ) );


    // validate PC_REG - important if a ROM is removed from the config while it is being executed
    uint page=(pCPU->get_PC()&0xf000)>>12;
    if (PageMatrix[page][active_bank[page]-1]==NULL)           // no rom loaded here
        pCPU->set_PC(0);

    CpcXXXX::run();

    pTIMER->state+=56;
    return true;

}



UINT8 Chp41::in(UINT8 address)
{
    return 0;
}

UINT8 Chp41::out(UINT8 address, UINT8 value)
{
    return 0;
}


/*****************************/
void Chp41::MemoryLost()
  {
  if (eKeyboard==eKeyboardNone)
    return;
  //BASE=16;
  for (int page=1;page<=0xf;page++)
    active_bank[page]=1;
  pCPU->set_PC(0x0232);
  ResetTimer();
  }


/*****************************/
void Chp41::EnableRun()
  {
  fRunEnable=true;
  }


/*****************************/
void Chp41::DisableRun()
  {
  fRunEnable=false;
  }



/*****************************/
// stops the callback timer
/*****************************/
void Chp41::Halt()
  {
//  if (ExecuteEvent)
//    KillTimer(NULL,ExecuteEvent);
  ExecuteEvent=0;
  }


/*****************************/
void Chp41::SetProcParams(
  int ProcIntervalIn,            // time in milliseconds between processor runs
  int ProcCyclesIn)              // number of processor cycles to run each time
  {
  ProcInterval=ProcIntervalIn;
  ProcCycles=ProcCyclesIn;
  if (ExecuteEvent)              // restart processor with new values
    {
    Halt();
    run();
    }
  }


/*****************************/
void Chp41::GetProcParams(
  int &ProcIntervalOut,          // time in milliseconds between processor runs
  int &ProcCyclesOut)            // number of processor cycles to run each time
  {
  ProcIntervalOut=ProcInterval;
  ProcCyclesOut=ProcCycles;
  }


/****************************/
void Chp41::SetSoundMode(int val) {
    //  if (GetVersion()&0x80000000)   // high bit set if win95/98
    //    Speaker(0,1);	             // be sure speaker is off

    //  if (val==eSoundSpeaker)
    //    {
    //    if (PCPerf.QuadPart &&           // 0 if performance counter not supported
    //      (GetVersion()&0x80000000) )    // high order bit set if win95/98/win32s - sound not supported on NT
    //      SoundMode=val;
    //    else
    //      SoundMode=eSoundNone;
    //    }
    //  else
    SoundMode=val;
}


/****************************/
int Chp41::GetSoundMode() {
    return(SoundMode);
}







/****************************/
// return true if RAM exists at the specified RAM address
/****************************/
bool Chp41::Chk_Adr(UINT32 *d, UINT32 data)
{
    if ((*d>=0x000)&&(*d<=0x00f))      // status registers
        return(true);
    if ((*d>=0x010)&&(*d<=0x03f))      // void
        return(false);
    if ((*d>=0x040)&&(*d<=0x0bf))      // extended functions - 128 regs
        return(XMemModules>=1);
    if ((*d>=0x0c0)&&(*d<=0xff))       // main memory for C
        return(true);
    if ((*d>=0x100)&&(*d<=0x13f))      // memory module 1
        return(MemModules>=1);
    if ((*d>=0x140)&&(*d<=0x17f))      // memory module 2
        return(MemModules>=2);
    if ((*d>=0x180)&&(*d<=0x1bf))      // memory module 3
        return(MemModules>=3);
    if ((*d>=0x1c0)&&(*d<=0x1ff))      // memory module 4
        return(MemModules>=4);
    // void: 200
    if ((*d>=0x201)&&(*d<=0x2ef))      // extended memory 1 - 239 regs
        return(XMemModules>=2);
    // void: 2f0-300
    if ((*d>=0x301)&&(*d<=0x3ef))      // extended memory 2 - 239 regs
        return(XMemModules>=3);
    // void: 3f0-3ff
    // end of memory: 3ff
    return(false);

}

bool Chp41::Chk_Adr_R(UINT32 *d, UINT32 *data)
{
}



int Chp41::RamExist(word addr)
  {
  if ((addr>=0x000)&&(addr<=0x00f))      // status registers
    return(1);
  if ((addr>=0x010)&&(addr<=0x03f))      // void
    return(0);
  if ((addr>=0x040)&&(addr<=0x0bf))      // extended functions - 128 regs
    return(XMemModules>=1);
  if ((addr>=0x0c0)&&(addr<=0xff))       // main memory for C
    return(1);
  if ((addr>=0x100)&&(addr<=0x13f))      // memory module 1
    return(MemModules>=1);
  if ((addr>=0x140)&&(addr<=0x17f))      // memory module 2
    return(MemModules>=2);
  if ((addr>=0x180)&&(addr<=0x1bf))      // memory module 3
    return(MemModules>=3);
  if ((addr>=0x1c0)&&(addr<=0x1ff))      // memory module 4
    return(MemModules>=4);
  // void: 200
  if ((addr>=0x201)&&(addr<=0x2ef))      // extended memory 1 - 239 regs
    return(XMemModules>=2);
  // void: 2f0-300
  if ((addr>=0x301)&&(addr<=0x3ef))      // extended memory 2 - 239 regs
    return(XMemModules>=3);
  // void: 3f0-3ff
  // end of memory: 3ff
  return(0);
  }


/*****************************/
// timer for execution loop
/****************************/
void Chp41::ExecuteProc() {
#if 0
    for (int i=0;i<pThis->ProcCycles;i++)
    {
        if (pThis->GetSoundMode()==eSoundWave)                              // if wave mode, put must be called continuously to minimize clicks
        {
            static int SkipSound=0;
            if (SkipSound)
                SkipSound--;
            else if (WaveSound.Put(pThis->F_REG)==-1)                         // returns -1 if failed to open
                SkipSound=10000;                                                // skip sound for a while because Put() slows way down while waiting for sound board to free up
        }
        else if (pThis->IsSleeping())
            break;
        if (pThis->GetSoundMode()==eSoundSpeaker || (pThis->TMR_S[2]&0x04)) // override ProcInterval to slow down execution to match real hardware
        {
            QueryPerformanceCounter(&pThis->PCCount[0]);	                    // get current count
            pThis->Execute();                                                 // execute one instruction
            pThis->PCCount[0].QuadPart+=pThis->InstrNSec;
            do
                QueryPerformanceCounter(&pThis->PCCount[1]);
            while (pThis->PCCount[1].QuadPart < pThis->PCCount[0].QuadPart);  // delay until exact amount of time for real machine cycle has passed
        }
        else if (!pThis->IsSleeping())                                      // dont call execute while sleeping because it will execute the next instruction anyway
            pThis->Execute();                                                 // execute one instruction
        if (pThis->Boost && i+pThis->Boost>=pThis->ProcCycles)              // boost this loop for a few more cycles
        {                                                                 // to prevent it from exiting in the middle of heavy display operations
            i-=pThis->Boost;
            pThis->Boost=0;
        }
        if (pThis->fBreak)                                                  // breakpoint has been raised
        {
            theApp.pMainWnd->OpenConsole();
            theApp.pMainWnd->OpenBreakpoints();
            pThis->fBreak=false;
            break;
        }
    }
    if (pThis->Indicator)
        theApp.m_pMainWnd->InvalidateRect(&pThis->RectIndicator,false);     // activity indicator
    if (pThis->UpdateDisplay)
        theApp.m_pMainWnd->InvalidateRect(pThis->pRectLCD,false);           // refresh LCD
    if (pThis->UpdateAnnun)
        theApp.m_pMainWnd->InvalidateRect(pThis->pRectAnnun,false);         // refresh Annunciator
#endif
}



/****************************/
// executes intelligent printer instructions
/****************************/
#define PRINTER_TRACE       (1 << 15)
#define PRINTER_NORM        (1 << 14)
#define PRINTER_PRINTDOWN   (1 << 13)
#define PRINTER_ADVDOWN     (1 << 12)
#define PRINTER_OUTPAPER    (1 << 11)
#define PRINTER_BATLOW      (1 << 10)
#define PRINTER_IDLE        (1 << 9)
#define PRINTER_BUFEMPTY    (1 << 8)
#define PRINTER_LOWERCASE   (1 << 7)
#define PRINTER_GRAPHIC     (1 << 6)
#define PRINTER_DOUBLE      (1 << 5)
#define PRINTER_RIGHTJUST   (1 << 4)
#define PRINTER_EOLSENT     (1 << 3)
#define PRINTER_NOADV       (1 << 2)
#define PRINTER_NOTUSED_1   (1 << 1)
#define PRINTER_NOTUSED_0   (1 << 0)

void Chp41::exec_perph_printer(void)
  {
  hp41cpu->r->CARRY=0;
//  qWarning()<<"exec_perph_printer";
  if (PageMatrix[6][0]==NULL)  // if no printer ROM
    return;
//  qWarning()<<QString("exec_perph_printer:%1").arg(hp41cpu->Tyte1,2,16,QChar('0'));

  switch(hp41cpu->Tyte1)
    {
    case 0x003:       /* ?XF 0 or BUSY? */
      {
      hp41cpu->r->CARRY = 0;
      break;
      }
    case 0x083:       /* ?XF 2 or ERROR? */
      {
      hp41cpu->r->CARRY = 1;
      break;
      }
    case 0x043:       /* ?XF 1 or POWON? */
      {
      hp41cpu->r->CARRY = 1;
      break;
      }
    case 0x007:       /* PRshort or BUF=BUF+C */
      {
      qWarning() << QString("%1").arg(((hp41cpu->r->C_REG[1] << 4) | hp41cpu->r->C_REG[0] ),2,16,QChar('0'));
      break;
      }
    case 0x03a:       /* RPREG 0 or C=STATUS */
      {
      UINT16 PRINT_STATUS= PRINTER_IDLE | PRINTER_BUFEMPTY; // | 0x3;
      memset(hp41cpu->r->C_REG,0,sizeof(hp41cpu->r->C_REG));
      hp41cpu->r->C_REG[13]=(byte)((PRINT_STATUS&0xf000)>>12);
      hp41cpu->r->C_REG[12]=(byte)((PRINT_STATUS&0x0f00)>>8);
      hp41cpu->r->C_REG[11]=(byte)((PRINT_STATUS&0x00f0)>>4);
      hp41cpu->r->C_REG[10]=(byte)(PRINT_STATUS&0x000f);
      break;
      }
    case 0x005:       /* RTNCPU or WPREG 01, RTN */
      {
      break;
      }
    default:
      {
      LOG(30);
      break;
      }
    }
  }





/****************************/
// _inp() and _outp() do not work on WinNT
// by Zharkoi Oleg
/****************************/
void Chp41::Speaker(short Freq, int Duration)
{
#if 0
    ulong Count;
    byte off,on;
    byte spk;

    spk=_inp(0x61);      // get current status
    off=spk&0xFC;
    on=spk|0x03;         // turn on bits 0 and 1

    if (Freq!=0)
    {
        Count=(unsigned long)(PCPerf.QuadPart/Freq);  // determine the timer frequency
        if (!(spk&0x03))
        {
            _outp(0x61,on);          // turn on the speaker
            _outp(0x43,0xB6);        // set up the timer
        }
        _outp(0x42,Count&0xff);
        _outp(0x42,Count>>8);
    }
    else
    {
        _outp(0x61,off);           // turn off the speaker
    }
#endif
}


void Chp41::SetKeyDown(byte KeyCode) {

//  if (eKeyboard==eKeyboardNone)
//    return;
    if (pKEYB->LastKey)
        fEnableCLRKEY=false;           // disable CLRKEY instruction
    else {
        fEnableCLRKEY=true;
        MinCLRKEY=3;                   // the key will be held down for this minimum number of CLRKEY instructions to avoid debounce code
    }
  hp41cpu->r->KEY_REG= KeyCode ? KeyCode : getKey();
  if (hp41cpu->r->KEY_REG) {
//      qWarning()<<"Key Pressed:"<<hp41cpu->r->KEY_REG;
      hp41cpu->r->KEYDOWN=true;
  }
//  if ( (eLightSleep==IsSleeping()) ||                 // light sleep and any key pressed or
//    ((eDeepSleep==IsSleeping())&&(KeyCode==0x18)) )   // deep sleep and ON key pressed
//    Wakeup();


  }

/**********************************/
// Simulates a key release
/**********************************/
void Chp41::SetKeyUp()
  {
//  if (eKeyboard==eKeyboardNone)
//    return;
  fEnableCLRKEY=true;
  }

#define KEY(c)	( pKEYB->keyPressedList.contains(TOUPPER(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(TOLOWER(c)))

UINT8 Chp41::getKey()
{
    UINT8 code = 0;
    if (pKEYB->LastKey)
    {
        if (KEY(K_OF))			code = 0x18;
        if (KEY(K_F1))			code = 0x18;
        if (KEY(K_F2))			code = 0xC6;
        if (KEY(K_F3))			code = 0xC5;
        if (KEY(K_F4))			code = 0xC4;

        if (KEY('A'))			code = 0x10;
        if (KEY('B'))			code = 0x30;
        if (KEY('C'))			code = 0x70;
        if (KEY('D'))			code = 0x80;
        if (KEY('E'))			code = 0xC0;
        if (KEY('F'))			code = 0x11;
        if (KEY('G'))			code = 0x31;
        if (KEY('H'))			code = 0x71;
        if (KEY('I'))			code = 0x81;
        if (KEY('J'))			code = 0xC1;
        if (KEY(K_SHT))			code = 0x12;
        if (KEY('K'))			code = 0x32;
        if (KEY('L'))			code = 0x72;
        if (KEY('M'))			code = 0x82;
        if (KEY(K_SST))			code = 0xC2;
        if (KEY('N'))			code = 0x13;
        if (KEY('O'))			code = 0x73;
        if (KEY('P'))			code = 0x83;
        if (KEY(K_LA))			code = 0xC3;
        if (KEY('Q'))			code = 0x14;
        if (KEY('R'))			code = 0x34;
        if (KEY('S'))			code = 0x74;
        if (KEY('T'))			code = 0x84;
        if (KEY('U'))			code = 0x15;
        if (KEY('V'))			code = 0x35;
        if (KEY('W'))			code = 0x75;
        if (KEY('X'))			code = 0x85;
        if (KEY('Y'))			code = 0x16;
        if (KEY('Z'))			code = 0x36;
        if (KEY('='))			code = 0x76;
        if (KEY('?'))			code = 0x86;

        if (KEY(':'))			code = 0x17;
        if (KEY(' '))			code = 0x37;
        if (KEY('.'))			code = 0x77;
        if (KEY(K_RS))			code = 0x87;

    }

    return code;

}
#if 0
byte Chp41::MapKeyToKey(
  flag fAlt,                  // true if this is an alt key
  flag fExtended,             // true if this is an extended PC keycode
  int PCKey)                  // PC keycode
  {
  if (fAlt)
    return(0);

  if (fExtended)
    {
    switch (PCKey)
      {
      case VK_ENTER:
        return(0x13);
      case VK_DIVIDE:
        return(0x17);
      }
    return(0);
    }

  if ( (PCKey>=VK_A) && (PCKey<=VK_Z) )
    return(UnshiftAlphaTable[PCKey-VK_A]);

  switch (PCKey)
    {
    case VK_F1:
      return(0);        // HELP
    case VK_F2:
      return(0x18);     // ON
    case VK_F3:
      return(0xc6);     // USER
    case VK_F4:
      return(0xc5);     // PRGM
    case VK_F5:
      return(0xc4);     // ALPHA
    case VK_F6:
      return(0xc2);     // SST
    case VK_F7:
      return(0x87);     // R/S
    case VK_F8:
    case VK_F9:
    case VK_F10:
    case VK_F11:
    case VK_F12:
      return(0);
    case VK_SHIFT:
      return(0x12);     // SHIFT
    case VK_CONTROL:
      return(1);        // COPY LCD
    case VK_TAB:
      return(3);        // TURBO
    case VK_BACK:
    case VK_ESCAPE:
      return(0xc3);     // BACKARROW
    case VK_OEM_MINUS:  // -_ KEY
    case VK_SUBTRACT:
      return(0x14);
    case VK_OEM_PLUS:   // += KEY
    case VK_ADD:
      return(0x15);
    case VK_MULTIPLY:
    case VK_OEM_QUOTE:  // '"
      return(0x16);
    case VK_OEM_2:      // /? KEY
    case VK_DIVIDE:
      return(0x17);
    case VK_0:
    case VK_NUMPAD0:
    case VK_INSERT:
      return(0x37);
    case VK_1:
    case VK_NUMPAD1:
    case VK_END:
      return(0x36);
    case VK_2:
    case VK_NUMPAD2:
    case VK_DOWN:
      return(0x76);
    case VK_3:
    case VK_NUMPAD3:
    case VK_NEXT:
      return(0x86);
    case VK_4:
    case VK_NUMPAD4:
    case VK_LEFT:
      return(0x35);
    case VK_5:
    case VK_NUMPAD5:
    case VK_CLEAR:
      return(0x75);
    case VK_6:
    case VK_NUMPAD6:
    case VK_RIGHT:
      return(0x85);
    case VK_7:
    case VK_NUMPAD7:
    case VK_HOME:
      return(0x34);
    case VK_8:
    case VK_NUMPAD8:
    case VK_UP:
      return(0x74);
    case VK_9:
    case VK_NUMPAD9:
    case VK_PRIOR:
      return(0x84);
    case VK_DELETE:
    case VK_OEM_PERIOD:
    case VK_DECIMAL:
    case VK_OEM_COMMA:
      return(0x77);
    case VK_SPACE:
      return(0x37);
    case VK_ENTER:
      return(0x13);
    }
  return(0);
  }
#endif

#endif
