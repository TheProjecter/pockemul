/********************************************************************************************************
 * PROGRAM      : test
 * DATE - TIME  : samedi 28 octobre 2006 - 12h40
 * AUTHOR       :  (  )
 * FILENAME     : Ce126.cpp
 * LICENSE      : GPL
 * COMMENTARY   :
 ********************************************************************************************************/
#include <QPainter>
#include <QTime>
#include <QSound>
#include <QFileDialog>
#include <QThread>

#include "common.h"

#include "ce140f.h"
#include "pcxxxx.h"
#include "Log.h"
#include "dialoganalog.h"
#include "paperwidget.h"
#include "Keyb.h"

#define DOWN	0
#define UP		1

TransMap KeyMapce140f[]={
    {1,	"FEED  ",	K_PFEED,34,234,	9},
    {2,	"RMT ON",	K_RMT_ON,34,234,9},
    {3,	"RMT OFF",	K_RMT_OFF,34,234,9},
    {4,	"POWER ON",	K_POW_ON,34,234,	9},
    {5,	"POWER OFF",K_POW_OFF,34,234,	9}
};
int KeyMapce140fLenght = 5;

Cce140f::Cce140f(CPObject *parent):Cprinter(this)
{								//[constructor]
    setfrequency( 0);
    ce140fbuf	= 0;
    ce140fdisplay= 0;
    bells		= 0;
    charTable = 0;
    ToDestroy	= false;
    BackGroundFname	= ":/EXT/ext/ce-140f.png";
    setcfgfname("ce140f");

    settop(10);
    setposX(0);
    pCONNECTOR	= new Cconnector(this,11,0,"Connector 11 pins",true,QPoint(594,238));	publish(pCONNECTOR);
    pTAPECONNECTOR	= new Cconnector(this,3,1,"Line in / Rec / Rmt",false);	publish(pTAPECONNECTOR);
    pTIMER		= new Ctimer(this);
    KeyMap      = KeyMapce140f;
    KeyMapLenght= KeyMapce140fLenght;
    pKEYB		= new Ckeyb(this,"ce140f.map");
    Pc_DX	= 620;
    Pc_DY	= 488;
    SnapPts = QPoint(594,145);

    setPaperPos(QRect(150,-3,207,149));

    stackBehind = true;

    ctrl_char = false;
    t = 0;
    c = 0;
    rmtSwitch = false;
};

void Cce140f::ComputeKey(void)
{
    if (pKEYB->LastKey == K_PFEED) {
        RefreshCe140f(0x0d);
    }
    if (pKEYB->LastKey == K_RMT_ON) {
        rmtSwitch = true;
    }
    if (pKEYB->LastKey == K_RMT_OFF) {
        rmtSwitch = false;
    }
}

//void Cce140f::resizeEvent ( QResizeEvent * ) {
//    float ratio = (float)this->width()/this->Pc_DX ;

//    QRect rect = paperWidget->baseRect;
//    paperWidget->setGeometry( rect.x()*ratio,
//                              rect.y()*ratio,
//                              rect.width()*ratio,
//                              rect.height()*ratio);
//}


void Cce140f::SaveAsText(void)
{
    mainwindow->releaseKeyboard();

    QString s = QFileDialog::getSaveFileName(
                    mainwindow,
                    tr("Choose a filename to save under"),
                    ".",
                   tr("Text File (*.txt)"));

    QFile file(s);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    file.write(TextBuffer);

    mainwindow->grabKeyboard();
        AddLog(LOG_PRINTER,TextBuffer.data());
}

