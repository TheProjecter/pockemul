
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
#include "bus.h"

#define DOWN	0
#define UP		1



Chp82143A::Chp82143A(CPObject *parent):Cprinter(this) {

    setcfgfname(QString("hp82143a"));
    BackGroundFname	= P_RES(":/hp41/hp82143a.png");

    pKEYB = new Ckeyb(this,"hp82143a.map");

    setDXmm(180);
    setDYmm(130);
    setDZmm(60);

    setDX(643);
    setDY(464);


    margin = 40;
    paperWidth = 640;
    setPaperPos(QRect(90,26,400,300));
}

Chp82143A::~Chp82143A() {
}

bool Chp82143A::init(void) {

    Cprinter::init();

    charTable = new QImage(P_RES(":/ext/ce126ptable.bmp"));
    charsize = 2;

    pCONNECTOR = new Cconnector(this,64,0,Cconnector::hp41,"hp41 Module",true,QPoint(631,468));
    publish(pCONNECTOR);

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();

    // Create CE-126 Paper Image
    // The final paper image is 207 x 149 at (277,0) for the ce125
    printerbuf	= new QImage(QSize(paperWidth, 3000),QImage::Format_ARGB32);
    printerdisplay= new QImage(QSize(paperWidth, 149),QImage::Format_ARGB32);

    paperWidget = new CpaperWidget(PaperPos(),printerbuf,this);
    paperWidget->hide();
//    paperWidget->show();
//    paperWidget->hide();

    // Fill it blank
    clearPaper();

    settop(10);
    setposX(0);
    return true;
}

void Chp82143A::clearPaper(void)
{
    // Fill it blank
    printerbuf->fill(PaperColor.rgba());
    printerdisplay->fill(QColor(255,255,255,0).rgba());
    settop(10);
    setposX(0);
    // empty TextBuffer
    TextBuffer.clear();
    paperWidget->updated = true;
}

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

quint16 Chp82143A::getStatus(void) {

    return PRINTER_IDLE | PRINTER_BUFEMPTY;
}

bool Chp82143A::run(void) {

    Cbus bus;

    bus.fromUInt64(pCONNECTOR->Get_values());

    switch (bus.getFunc()) {
    case BUS_SLEEP: break;
    case BUS_READDATA:
        if (bus.getAddr()==0x3a) {
            bus.setData(getStatus() >> 8);
            bus.setFunc(BUS_READDATA);
        }
        if (bus.getAddr()==0x3b) {
            bus.setData(getStatus() & 0xff);
            bus.setFunc(BUS_READDATA);
        }
        break;
    case BUS_WRITEDATA: Printer(bus.getData());
        bus.setFunc(BUS_SLEEP);
        break;
    }
    pCONNECTOR->Set_values(bus.toUInt64());
    return true;
}

bool Chp82143A::exit(void) {

    Cprinter::exit();

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


    qWarning()<<"Received : "<<data;
    static char buffer[82];     /* line buffer */
    static int buflen=0;        /* line len */
    static int flow=0;          /* flag lowercase */
    static int fdwid=0;         /* flag double width */
    int i, j;



    if (data>127) {
      /* control char. */
      switch (data) {
      case 224:
        /* print to left if buffer NOT empty */
        if (!TextBuffer.isEmpty()) {
          TextBuffer.append(13).append(10);
          printLine();    /* print buffer on display device */
          TextBuffer.clear();             /* and clear buffer */
          /* reset flags */
          flow=0;
          fdwid=0;
        }
        break;
      case 232:
          /* print to right */
          qWarning()<<"Print to Right";
          TextBuffer = TextBuffer.rightJustified(32,' ',true).append(13).append(10);
          printLine();
          TextBuffer.clear();
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
        if ((data>160)&&(data<192)) {
          /* accumulate spaces */
          for (i=160;i<data;i++) {
            if (TextBuffer.size()<70) {
              TextBuffer.append(' ');
              if (fdwid)
                TextBuffer.append(' ');
            }
          }
        }
        else {
          /* unknown code: print debug info */
          if (buflen<70) {
            TextBuffer.append('\\');
            TextBuffer.append('0'+(data/100));
            TextBuffer.append('0'+((data/10)%10));
            TextBuffer.append('0'+(data%10));
          }
        }
      } /* endswitch */
    }

    else {
      /* normal char. */
      if (flow)
        data=tolower(data);
      /* convert 41 special char to ASCII */
      switch (data) {
        case   0: data='*'; break;
        case  12: data='u'; break;
        case  29: data='#'; break;
        case 124: data='a'; break;  /* angle */
        case 126: data='s'; break;  /* sigma */
        case 127: data='`'; break;  /* append sign */
        default:
         if ((data<32)||(data>127))  data='.';
      }
      if (TextBuffer.size()<70) {
        TextBuffer.append(data);
        if (fdwid)
          TextBuffer.append(' ');
      }
    }


}

void Chp82143A::printLine() {
    QPainter painter;
    qWarning()<<"Print:"<<TextBuffer;
    for (int i=0; i< TextBuffer.size();i++) {
        quint8 data = TextBuffer.at(i);
        if (data == 0x0d){
            top+=10*2;//charsize;;

            //        qWarning()<<"CR PRINTED";
        }
        else if (data == 0x0a){

            setposX(0);

            //        qWarning()<<"CR PRINTED";
        }
        else
        {

            int x = ((data>>4) & 0x0F)*6;
            int y = (data & 0x0F) * 8;
            painter.begin(printerbuf);
            painter.drawImage(	QRectF( margin + (7 * posX*charsize),top,5*charsize,7*2/*charsize*/),
                                *charTable,
                                QRectF( x , y , 5,7));
            painter.end();
            posX++;

        }

        painter.begin(printerdisplay);

        painter.drawImage(QRectF(0,MAX(149-top,0),paperWidth/charsize,MIN(top,149)),
                          *printerbuf,
                          QRectF(0,MAX(0,top-149),paperWidth/charsize,MIN(top,149)));

        painter.end();
        // Draw printer head
        //    painter.fillRect(QRect(0 , 147,207,2),QBrush(QColor(0,0,0)));
        //    painter.fillRect(QRect(21 + (7 * posX) , 147,14,2),QBrush(QColor(255,255,255)));

    }


    Refresh_Display = true;

    paperWidget->setOffset(QPoint(0,top));
    paperWidget->updated = true;

}

bool Chp82143A::UpdateFinalImage(void) {

    Cprinter::UpdateFinalImage();

    QPainter painter;
    painter.begin(FinalImage);

    float ratio = ( (float) paperWidget->width() ) / ( paperWidget->bufferImage->width() - paperWidget->getOffset().x() );

//    ratio *= charsize;
    QRect source = QRect( QPoint(paperWidget->getOffset().x() ,
                                 paperWidget->getOffset().y()  - paperWidget->height() / ratio ) ,
                          QPoint(paperWidget->bufferImage->width(),
                                 paperWidget->getOffset().y() +10)
                          );
//    MSG_ERROR(QString("%1 - %2").arg(source.width()).arg(PaperPos().width()));
    painter.drawImage(PaperPos(),
                      paperWidget->bufferImage->copy(source).scaled(PaperPos().size(),Qt::IgnoreAspectRatio, Qt::SmoothTransformation )
                      );

//    painter.end();

//    QPainter painter;
//    painter.begin(FinalImage);
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
