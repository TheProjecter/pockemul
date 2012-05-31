#include <QPainter>
#include <QtGui>

#include "common.h"
#include "Log.h"
#include "sio.h"
#include "pcxxxx.h"
#include "Inter.h"
#include "Connect.h"
#include "dialogconsole.h"
#include "dialoganalog.h"
#include "init.h"

#define SIO_GET_PIN(n)		pSIOCONNECTOR->Get_pin(getPinId(n))
#define SIO_SET_PIN(n,v)	pSIOCONNECTOR->Set_pin(getPinId(n),v)


#define SIO_GNDP	1
#define SIO_SD 		2
#define SIO_RD		3
#define SIO_RS		4
#define SIO_CS		5

#define SIO_GND		7
#define SIO_CD		8
#define SIO_VC1		10
#define SIO_RR		11
#define SIO_PAK		12
#define SIO_VC2		13
#define SIO_ER		14
#define SIO_PRQ		15


Csio::~Csio(){
    delete(pSIOCONNECTOR);
}

qint8 Csio::getPinId(SIGNAME signal) {
    return signalMap[signal];
}

bool Csio::SaveSession_File(QXmlStreamWriter *xmlOut)
{
    xmlOut->writeStartElement("session");
        xmlOut->writeAttribute("version", "2.0");
        xmlOut->writeAttribute("model", SessionHeader );
        xmlOut->writeAttribute("type",pSIOCONNECTOR->Desc);
    xmlOut->writeEndElement();  // session

    return true;
}

bool Csio::LoadSession_File(QXmlStreamReader *xmlIn)
{
    if (xmlIn->name()=="session") {
            QString type = xmlIn->attributes().value("type").toString();
            initConnectorType(type);
    }

    return true;
}

void Csio::updateMapConsole(void) {
    AddLog(LOG_MASTER,"update serial map");
    dialogconsole->lEdit_SD->setText(QString("%1").arg(signalMap[S_SD]));
    dialogconsole->lEdit_RD->setText(QString("%1").arg(signalMap[S_RD]));
    dialogconsole->lEdit_RS->setText(QString("%1").arg(signalMap[S_RS]));
    dialogconsole->lEdit_CS->setText(QString("%1").arg(signalMap[S_CS]));
    dialogconsole->lEdit_CD->setText(QString("%1").arg(signalMap[S_CD]));
    dialogconsole->lEdit_RR->setText(QString("%1").arg(signalMap[S_RR]));
    dialogconsole->lEdit_ER->setText(QString("%1").arg(signalMap[S_ER]));
    dialogconsole->update();
}

bool Csio::initSignalMap(Cconnector::ConnectorType type) {
    switch (type) {
    case Cconnector::Sharp_11 : signalMap.clear();
                                signalMap[S_SD] = 7;    // ok
                                signalMap[S_RD] = 6;    // ok
                                signalMap[S_RS] = 5;
                                signalMap[S_CS] = 9;    // ok
                                signalMap[S_CD] = 8;
                                signalMap[S_RR] = 4;
                                signalMap[S_ER] = 5;
                                updateMapConsole();
                                pSIOCONNECTOR->Desc = "Sharp 11 pins";
                                pSIOCONNECTOR->setNbpins(11);
                                pSIOCONNECTOR->setType(Cconnector::Sharp_11);
                                WatchPoint.remove((qint64*)pSIOCONNECTOR_value);
                                WatchPoint.add(&pSIOCONNECTOR_value,64,11,this,pSIOCONNECTOR->Desc);
                                BackGroundFname	= ":/EXT/ext/simu.png";
                                pSIOCONNECTOR->setSnap(QPoint(130,7));
                                setDX(160);
                                setDY(160);
                                resize(getDX(),getDY());
                                InitDisplay();
                                break;
    case Cconnector::Sharp_15 : signalMap.clear();
                                signalMap[S_SD] = 2;
                                signalMap[S_RD] = 3;
                                signalMap[S_RS] = 4;
                                signalMap[S_CS] = 5;
                                signalMap[S_CD] = 8;
                                signalMap[S_RR] = 11;
                                signalMap[S_ER] = 14;
                                updateMapConsole();
                                pSIOCONNECTOR->Desc = "Sharp 15 pins";
                                pSIOCONNECTOR->setNbpins(15);
                                pSIOCONNECTOR->setType(Cconnector::Sharp_15);
                                WatchPoint.remove((qint64*)pSIOCONNECTOR_value);
                                WatchPoint.add(&pSIOCONNECTOR_value,64,15,this,pSIOCONNECTOR->Desc);
                                BackGroundFname	= ":/EXT/ext/serial.png";
                                pSIOCONNECTOR->setSnap(QPoint(23,28));
                                setDX(195);
                                setDY(145);
                                resize(getDX(),getDY());
                                InitDisplay();
                                break;
    case Cconnector::Canon_9  : signalMap.clear();
                                signalMap[S_SD] = 2;
                                signalMap[S_RD] = 3;
                                signalMap[S_RS] = 4;
                                signalMap[S_CS] = 5;
                                signalMap[S_CD] = 8;
                                signalMap[S_RR] = 11;
                                signalMap[S_ER] = 14;
                                updateMapConsole();
                                pSIOCONNECTOR->Desc = "Canon 9 pins";
                                pSIOCONNECTOR->setNbpins(9);
                                pSIOCONNECTOR->setType(Cconnector::Canon_9);
                                WatchPoint.remove((qint64*)pSIOCONNECTOR_value);
                                WatchPoint.add(&pSIOCONNECTOR_value,64,9,this,pSIOCONNECTOR->Desc);
                                break;
    case Cconnector::DB25  :    signalMap.clear();
                                signalMap[S_SD] = 2;
                                signalMap[S_RD] = 3;
                                signalMap[S_RS] = 4;
                                signalMap[S_CS] = 5;
                                signalMap[S_CD] = 8;
                                signalMap[S_RR] = 11;
                                signalMap[S_ER] = 14;
                                updateMapConsole();
                                pSIOCONNECTOR->Desc = "DB25 Serial Connector";
                                pSIOCONNECTOR->setNbpins(25);
                                pSIOCONNECTOR->setType(Cconnector::DB25);
                                WatchPoint.remove((qint64*)pSIOCONNECTOR_value);
                                WatchPoint.add(&pSIOCONNECTOR_value,64,25,this,pSIOCONNECTOR->Desc);
                                break;
    default: return false;
        break;
    }

    return true;
}