void Cce140f::RefreshCe140f(qint8 data)
{

    QPainter painter;

//if (posX==0) bells->play();

// copy ce126buf to ce126display
// The final paper image is 207 x 149 at (277,0) for the ce125

// grab data char to byteArray
    TextBuffer += data;

    if (data == 0x0d){
        top+=10;
        posX=0;
    }
    else
    {
        painter.begin(ce140fbuf);
        int x = ((data>>4) & 0x0F)*6;
        int y = (data & 0x0F) * 8;
        painter.drawImage(	QPointF( 25 + (7 * posX),top),
                            *charTable,
                            QRectF( x , y , 5,7));
        posX++;
        painter.end();
    }


    painter.begin(ce140fdisplay);

    painter.drawImage(QRectF(0,MAX(149-top,0),207,MIN(top,149)),*ce140fbuf,QRectF(0,MAX(0,top-149),207,MIN(top,149)));

// Draw printer head
    painter.fillRect(QRect(0 , 147,207,2),QBrush(QColor(0,0,0)));
    painter.fillRect(QRect(21 + (7 * posX) , 147,14,2),QBrush(QColor(255,255,255)));

    painter.end();

    Refresh_Display = true;

    paperWidget->setOffset(QPoint(0,top));

}


/*****************************************************/
/* Initialize PRINTER								 */
/*****************************************************/
void Cce140f::clearPaper(void)
{
    // Fill it blank
    ce140fbuf->fill(PaperColor.rgba());
    ce140fdisplay->fill(QColor(255,255,255,0).rgba());
    settop(10);
    setposX(0);
    // empty TextBuffer
    TextBuffer.clear();
}


bool Cce140f::init(void)
{
    CPObject::init();

    setfrequency( 0);

    WatchPoint.add(&pCONNECTOR_value,64,11,this,"Standard 11pins connector");
    WatchPoint.add(&pTAPECONNECTOR_value,64,2,this,"Line In / Rec");


    AddLog(LOG_PRINTER,tr("PRT initializing..."));

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();



    // Create CE-126 Paper Image
    // The final paper image is 207 x 149 at (277,0) for the ce125
    ce140fbuf	= new QImage(QSize(207, 3000),QImage::Format_ARGB32);
    ce140fdisplay= new QImage(QSize(207, 149),QImage::Format_ARGB32);

    // Fill it blank
    clearPaper();

    charTable = new QImage(":/EXT/ext/ce126ptable.bmp");

//	bells	 = new QSound("ce.wav");

// Create a paper widget

    paperWidget = new CpaperWidget(PaperPos(),ce140fbuf,this);
    paperWidget->show();

//	SET_PIN(PIN_ACK,DOWN);
AddLog(LOG_PRINTER,tr("Initial value for PIN_BUSY %1").arg(GET_PIN(PIN_BUSY)?"1":"0"));
    Previous_PIN_BUSY = GET_PIN(PIN_BUSY);
    Previous_PIN_MT_OUT1 = GET_PIN(PIN_MT_OUT1);
    code_transfer_step = 0;
    device_code = 0;
    wait_data_function=0;
    halfdata = false;
    halfdata_out = false;
    time.start();

    run_oldstate = -1;
    return true;
}



/*****************************************************/
/* Exit PRINTER										 */
/*****************************************************/
bool Cce140f::exit(void)
{
    AddLog(LOG_PRINTER,"PRT Closing...");
    AddLog(LOG_PRINTER,"done.");
    Cprinter::exit();
    return true;
}


/*****************************************************/
/* CE-126P PRINTER emulation						 */
/*****************************************************/
void Cce140f::Printer(qint8 d)
{
    if(ctrl_char && d==0x20)
        ctrl_char=false;
    else
    {
        if(d==0xf || d==0xe)
            ctrl_char=true;
        else
        {
            RefreshCe140f(d);
        }
    }
}

//********************************************************/
//* Check for E-PORT and Get data						 */
//********************************************************/
// PIN_MT_OUT2	1
// PIN_GND		2
// PIN_VGG		3
// PIN_BUSY		4
// PIN_D_OUT	5
// PIN_MT_IN	6
// PIN_MT_OUT1	7
// PIN_D_IN		8
// PIN_ACK		9
// PIN_SEL2		10
// PIN_SEL1		11
//********************************************************/



