
//TODO: Key management
#include <QDebug>

#include <QPainter>

#include "hp82143A.h"
#include "Connect.h"
#include "Keyb.h"
#include "dialogconsole.h"
#include "init.h"
#include "Inter.h"
#include "Log.h"
#include "paperwidget.h"

#define DOWN	0
#define UP		1

/*
TransMap KeyMapce140p[]={
    {1,	"FEED  ",	K_PFEED,34,234,	9},
    {2, "PRINTER ON",K_PRINT_ON,34,200,9},
    {3, "SIO ON",K_PRINT_OFF,35,150,9}
};
 int KeyMapce140pLenght = 3;
*/

Chp82143A::Chp82143A(CPObject *parent):CprinterCtronics(this) {

    setcfgfname(QString("hp82143A"));
    BackGroundFname	= P_RES(":/hp41/hp82143a.png");

    delete pKEYB; pKEYB		= new Ckeyb(this,"hp82143A.map");

    setDXmm(210);//Pc_DX_mm = 256;
    setDYmm(145);//Pc_DY_mm = 185;
    setDZmm(42);//Pc_DZ_mm = 42;

    setDX(750);//Pc_DX	= 895;
    setDY(518);//Pc_DY	= 615;


    margin = 40;
    paperWidth = 640;
    setPaperPos(QRect(90,26,400,300));
}

Chp82143A::~Chp82143A() {
}

bool Chp82143A::init(void) {

    charTable = new QImage(P_RES(":/ext/ce126ptable.bmp"));
    charsize = 2;

    CprinterCtronics::init();

//    qWarning()<<"init done";
    return true;
}

bool Chp82143A::run(void) {

    CprinterCtronics::run();

    return true;
}

bool Chp82143A::exit(void) {

    CprinterCtronics::exit();

    return true;
}


void Chp82143A::ComputeKey(void)
{
    if (pKEYB->LastKey == K_PFEED) {
        Printer(0x0d);
    }
    if (pKEYB->LastKey == K_PRT_COND) {
        if (charsize==1) charsize=2;
        else charsize = 1;
        update();
    }
//    if (pKEYB->LastKey == K_PRINT_ON) {
//        printerSwitch = true;
//    }
//    if (pKEYB->LastKey == K_PRINT_OFF) {
//        printerSwitch = false;
//    }
}

void Chp82143A::Printer(quint8 data) {
    QPainter painter;


    if (data == 0x0d){
        top+=10*2;//charsize;
        setposX(0);
        TextBuffer += data;
//        qWarning()<<"CR PRINTED";
    }
    else
    {
        if (posX>=(80/charsize)) {
            top+=10*2;//charsize;
            setposX(0);
            TextBuffer += 0x0d;
        }
//        qWarning()<<"CHAR PRINTED:"<<QChar(data);
        TextBuffer += data;
        painter.begin(printerbuf);
        int x = ((data>>4) & 0x0F)*6;
        int y = (data & 0x0F) * 8;
        painter.drawImage(	QRectF( margin + (7 * posX*charsize),top,5*charsize,7*2/*charsize*/),
                            *charTable,
                            QRectF( x , y , 5,7));
        posX++;
        painter.end();
    }


    painter.begin(printerdisplay);

    painter.drawImage(QRectF(0,MAX(149-top,0),paperWidth/charsize,MIN(top,149)),
                      *printerbuf,
                      QRectF(0,MAX(0,top-149),paperWidth/charsize,MIN(top,149)));

// Draw printer head
//    painter.fillRect(QRect(0 , 147,207,2),QBrush(QColor(0,0,0)));
//    painter.fillRect(QRect(21 + (7 * posX) , 147,14,2),QBrush(QColor(255,255,255)));



    painter.end();

    Refresh_Display = true;

    paperWidget->setOffset(QPoint(0,top));
    paperWidget->updated = true;


}

bool Chp82143A::UpdateFinalImage(void) {

    CprinterCtronics::UpdateFinalImage();

    QPainter painter;
    painter.begin(FinalImage);
    painter.drawImage(650,280,BackgroundImageBackup->copy(650,280,33,60).mirrored(false,charsize==1?false:true));
    painter.end();

    return true;
}




#if 0



/* ****************************************** */
/* Emu41: HP-41C software emulator for PC     */
/*                                            */
/* printer.c   82143 printer simulator module */
/*                                            */
/* Version 2.5, J-F Garnier 1997-2007         */
/* ****************************************** */

/* Conditional compilation:
  none
*/

#include <ctype.h>

/* commun variables to all modules */
#define GLOBAL extern