Csio::Csio(CPObject *parent)	: CPObject(this)
{							//[constructor]
    si=so=0;			//si,so port access?(0:none, 1:access)
    plink=0;			//aplinks using?(0:none, 1:using)
    plinkmode=0;		//select plink client(0:plink, 1:plinkc)
    exportbit=0;
    exportbyte=1;
    convCRLF=1;

    currentBit=oldstate_in=0;
    Start_Bit_Sent = false;
    t=c=0;waitbitstart=1;waitbitstop=0;

    baudrate = 1200;

    ToDestroy = false;

    inBitNb = 0;
    Sii_ndx				= 0;
    Sii_wait			= 0;
    Sii_wait_recv       = 0;
    Sii_startbitsent	= FALSE;
    Sii_stopbitsent		= TRUE;
    Sii_TransferStarted = FALSE;
    Sii_TextLength		= 0;
    Sii_Bit_Nb			= 0;
    Sii_LfWait			= 500;

    pSIOCONNECTOR = new Cconnector(this,15,0,Cconnector::Sharp_15,"Sharp 15 pins",true,QPoint(23,28)); publish(pSIOCONNECTOR);
    setfrequency( 0 );
    BackGroundFname	= ":/EXT/ext/serial.png";

    pTIMER		= new Ctimer(this);
    setDX(195);
    setDY(145);

    dialogconsole = new DialogConsole(this);
    initSignalMap(Cconnector::Sharp_15);
}

void Csio::Set_Sii_bit(qint8 bit)	{ Sii_bit = bit;				}
qint8 Csio::Get_Sii_bit(void)		{ return (Sii_bit);				}

bool Csio::Get_CD(void)		{ return(CD);	}
bool Csio::Get_CS(void)		{ return(CS);	}
bool Csio::Get_RR(void)		{ return(RR);	}
bool Csio::Get_RS(void)		{ return(RS);	}
bool Csio::Get_ER(void)		{ return(ER);	}
bool Csio::Get_SD(void)		{ return(SD);	}
bool Csio::Get_RD(void)		{ return(RD);	}

void Csio::Set_SD(bool val) { SD = val;	}
void Csio::Set_CD(bool val) { CD = val;	}
void Csio::Set_CS(bool val) { CS = val;	}
void Csio::Set_RD(bool val) { RD = val;	}
void Csio::Set_RR(bool val) { RR = val;	}
void Csio::Set_ER(bool val) { ER = val;	}
void Csio::Set_RS(bool val) { RS = val;	}

void Csio::Set_BaudRate(int br) {
    baudrate = br;
    AddLog(LOG_SIO,tr("new baudrate = %1").arg(br))
}
int  Csio::Get_BaudRate(void) {return baudrate;}