#define		WAIT ( pPC->frequency / 10000*6)

#define RECEIVE_MODE	1
#define SEND_MODE		2
#define TEST_MODE		3

extern int LogLevel;
//
// no bit start
// no bit stop
void Cce140f::pulldownsignal(void)
{

#if 0
    SET_PIN(PIN_BUSY,DOWN);
    SET_PIN(PIN_D_OUT,DOWN);
    SET_PIN(PIN_D_IN,DOWN);
    SET_PIN(PIN_SEL2,DOWN);
    SET_PIN(PIN_SEL1,DOWN);
#endif
}

BYTE Cce140f::Pop_out8(void) {
    return data_out.takeFirst();
}

BYTE Cce140f::Pop_out4(void) {
    BYTE t=0;
    if (halfdata_out) {
        t = data_out.takeFirst();
        halfdata_out = false;

        AddLog(LOG_PRINTER,tr("byte from floppy : %1 - %2").arg(t,2,16).arg(QChar(t)) );
        t = (t >> 4);
    }
    else {
        halfdata_out = true;
        t = data_out.first() & 0x0F;
    }
    //AddLog(LOG_PRINTER,tr("4bits from floppy : %1").arg((t >> 4),2,16) );
    return t;
}

void Cce140f::Push8(BYTE b) {
    data.append(b);
}
void Cce140f::Push4(BYTE b) {
    if (halfdata) {
        int t = data.last()+(b<<4);
        data.pop_back();
        data.append(t);
        halfdata = false;
        AddLog(LOG_PRINTER,tr("byte to floppy : %1 - %2").arg(t,2,16).arg(QChar(t)) );
    }
    else {
        data.append(b);
        halfdata = true;
    }
}