/* ****************************************** */
/* print_char(int n)                          */
/*                                            */
/* send one character to the HP82143 buffer   */
/* actual printing to the screen emulating    */
/* the HP82143 is done by print_str(buffer)   */
/* ****************************************** */
void print_char(int n)
{
  static char buffer[82];     /* line buffer */
  static int buflen=0;        /* line len */
  static int flow=0;          /* flag lowercase */
  static int fdwid=0;         /* flag double width */
  int i, j;

  n &= 255;                   /* security */

  if (n>127) {
    /* control char. */
    switch (n) {
    case 224:
      /* print to left if buffer NOT empty */
      if (buflen) {
        buffer[buflen++]=13;
        buffer[buflen++]=10;
        buffer[buflen]=0;
        print_str(buffer);    /* print buffer on display device */
        buflen=0;             /* and clear buffer */
        /* reset flags */
        flow=0;
        fdwid=0;
      }
      break;
    case 232:
      /* print to right */
      if (buflen>31) buflen=31;
      /* insert spaces into head */
      for (i=(buflen-1),j=30;i>=0;i--,j--)
        buffer[j]=buffer[i];
      for (;j>=0;j--)
        buffer[j]=' ';
      buflen=31;
      buffer[buflen++]=13;
      buffer[buflen++]=10;
      buffer[buflen]=0;
      print_str(buffer);
      buflen=0;
      /* reset flags */
      flow=0;
      fdwid=0;
      break;
    case 209:
      /* lowercase */
      flow=1;
      break;
    case 212:
      /* double width */
      fdwid=1;
      break;
    case 213:
      /* lower case & double width */
      flow=1;
      fdwid=1;
      break;
    case 255:
      /* sometime sent after ADV, don't know why, ignore it */
      break;
    default:
      if ((n>160)&&(n<192)) {
        /* accumulate spaces */
        for (i=160;i<n;i++) {
          if (buflen<70) {
            buffer[buflen++]=' ';
            if (fdwid)
              buffer[buflen++]=' ';
          }
        }
      }
      else {
        /* unknown code: print debug info */
        if (buflen<70) {
          buffer[buflen++]='\\';
          buffer[buflen++]='0'+(n/100);
          buffer[buflen++]='0'+((n/10)%10);
          buffer[buflen++]='0'+(n%10);
        }
      }
    } /* endswitch */
  }

  else {
    /* normal char. */
    if (flow)
      n=tolower(n);
    /* convert 41 special char to ASCII */
    switch (n) {
      case   0: n='*'; break;
      case  12: n='u'; break;
      case  29: n='#'; break;
      case 124: n='a'; break;  /* angle */
      case 126: n='s'; break;  /* sigma */
      case 127: n='`'; break;  /* append sign */
      default:
       if ((n<32)||(n>127))  n='.';
    }
    if (buflen<70) {
      buffer[buflen++]=n;
      if (fdwid)
        buffer[buflen++]=' ';
    }
  }
}





/* ****************************************** */
/* set_mode_printer()                         */
/*                                            */
/* set the HP82143 printer mode:              */
/*   0:MAN -> 1:NOR -> 2:TRA -> 3:OFF         */
/*  -1 means not plugged                      */
/* ****************************************** */
void set_mode_printer(void)
{
  if (mode_printer>=0) {
    mode_printer++;
    if (mode_printer>3) mode_printer=0;
    flagPrter=1;
  }
}

/* ****************************************** */
/* set_prx_printer()                          */
/*                                            */
/* set the flag for pressed PRINT key         */
/* ****************************************** */
void set_prx_printer(void)
{
  if ((mode_printer>=0)&&(mode_printer<=2)) {
    flagPrx=4;
    flagPrter=1;
  }
}

/* ****************************************** */
/* set_adv_printer()                          */
/*                                            */
/* set the flag for pressed ADVANCE key       */
/* ****************************************** */
void set_adv_printer(void)
{
  if ((mode_printer>=0)&&(mode_printer<=2)) {
    flagAdv=4;
    flagPrter=1;
  }
}

/* ****************************************** */
/* get_printer_status(char *regC)             */
/*                                            */
/* get the status word from the HP82143       */
/* ****************************************** */
int get_printer_status(void)
{
  int n;

  n=0;
  if ((mode_printer>=0)&&(mode_printer<=2)) {
    /* printer is on, return status */
    n=mode_printer<<14;
    if (flagPrx) n |= 1<<13;
    if (flagAdv) n |= 1<<12;
    n |= 3<<8;  /* idle, buffer empty */
    n |= 3;     /* I don't remember why ... */
    if (flagPrx) flagPrx--;
    if (flagAdv) flagAdv--;
    flagPrter=0;
  }
  return(n);
}


/* ****************************************** */
/* test_printer_flag(int n)                   */
/*                                            */
/* test a 82143 flag                          */
/* ****************************************** */
int test_printer_flag(int n)
{
  int f;

  if ((mode_printer>=0)&&(mode_printer<=2)) {
    /* printer is on, return flag */
    switch (n) {
      case 0: f=0; break;   /* BUSY? */
      case 1: f=1; break;   /* POWON? */
      case 2: f=flagPrter; f=1; break;  /* data to be read (forced to 1) */
      /* (this is not consistent with the NPIC description, but it works...) */
      default: f=0;
    }
  }
  else
    f=0;

  return(f);
}

#endif