void Csio::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    BuildContextMenu(&menu);

    menu.addSeparator();

    QMenu * menuConnectorType = menu.addMenu(tr("Connector Type"));
        menuConnectorType->addAction(tr("Sharp 11 pins"));
        menuConnectorType->addAction(tr("Sharp 15 pins"));
        menuConnectorType->addAction(tr("Canon 9 pins"));
        menuConnectorType->addAction(tr("DB25 Serial Connector"));

        connect(menuConnectorType, SIGNAL(triggered(QAction*)), this, SLOT(slotConnType(QAction*)));

    menu.addAction(tr("Show console"),this,SLOT(ShowConsole()));
    menu.addAction(tr("Hide console"),this,SLOT(HideConsole()));

    menu.exec(event->globalPos () );
}

void Csio::initConnectorType(QString type) {
    if (type == QString("Sharp 11 pins")) {
        initSignalMap(Cconnector::Sharp_11);
    }
    else if (type == QString("Sharp 15 pins")) {
        initSignalMap(Cconnector::Sharp_15);
    }
    else if (type == QString("Canon 9 pins")) {
        initSignalMap(Cconnector::Canon_9);
    }
    else if (type == QString("DB25 Serial Connector")) {
        initSignalMap(Cconnector::DB25);
    }

}

void Csio::slotConnType(QAction* action) {
    initConnectorType(action->text());
}

void Csio::ShowConsole(void) {
    dialogconsole->show();
}
void Csio::HideConsole(void) {
    dialogconsole->hide();
}

bool Csio::run(void)
{
    pSIOCONNECTOR_value = pSIOCONNECTOR->Get_values();

	// Read connector
    Set_SD( SIO_GET_PIN(S_SD) );
    Set_RR( SIO_GET_PIN(S_RR) );
    Set_RS( SIO_GET_PIN(S_RS) );
    Set_ER( SIO_GET_PIN(S_ER) );
//	Set_PRQ( SIO_GET_PIN(S_PRQ) );
	
    if (RS) {
        Set_CS(1);
        Set_CD(1);
//        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(1);
    }

	Sii_bit = 0;
    if (ER && CD && RR)	{
//        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(8);
        Sii_bit = inReadBit();


    }
	Set_RD( Sii_bit );	
	

    bitToByte();

    SIO_SET_PIN(S_RD, Get_RD());
    SIO_SET_PIN(S_CS, Get_CS());
    SIO_SET_PIN(S_CD, Get_CD());
    //SIO_SET_PIN(SIO_PAK, 0);

    pSIOCONNECTOR_value = pSIOCONNECTOR->Get_values();
	return true;
}

void Csio::startTransfer(void)
{
	Sii_wait			= 0;
	Sii_ndx				= 0;
	Sii_bit				= 0;
	Sii_startbitsent	= false;
	Sii_stopbitsent		= true;
	Sii_TransferStarted = true;
    byteBufferSize = baInput.size();
	Set_CD(1);
	
}

void Csio::clearInput(void)
{
	Sii_ndx = 0;
	baInput.clear();
}

bool Csio::inReadBit(void)
{

    if (oldstate_in	== 0) oldstate_in = pTIMER->state;
	BYTE		data		= 0;
	int			deltastate	= 0;

	Sii_LfWait = 100;
    Sii_wait	= TICKS_BDS;
	
// If there are new data in baInput
	if (! Sii_TransferStarted) return 0;

    //if ( Sii_ndx < baInput.size() )
    if (baInput.size())
	{
        deltastate = pTIMER->state - oldstate_in;

        if (deltastate < Sii_wait) return(currentBit);


        oldstate_in	= pTIMER->state;
//		oldstate_in	+= deltastate;

        data	= baInput.at(0);


        // Update Input proressbar
        emit valueChanged((int)((Sii_ndx*100)/byteBufferSize+.5));
        Refresh_Display = true;
		data = (data == 0x0A ? 0x0D : data);
        currentBit		= byteToBit(data);
		
        switch (currentBit)
		{
        case 3:	currentBit = 1;
                Sii_wait = TICKS_BDS;
                if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(1);
                return(currentBit);		// START BIT
		case 0:
        case 1:	AddLog(LOG_SIO,tr("Envoie bit = %1").arg(currentBit));
                Sii_wait = TICKS_BDS;
                if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(2);
                return(currentBit);		// DATA BIT

        case 2:	currentBit = 0;
                Sii_wait = TICKS_BDS;

                if (data == 0x0D)
                {
                    Sii_wait+=Sii_LfWait*pTIMER->pPC->getfrequency()/1000;
                    AddLog(LOG_SIO,tr("LF found, wait %1 ms").arg(Sii_LfWait));
                }
				
                baInput.remove(0,1);										// Next Char
                Sii_ndx++;
                if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(4);
                return(currentBit);
		}
	}

	if (
		Sii_TransferStarted &&
        (baInput.size() == 0) )
	{
		// End of file
//		Sii_ndx				= 0;
//		Set_Sii_bit(0);
		Sii_startbitsent	= FALSE;
		Sii_stopbitsent		= TRUE;
		Sii_TransferStarted	= FALSE;
		Set_CD(0);
		clearInput();
		AddLog(LOG_SIO,tr("END TRANSMISSION"));
	}

	return(0);
}