#define CE126LATENCY (pTIMER->pPC->getfrequency()/3200)
//#define CE126LATENCY (getfrequency()/3200)
bool Cce140f::run(void)
{

    bool bit = false;
    ce140f_Mode=RECEIVE_MODE;

    pTAPECONNECTOR->Set_pin(3,(rmtSwitch ? GET_PIN(PIN_SEL1):true));       // RMT
    pTAPECONNECTOR->Set_pin(2,GET_PIN(PIN_MT_OUT1));    // Out
    SET_PIN(PIN_MT_IN,pTAPECONNECTOR->Get_pin(1));      // In


    pCONNECTOR_value = pCONNECTOR->Get_values();
    pTAPECONNECTOR_value = pTAPECONNECTOR->Get_values();

#if 1
// Try to introduce a latency
    qint64			deltastate = 0;

    if (run_oldstate == -1) run_oldstate = pTIMER->state;
    deltastate = pTIMER->state - run_oldstate;
    if (deltastate < CE126LATENCY ) return true;
    run_oldstate	= pTIMER->state;
#endif

#if 0
    char dump1[20]="";
    static char dump2[20]="";

    pCONNECTOR->Dump_pin();
    strcpy(dump1,pCONNECTOR->dump);

    if (strcmp(dump1,dump2) != 0)
    {
        strcpy(dump2 ,dump1);
        AddLog(LOG_PRINTER,tr("input printer %1").arg(dump1));
    }
#endif

//	AddLog(LOG_PRINTER,tr("%1").arg(dump1));

    bool PIN_BUSY_GoDown = ( ( GET_PIN(PIN_BUSY) == DOWN ) && (Previous_PIN_BUSY == UP)) ? true:false;
    bool PIN_BUSY_GoUp   = ( ( GET_PIN(PIN_BUSY) == UP ) && (Previous_PIN_BUSY == DOWN)) ? true:false;
    bool PIN_BUSY_Change = (GET_PIN(PIN_BUSY) != Previous_PIN_BUSY ) ? true:false;
    bool PIN_MT_OUT1_GoDown = ( ( GET_PIN(PIN_MT_OUT1) == DOWN) &&	(Previous_PIN_MT_OUT1 == UP)) ? true:false;
    bool PIN_MT_OUT1_GoUp = ( (Previous_PIN_MT_OUT1 == DOWN) && (GET_PIN(PIN_MT_OUT1) == UP )) ? true:false;
    bool PIN_MT_OUT1_Change = (GET_PIN(PIN_MT_OUT1) != Previous_PIN_MT_OUT1 ) ? true:false;
    bool PIN_D_OUT_Change = (GET_PIN(PIN_D_OUT) != Previous_PIN_D_OUT ) ? true:false;

//    if (   (GET_PIN(PIN_MT_OUT1) == UP)
//        && (GET_PIN(PIN_D_OUT)==UP) ) {
//        if (PIN_MT_OUT1_Change || PIN_D_OUT_Change) {
//            time.restart();
//            code_transfer_step=1;
//            if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 8;
//        }
//        if (time.elapsed() > 40) {
//            // Code transfer sequence started
//            // Raise ACK
//            code_transfer_step = 2;
//            SET_PIN(PIN_ACK,UP);
//            if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 9;
//        }
//    }
//    else {
//        code_transfer_step=0;
//    }

    switch (code_transfer_step) {
    case 0 :    if ((GET_PIN(PIN_MT_OUT1) == UP) && (GET_PIN(PIN_D_OUT)==UP)) {
                    time.restart();
                    code_transfer_step=1;
                    if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 8;
                }
                break;
    case 1 :    if ((GET_PIN(PIN_MT_OUT1) == UP) && (GET_PIN(PIN_D_OUT)==UP)) {
                    if (time.elapsed() > 40) {
                        // Code transfer sequence started
                        // Raise ACK
                        code_transfer_step = 2;
                        SET_PIN(PIN_ACK,UP);
                        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 9;
                    }
                }
                else {
                    code_transfer_step=0;
                    //if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 7;
                }
                break;
    case 2:     if (GET_PIN(PIN_BUSY) == UP )	//check for BUSY
                {
                    if(GET_PIN(PIN_D_OUT) == UP)
                    {
                        bit = true;;
                    } else
                    {
                        bit = false;
                    }
                    t>>=1;
                    if (bit) t|=0x80;
                    if((c=(++c)&7)==0)
                    {
                        AddLog(LOG_PRINTER,tr("device code : %1").arg(t,2,16) );
                        device_code = t;
                        //Printer(t);
                        SET_PIN(PIN_ACK,DOWN);
                        code_transfer_step=4;
                        //t=0; c=0;
                    }
                    else {
                        SET_PIN(PIN_ACK,DOWN);
                        code_transfer_step=3;
                    }
                }
                break;
    case 3:     code_transfer_step=2;
                    // wait 2 ms and raise ACK
                    SET_PIN(PIN_ACK,UP);

                break;
    case 4:     if ((GET_PIN(PIN_BUSY) == DOWN)&&(GET_PIN(PIN_MT_OUT1) == DOWN)) {
                    SET_PIN(PIN_ACK,UP);
                    code_transfer_step=5;
                    time.restart();
                    t=0; c=0;
                }
                break;
    case 5:     if (time.elapsed()>9) {
                    SET_PIN(PIN_ACK,DOWN);
                    code_transfer_step=0;


                }
                break;
            }

    //if ( (device_code == 0x41) && (code_transfer_step==0)) {
        if ((device_code >0) && (code_transfer_step==0)&&PIN_BUSY_GoUp && (GET_PIN(PIN_ACK)==DOWN)) {
            // read the 4 bits

            t = GET_PIN(PIN_SEL1) + (GET_PIN(PIN_SEL2)<<1) + (GET_PIN(PIN_D_OUT)<<2) + (GET_PIN(PIN_D_IN)<<3);

            //AddLog(LOG_PRINTER,tr("4bits to floppy : %1").arg(t,2,16) );
            Push4(t);
            SET_PIN(PIN_ACK,UP);
            time.restart();

            if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 10;
        }
        else
        if ((device_code >0) && (code_transfer_step==0)&&PIN_BUSY_GoDown && (GET_PIN(PIN_ACK)==UP)) {
            SET_PIN(PIN_ACK,DOWN);
            time.restart();
            if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 11;
        }
//    }

else

    if ( !data_out.empty() && (time.elapsed()>5) && (GET_PIN(PIN_BUSY)==DOWN) && (GET_PIN(PIN_ACK)==DOWN)) {
        BYTE t = Pop_out4();

        SET_PIN(PIN_SEL1,t&0x01);
        SET_PIN(PIN_SEL2,(t&0x02)>>1);
        SET_PIN(PIN_D_OUT,(t&0x04)>>2);
        SET_PIN(PIN_D_IN,(t&0x08)>>3);

        SET_PIN(PIN_ACK,UP);
        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->dataplot.Marker = 12;
    }

else
    if ((GET_PIN(PIN_ACK)==UP) && PIN_BUSY_GoUp) {
        SET_PIN(PIN_ACK,DOWN);
    }
else
    if ((time.elapsed()>20) && !data.empty()) {
        processCommand();
        data.clear();
    }




    Previous_PIN_BUSY = GET_PIN(PIN_BUSY);
    Previous_PIN_MT_OUT1 = GET_PIN(PIN_MT_OUT1);
    Previous_PIN_D_OUT = GET_PIN(PIN_D_OUT);


#if 0
    char dump11[20]="";
    static char dump12[20]="";

    pCONNECTOR->Dump_pin();
    strcpy(dump11,pCONNECTOR->dump);

    if (strcmp(dump11,dump12) != 0)
    {
        strcpy(dump12 ,dump11);
        AddLog(LOG_PRINTER,tr("output printer %1").arg(dump11));
    }
#endif

    pCONNECTOR_value = pCONNECTOR->Get_values();
    pTAPECONNECTOR_value = pTAPECONNECTOR->Get_values();


    return true;
}

