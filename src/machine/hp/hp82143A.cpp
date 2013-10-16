
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

quint8 hp82143a_chargen [128][5]= {
  [0x00] = { 0x08, 0x1c, 0x3e, 0x1c, 0x08 },  // undefined - use diamond
  [0x01] = { 0x00, 0x14, 0x08, 0x14, 0x00 },  // small x
  [0x02] = { 0x44, 0x29, 0x11, 0x29, 0x44 },  // x-bar
  [0x03] = { 0x08, 0x1c, 0x2a, 0x08, 0x08 },  // left arrow
  [0x04] = { 0x38, 0x44, 0x44, 0x38, 0x44 },  // LC alpha
  [0x05] = { 0x7e, 0x15, 0x25, 0x25, 0x1a },  // UC beta
  [0x06] = { 0x7f, 0x01, 0x01, 0x01, 0x03 },  // UC gamma
  [0x07] = { 0x10, 0x30, 0x7f, 0x30, 0x10 },  // down arrow
  [0x08] = { 0x60, 0x18, 0x06, 0x18, 0x60 },  // UC delta
  [0x09] = { 0x38, 0x44, 0x44, 0x3c, 0x04 },  // LC sigma
  [0x0a] = { 0x08, 0x1c, 0x3e, 0x1c, 0x08 },  // diamond
  [0x0b] = { 0x62, 0x14, 0x08, 0x10, 0x60 },  // LC lambda
  [0x0c] = { 0x40, 0x3c, 0x20, 0x20, 0x1c },  // LC mu
  [0x0d] = { 0x60, 0x50, 0x58, 0x64, 0x42 },  // angle
  [0x0e] = { 0x10, 0x18, 0x78, 0x04, 0x02 },  // LC tau
  [0x0f] = { 0x08, 0x55, 0x77, 0x55, 0x08 },  // UC phi
  [0x10] = { 0x3e, 0x49, 0x49, 0x49, 0x3e },  // UC theta
  [0x11] = { 0x5e, 0x61, 0x01, 0x61, 0x5e },  // UC omega
  [0x12] = { 0x30, 0x4a, 0x4d, 0x49, 0x30 },  // LC delta
  [0x13] = { 0x78, 0x14, 0x15, 0x14, 0x78 },  // UC A dot
  [0x14] = { 0x38, 0x44, 0x45, 0x3e, 0x44 },  // LC a dot
  [0x15] = { 0x78, 0x15, 0x14, 0x15, 0x78 },  // UC A umlaut
  [0x16] = { 0x38, 0x45, 0x44, 0x7d, 0x40 },  // LC a umlaut
  [0x17] = { 0x3c, 0x43, 0x42, 0x43, 0x3c },  // UC O umlaut
  [0x18] = { 0x38, 0x45, 0x44, 0x45, 0x38 },  // LC o umlaut
  [0x19] = { 0x3e, 0x41, 0x40, 0x41, 0x3e },  // UC U umlaut
  [0x1a] = { 0x3c, 0x41, 0x40, 0x41, 0x3c },  // LC u umlaut
  [0x1b] = { 0x7e, 0x09, 0x7f, 0x49, 0x49 },  // UC AE
  [0x1c] = { 0x38, 0x44, 0x38, 0x54, 0x58 },  // LC ae
  [0x1d] = { 0x14, 0x34, 0x1c, 0x16, 0x14 },  // not equal
  [0x1e] = { 0x48, 0x7e, 0x49, 0x41, 0x22 },  // pound sterling
  [0x1f] = { 0x55, 0x2a, 0x55, 0x2a, 0x55 },  // ?
  [0x20] = { 0x00, 0x00, 0x00, 0x00, 0x00 },  // space
  [0x21] = { 0x00, 0x00, 0x5f, 0x00, 0x00 },  // bang
  [0x22] = { 0x00, 0x03, 0x00, 0x03, 0x00 },  // double quote
  [0x23] = { 0x14, 0x7f, 0x14, 0x7f, 0x14 },  // hash (pound, octothorpe)
  [0x24] = { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },  // dollar
  [0x25] = { 0x23, 0x13, 0x08, 0x64, 0x62 },  // percent
  [0x26] = { 0x36, 0x49, 0x56, 0x20, 0x50 },  // ampersand
  [0x27] = { 0x00, 0x00, 0x03, 0x00, 0x00 },  // single quote
  [0x28] = { 0x00, 0x1c, 0x22, 0x41, 0x00 },  // left parenthesis
  [0x29] = { 0x00, 0x41, 0x22, 0x1c, 0x00 },  // right parenthesis
  [0x2a] = { 0x14, 0x08, 0x3e, 0x08, 0x14 },  // asterisk
  [0x2b] = { 0x08, 0x08, 0x3e, 0x08, 0x08 },  // plus
  [0x2c] = { 0x00, 0x40, 0x30, 0x00, 0x00 },  // comma
  [0x2d] = { 0x08, 0x08, 0x08, 0x08, 0x08 },  // hyphen
  [0x2e] = { 0x00, 0x60, 0x60, 0x00, 0x00 },  // period
  [0x2f] = { 0x20, 0x10, 0x08, 0x04, 0x02 },  // slash
  [0x30] = { 0x3e, 0x51, 0x49, 0x45, 0x3e },  // zero
  [0x31] = { 0x00, 0x42, 0x7e, 0x40, 0x00 },  // one
  [0x32] = { 0x62, 0x51, 0x49, 0x49, 0x46 },  // two
  [0x33] = { 0x21, 0x41, 0x49, 0x4d, 0x33 },  // three
  [0x34] = { 0x18, 0x14, 0x12, 0x7f, 0x10 },  // four
  [0x35] = { 0x27, 0x45, 0x45, 0x45, 0x39 },  // five
  [0x36] = { 0x3c, 0x4a, 0x49, 0x48, 0x30 },  // six
  [0x37] = { 0x01, 0x71, 0x09, 0x05, 0x03 },  // seven
  [0x38] = { 0x36, 0x49, 0x49, 0x49, 0x36 },  // eight
  [0x39] = { 0x06, 0x49, 0x49, 0x29, 0x1e },  // nine
  [0x3a] = { 0x00, 0x00, 0x24, 0x00, 0x00 },  // colon
  [0x3b] = { 0x00, 0x40, 0x34, 0x00, 0x00 },  // semicolon
  [0x3c] = { 0x08, 0x14, 0x22, 0x41, 0x00 },  // less than
  [0x3d] = { 0x14, 0x14, 0x14, 0x14, 0x14 },  // equal
  [0x3e] = { 0x00, 0x41, 0x22, 0x14, 0x08 },  // greater than
  [0x3f] = { 0x02, 0x01, 0x51, 0x09, 0x06 },  // question mark
  [0x40] = { 0x3e, 0x41, 0x5d, 0x5d, 0x1e },  // at
  [0x41] = { 0x7e, 0x11, 0x11, 0x11, 0x7e },  // UC A
  [0x42] = { 0x7f, 0x49, 0x49, 0x49, 0x36 },  // UC B
  [0x43] = { 0x3e, 0x41, 0x41, 0x41, 0x22 },  // UC C
  [0x44] = { 0x41, 0x7f, 0x41, 0x41, 0x3e },  // UC D
  [0x45] = { 0x7f, 0x49, 0x49, 0x49, 0x41 },  // UC E
  [0x46] = { 0x7f, 0x09, 0x09, 0x09, 0x01 },  // UC F
  [0x47] = { 0x3e, 0x41, 0x41, 0x51, 0x72 },  // UC G
  [0x48] = { 0x7f, 0x08, 0x08, 0x08, 0x7f },  // UC H
  [0x49] = { 0x00, 0x41, 0x7f, 0x41, 0x00 },  // UC I
  [0x4a] = { 0x20, 0x40, 0x40, 0xef, 0x00 },  // UC J
  [0x4b] = { 0x7f, 0x08, 0x14, 0x22, 0x41 },  // UC K
  [0x4c] = { 0x7f, 0x40, 0x40, 0x40, 0x40 },  // UC L
  [0x4d] = { 0x7f, 0x02, 0x0c, 0x02, 0x7f },  // UC M
  [0x4e] = { 0x7f, 0x04, 0x08, 0x10, 0x7f },  // UC N
  [0x4f] = { 0x3e, 0x41, 0x41, 0x41, 0x3e },  // UC O
  [0x50] = { 0x7f, 0x09, 0x09, 0x09, 0x06 },  // UC P
  [0x51] = { 0x3e, 0x41, 0x51, 0x21, 0x5e },  // UC Q
  [0x52] = { 0x7f, 0x09, 0x19, 0x29, 0x46 },  // UC R
  [0x53] = { 0x26, 0x49, 0x49, 0x49, 0x32 },  // UC S
  [0x54] = { 0x01, 0x01, 0x7f, 0x01, 0x01 },  // UC T
  [0x55] = { 0x3f, 0x40, 0x40, 0x40, 0x3f },  // UC U
  [0x56] = { 0x07, 0x18, 0x60, 0x18, 0x07 },  // UC V
  [0x57] = { 0x7f, 0x20, 0x18, 0x20, 0x7f },  // UC W
  [0x58] = { 0x63, 0x14, 0x08, 0x14, 0x63 },  // UC X
  [0x59] = { 0x03, 0x04, 0x78, 0x04, 0x03 },  // UC Y
  [0x5a] = { 0x61, 0x51, 0x49, 0x45, 0x43 },  // UC Z
  [0x5b] = { 0x00, 0x7f, 0x41, 0x41, 0x00 },  // left bracket
  [0x5c] = { 0x02, 0x04, 0x08, 0x10, 0x20 },  // backslash
  [0x5d] = { 0x00, 0x41, 0x41, 0x7f, 0x00 },  // right bracket
  [0x5e] = { 0x04, 0x02, 0x7f, 0x02, 0x04 },  // up arrow
  [0x5f] = { 0x40, 0x40, 0x40, 0x40, 0x40 },  // underscore
  [0x60] = { 0x00, 0x01, 0x07, 0x01, 0x00 },  // superscript T
  [0x61] = { 0x20, 0x54, 0x54, 0x54, 0x78 },  // LC a
  [0x62] = { 0x7f, 0x48, 0x44, 0x44, 0x38 },  // LC b
  [0x63] = { 0x38, 0x44, 0x44, 0x44, 0x20 },  // LC c
  [0x64] = { 0x38, 0x44, 0x44, 0x48, 0x7f },  // LC d
  [0x65] = { 0x38, 0x54, 0x54, 0x54, 0x08 },  // LC e
  [0x66] = { 0x08, 0x7c, 0x0a, 0x01, 0x02 },  // LC f
  [0x67] = { 0x08, 0x14, 0x54, 0x54, 0x38 },  // LC g
  [0x68] = { 0x7f, 0x10, 0x08, 0x08, 0x70 },  // LC h
  [0x69] = { 0x00, 0x44, 0x7d, 0x40, 0x00 },  // LC i
  [0x6a] = { 0x20, 0x40, 0x40, 0x3d, 0x00 },  // LC j
  [0x6b] = { 0x00, 0x7f, 0x28, 0x44, 0x00 },  // LC k
  [0x6c] = { 0x00, 0x41, 0x7f, 0x40, 0x00 },  // LC l
  [0x6d] = { 0x78, 0x04, 0x18, 0x04, 0x78 },  // LC m
  [0x6e] = { 0x7c, 0x08, 0x04, 0x04, 0x78 },  // LC n
  [0x6f] = { 0x38, 0x44, 0x44, 0x44, 0x38 },  // LC o
  [0x70] = { 0x7c, 0x14, 0x24, 0x24, 0x18 },  // LC p
  [0x71] = { 0x18, 0x24, 0x24, 0x7c, 0x40 },  // LC q
  [0x72] = { 0x7c, 0x08, 0x04, 0x04, 0x08 },  // LC r
  [0x73] = { 0x48, 0x54, 0x54, 0x54, 0x24 },  // LC s
  [0x74] = { 0x04, 0x3e, 0x44, 0x20, 0x00 },  // LC t
  [0x75] = { 0x3c, 0x40, 0x40, 0x20, 0x7c },  // LC u
  [0x76] = { 0x1c, 0x20, 0x40, 0x20, 0x1c },  // LC v
  [0x77] = { 0x3c, 0x40, 0x30, 0x40, 0x3c },  // LC w
  [0x78] = { 0x44, 0x28, 0x10, 0x28, 0x44 },  // LC x
  [0x79] = { 0x44, 0x28, 0x10, 0x08, 0x04 },  // LC y
  [0x7a] = { 0x44, 0x64, 0x54, 0x4c, 0x44 },  // LC z
  [0x7b] = { 0x08, 0x78, 0x08, 0x78, 0x04 },  // LC pi
  [0x7c] = { 0x00, 0x00, 0x7f, 0x00, 0x00 },  // vertical bar
  [0x7d] = { 0x08, 0x08, 0x2a, 0x1c, 0x08 },  // right arrow
  [0x7e] = { 0x63, 0x55, 0x49, 0x41, 0x63 },  // UC sigma
  [0x7f] = { 0x7f, 0x08, 0x08, 0x08, 0x08 },  // lazy T
};