// Byte to bit
//
// Take in account number of start, stop and parity bits
//

qint8 Csio::byteToBit(qint8 data)
{
	qint8			bit			= 0;

		if (!Start_Bit_Sent)
		{
			Start_Bit_Sent = TRUE;

//            if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(1);
			AddLog(LOG_SIO,tr("START BIT : %1").arg(data,2,16,QChar('0')));

			return(3);  // Startbit = 1
		}
		else
		if (inBitNb<8)
		{
			bit = ( ((data >> inBitNb) & 0x01) ? 0 : 1 );
			inBitNb++;
//            if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(inBitNb+2);
			return(bit);
		}
		else
		if (inBitNb == 8)
		{
			inBitNb = 0;
			AddLog(LOG_SIO,tr("STOP BIT"));
//            if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(10);
			Start_Bit_Sent	= FALSE;
			return(2);	// STOPBIT : To be converted to 0
		}
	return(3);
}


void Csio::ExportBit(bool data) {}

void Csio::byteRecv(qint8 data)
{
	baOutput.append( (char) data);

    // Emit signal new data
    emit newByteRecv(data);

	Refresh_Display = true;
}

// convert bit to Byte
void Csio::bitToByte(void)
{

    int deltastate=0;
    if (oldstate_out == 0) oldstate_out	= pTIMER->state;
//	0	START BIT
//	0-1	b0 0=set,1=not set	
//	0-1	b1 0=set,1=not set
//	0-1	b2 0=set,1=not set
//	0-1	b3 0=set,1=not set
//	0-1	b4 0=set,1=not set
//	0-1	b5 0=set,1=not set
//	0-1	b6 0=set,1=not set
//	0-1	b7 0=set,1=not set
//	1	STOP BIT

    deltastate = pTIMER->state - oldstate_out;
    if (deltastate < Sii_wait_recv) {
        return;
    }
		
	if (!(ER && RS)) 
	{
        oldstate_out	= pTIMER->state;
        Sii_wait_recv	= 0;
        waitbitstart = 1;
		return;
	}	
//    Sii_wait	= TICKS_BDS;
    oldstate_out	= pTIMER->state;
//    oldstate_out	+= Sii_wait;
		
    AddLog(LOG_SIO,tr("STOP BIT"));
    if (waitbitstop && (SD==0))
    {
        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(10);
        waitbitstop = 0;
        waitbitstart = 1;
        //		Bit STOP
        AddLog(LOG_SIO,tr("STOP BIT"));
        Sii_wait_recv = 0;
    }
    else if (waitbitstart && SD)
    {
        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(1);
        waitbitstart = 0;
        //		Bit START
        AddLog(LOG_SIO,tr("START BIT"));
        Sii_wait_recv	= TICKS_BDS;
        c=0;
    }
    else if (!waitbitstart)
    {
		t>>=1;
		if(SD==0) t|=0x80;
		AddLog(LOG_SIO,tr("Bit = %1").arg(SD));
        if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(c+2);
		if((c=(++c)&7)==0)
        {
			AddLog(LOG_SIO,tr("Byte = %1").arg(t,2,16,QChar('0')));
            byteRecv(t);
			t=0;
			waitbitstop = 1;
//            Set_CS(0);
//            Sii_wait_recv = 0;
		}
	}
//    else if (mainwindow->dialoganalogic) mainwindow->dialoganalogic->setMarker(0);
}


/*****************************************************************************/
/* Initialize SIO															 */
/*****************************************************************************/
bool Csio::init(void)
{

	AddLog(LOG_MASTER,"SIO initializing...");

	CD = CS = ER = RD = RR = RS = SD = 0;

    setDX(195);//Pc_DX	= 195;
    oldstate_out = 0;
    setDY(145);//Pc_DY	= 145;

	CPObject::init();


    dialogconsole->show();
    connect(this,SIGNAL(valueChanged(int)),dialogconsole->inputProgressBar,SLOT(setValue(int)));




	return true;
}

/*****************************************************************************/
/* Exit SIO																	 */
/*****************************************************************************/
bool Csio::exit(void)
{
    if (dialogconsole) {
        dialogconsole->close();
        dialogconsole = 0;
    }
	return true;
}


/*****************************************************************************/
/* Set data to RxD port from file											 */
/*****************************************************************************/
void Csio::Set_SI(void)
{
}

/*****************************************************************************/
/* Put data to file from TxD												 */
/*****************************************************************************/
void Csio::Put_SO(void)
{
}

 
void Csio::paintEvent(QPaintEvent *event)
{
	CPObject::paintEvent(event);
	dialogconsole->refresh();
}