void Cce140f::processCommand(void) {
    QString s ="";
    for (int i =0;i< data.size();i++) {
        s.append(QChar(data.at(i)));
    }
    AddLog(LOG_PRINTER,tr("floppy command : %1").arg(s));

    // Verify checksum
    int checksum = 0;
    for (int i =0;i< data.size()-1;i++) {
        checksum = (data.at(i)+checksum) & 0xff;
    }
    AddLog(LOG_PRINTER,tr("floppy command checksum = %1 compared to %2").arg(checksum,2,16).arg(data.last(),2,16));

    if (wait_data_function >0) {
        data.prepend(wait_data_function);
        wait_data_function = 0;
    }

    switch (data.first()) {
        case 0x05: process_FILES();break;
        case 0x06: process_FILES_LIST();break;
        case 0x08: process_INIT(0);break;
        case 0x09: process_INIT(1);break;
        case 0x0E: process_LOAD(0);break;
        case 0x17: process_LOAD(1);break;
        case 0x12: process_LOAD(2);break;
        case 0x10: process_SAVE(0);break;
        case 0x11: process_SAVE(1);break;
        case 0x16: process_SAVE(2);break;       // SAVE ASCII
        case 0x1D: process_DSKF(); break;
        case 0xFE: process_SAVE(0xfe);break;    // Handle ascii saved data stream
        case 0xFF: process_SAVE(0xff);break;    // Handle saved data stream
    }
}

void Cce140f::process_DSKF(void) {
    AddLog(LOG_PRINTER,tr("Process DSKF(%1)").arg(data.at(1)));
    data_out.append(0x00);

    data_out.append(0x01);
    data_out.append(0x02);
    data_out.append(0x04);
    data_out.append(0x07);

//    data_out.append(0x49);
}

void Cce140f::process_FILES(void) {
    AddLog(LOG_PRINTER,tr("Process FILES(%1)").arg(data.at(1)));
    data_out.append(0x00);
    checksum = 0;

    // Send nb files
    data_out.append(CheckSum(0x02));

    // Send CheckSum
    data_out.append(checksum);

}