Chp82143A::Chp82143A(CPObject *parent):Cprinter(this) {

    setcfgfname(QString("hp82143a"));
    BackGroundFname	= P_RES(":/hp41/hp82143a.png");

    pKEYB = new Ckeyb(this,"hp82143a.map");

    setDXmm(180);
    setDYmm(130);
    setDZmm(60);

    setDX(643);
    setDY(464);


    margin = 30;
    paperWidth = 24*7+margin+margin;
    setPaperPos(QRect(355,16,216,250));

    Mode = TRACE_MODE;
    flow = fdwid = fprint = fpadv = false;
}

Chp82143A::~Chp82143A() {
}

bool Chp82143A::init(void) {

    Cprinter::init();

    charTable = new QImage(P_RES(":/ext/ce126ptable.bmp"));
    charsize = 1;

    pCONNECTOR = new Cconnector(this,64,0,Cconnector::hp41,"hp41 Module",true,QPoint(631,468));
    publish(pCONNECTOR);

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();

    // Create  Paper Image
    // The final paper image is 207 x 149 at (277,0) for the ce125
    printerbuf	= new QImage(QSize(paperWidth, 3000),QImage::Format_ARGB32);
    printerdisplay= new QImage(QSize(paperWidth, 250),QImage::Format_ARGB32);

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

#define PRINTER_TRACE       (1 << 15) // if TRACE and NORM are both
#define PRINTER_NORM        (1 << 14) //     zero, the switch is set to MAN
#define PRINTER_PRINTDOWN   (1 << 13) // PRinT KEY is pressed
#define PRINTER_ADVDOWN     (1 << 12) // paper ADVance KEY is pressed
#define PRINTER_OUTPAPER    (1 << 11) // paper out
#define PRINTER_BATLOW      (1 << 10) // low battery
#define PRINTER_IDLE        (1 << 9)  // not printing or advancing paper
#define PRINTER_BUFEMPTY    (1 << 8)  // Buffer Empty
#define PRINTER_LOWERCASE   (1 << 7)  // Lower Case mode
#define PRINTER_GRAPHIC     (1 << 6)  // Graphics mode (bit-mapped columns)
#define PRINTER_DOUBLE      (1 << 5)  // Double Wide mode
#define PRINTER_RIGHTJUST   (1 << 4)  // Type of End of Line is Right Justify
#define PRINTER_EOLSENT     (1 << 3)  // Last byte was End Of Line
#define PRINTER_NOADV       (1 << 2)  // IGNore paper advance key
#define PRINTER_NOTUSED_1   (1 << 1)
#define PRINTER_NOTUSED_0   (1 << 0)

quint16 Chp82143A::getStatus(void) {

    quint16 status=0;

    switch (Mode) {
    case TRACE_MODE: status |= PRINTER_TRACE; break;
    case NORM_MODE: status |= PRINTER_NORM; break;
    }

    if (flow) status |= PRINTER_LOWERCASE;
    if (fdwid) status |= PRINTER_DOUBLE;
    if (fprint) status |= PRINTER_PRINTDOWN;
    if (fpadv) status |= PRINTER_ADVDOWN;

    return status | PRINTER_IDLE | PRINTER_BUFEMPTY;
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

#define KEY(c)	( pKEYB->keyPressedList.contains(TOUPPER(c)) || pKEYB->keyPressedList.contains(c) || pKEYB->keyPressedList.contains(TOLOWER(c)))
void Chp82143A::ComputeKey(void)
{
    if (KEY(K_PFEED)) {
        top+=10;
        fpadv = true;
        update();
    }
    else fpadv = false;

    if (KEY(K_PRINT)) {
        fprint = true;
    }
    else fprint = false;

    if (pKEYB->LastKey == K_PRT_NORM) { Mode = NORM_MODE; update(); }
    if (pKEYB->LastKey == K_PRT_TRACE) { Mode = TRACE_MODE; update(); }
    if (pKEYB->LastKey == K_PRT_MANUAL) { Mode = MANUAL_MODE; update(); }

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
          TextBuffer = TextBuffer.rightJustified(24,' ',true).append(13).append(10);
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
            top+=10;//charsize;;
        }
        else if (data == 0x0a){

            setposX(0);
        }
        else
        {

            int x = ((data>>4) & 0x0F)*6;
            int y = (data & 0x0F) * 8;
            painter.begin(printerbuf);
            painter.setPen("black");

            for (int a = 0; a< 5; a++)
                for (int b=0; b<8;b++)
                {
                    if ( hp82143a_chargen[data][a] & (1<<b) )
                        painter.drawPoint(  margin + (7 * posX*charsize) +a, top+b);
                }
//            painter.drawImage(	QRectF( margin + (7 * posX*charsize),top,5*charsize,7*2/*charsize*/),
//                                *charTable,
//                                QRectF( x , y , 5,7));
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

    // Draw switch
    // MODE SWITCH

    if (Mode == NORM_MODE)
        painter.drawImage(205,385,QImage(P_RES(":/hp41/hp82143a_mode_norm.png")));
    if (Mode == MANUAL_MODE)
        painter.drawImage(205,385,QImage(P_RES(":/hp41/hp82143a_mode_man.png")));
    if (Mode == TRACE_MODE)
        painter.drawImage(205,385,BackgroundImageBackup->copy(205,385,69,24));


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