void Cce140f::process_FILES_LIST(void) {
    AddLog(LOG_PRINTER,tr("Process FILES_LIST"));
    data_out.append(0x00);

    checksum=0;

    // Send filenames
    sendString("X:TEST    .BAS ");

    //sendString(" ");

    // Send CheckSum
    data_out.append(checksum);

}

void Cce140f::process_INIT(int cmd) {
    switch (cmd) {
    case 0:
        AddLog(LOG_PRINTER,tr("Process INIT(%1)").arg(data.at(1)));
        data_out.append(0x00);
        break;
    case 1:
        AddLog(LOG_PRINTER,tr("Process INIT2(%1)").arg(data.at(1)));
        data_out.append(0x00);
        break;

    }
}

void Cce140f::process_SAVE(int cmd) {
    QString s = "";
    switch (cmd) {

    case 0:
            // create file

            for (int i=1;i<15;i++) {
                s.append(QChar(data.at(i)));
            }
            AddLog(LOG_PRINTER,tr("process_SAVE file:%1").arg(s));


            data_out.append(0x00);
            break;
    case 1:
            for (int i=1;i<15;i++) {
                s.append(QChar(data.at(i)));
            }
            AddLog(LOG_PRINTER,tr("process_SAVE file:%1").arg(s));

            // transmet 4 bytes : the lenght ?
            data_out.append(0x00);
            wait_data_function = 0xff;
            break;
    case 2:
            for (int i=1;i<15;i++) {
                s.append(QChar(data.at(i)));
            }
            AddLog(LOG_PRINTER,tr("process_SAVE ASCIIfile:%1").arg(s));

            // transmet 4 bytes : the lenght ?
            data_out.append(0x00);
            wait_data_function = 0xfe;
            break;
    case 0xfe: // received ascii data steam
            //store data
            AddLog(LOG_PRINTER,tr("retreived ascii data stream"));
            data_out.append(0x00);

            break;
    case 0xff: // received data
            //store data
            AddLog(LOG_PRINTER,tr("retreived data stream"));
            data_out.append(0x00);
            break;
        }
}

void Cce140f::process_LOAD(int cmd) {
    QString s = "";
    switch (cmd) {

    case 0:
            // open file

            for (int i=1;i<15;i++) {
                s.append(QChar(data.at(i)));
            }
            AddLog(LOG_PRINTER,tr("process_LOAD file:%1").arg(s));


            data_out.append(0x00);
            checksum = 0;
            sendString(" ");

            // Send size : 3 bytes + checksum
            data_out.append(CheckSum(0x25));
            data_out.append(CheckSum(0x00));
            data_out.append(CheckSum(0x00));
            data_out.append(checksum);
            break;
    case 1:


            // transmet 4 bytes : the lenght ?
            data_out.append(0x00);
            checksum=0;
            data_out.append(CheckSum(0x30));
            data_out.append(checksum);
            //wait_data_function = 0xfd;
            break;
    case 2:

            // transmet 4 bytes : the lenght ?
            data_out.append(0x00);
            checksum=0;
            data_out.append(CheckSum(0x1A));  // 0x1A pour fin de fichier
            data_out.append(checksum);
            data_out.append(0x00);
            break;
    case 0xfe: // received ascii data steam
            //store data
            AddLog(LOG_PRINTER,tr("retreived ascii data stream"));
            data_out.append(0x00);

            break;
    case 0xff: // received data
            //store data
            AddLog(LOG_PRINTER,tr("retreived data stream"));
            data_out.append(0x00);
            break;
        }
}

void Cce140f::sendString(QString s) {
    for (int i=0;i<s.size();i++){
        data_out.append(CheckSum(s.at(i).toAscii()));
    }
}

BYTE Cce140f::CheckSum(BYTE b) {
    checksum = (checksum + b) & 0xff;
    return b;
}
