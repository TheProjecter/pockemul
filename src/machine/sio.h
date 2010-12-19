#ifndef _SIO_H
#define _SIO_H

/*** POCKEMUL [SIO.H] *************************************/
/* UART(serial port) emulation class                      */
/**********************************************************/
#include <QMenu>
#include "Connect.h"
#include "pobject.h"
#include "Inter.h"
#include "init.h"

#define TICKS_BDS	(pTIMER->pPC->getfrequency()/baudrate)
class DialogConsole;

class Csio:public CPObject{
Q_OBJECT

public:
    const char*	GetClassName(){ return("Csio");};

	QByteArray baOutput;
	QByteArray baInput;
		
    Cconnector	*pSIOCONNECTOR; qint64 pSIOCONNECTOR_value;

    virtual bool run(void);

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
	void Set_SI(void);				//serial in 
	void Put_SO(void);				//serial out

	void ExportBit(bool);
    virtual void ExportByte(qint8);

    void Set_BaudRate(int);
    int  Get_BaudRate(void);

	virtual void Set_CD(bool);
	virtual void Set_CS(bool);
	virtual void Set_RD(bool);
	virtual void Set_RR(bool);
	virtual void Set_RS(bool);
	virtual void Set_ER(bool);
	virtual void Set_SD(bool);
	
	bool Get_CD(void);
	bool Get_CS(void);
	bool Get_RD(void);
	bool Get_RR(void);
	bool Get_RS(void);
	bool Get_ER(void);
	bool Get_SD(void);

	void	Set_Sii_bit(qint8);
	qint8	Get_Sii_bit(void);
	
	qint8	inReadBitFromByte(qint8 data);
	bool	inReadBit(void);
	void	startTransfer(void);

	void	outReadBit(void);


	bool	CD,CS,RD,RR,RS,ER,SD;
	qint8	Sii_bit;
	bool	si,so;					//SI,SO port access flag
	bool	plink;					//plink server ON/OFF
	qint8	plinkmode;				//select plink client
	char	*filename[2];			//SI&SO data filename
	FILE	*fp_si,*fp_so;			//SI,SO file handle
	qint8	exportbit;				//export bit a bit
	qint8	exportbyte;				//export Byte a Byte
	qint8	convCRLF;

	bool	ToDestroy;
	
	void	clearInput(void);
    DialogConsole *dialogconsole;
	
	Csio(CPObject *parent = 0)	: CPObject(this)
	{							//[constructor]
		si=so=0;			//si,so port access?(0:none, 1:access)
		plink=0;			//aplinks using?(0:none, 1:using)
		plinkmode=0;		//select plink client(0:plink, 1:plinkc)
		exportbit=0;
		exportbyte=1;
		convCRLF=1;
		
        baudrate = 1200;

		ToDestroy = false;
		
		inBitNb = 0;
		Sii_ndx				= 0;
		Sii_wait			= 0;
		Sii_startbitsent	= FALSE;
		Sii_stopbitsent		= TRUE;
		Sii_TransferStarted = FALSE;
		Sii_TextLength		= 0;
		Sii_Bit_Nb			= 0;
		Sii_LfWait			= 500;
		
        pSIOCONNECTOR = new Cconnector(this,15,"Connector 15 pins",true); publish(pSIOCONNECTOR);
		setfrequency( 0);
		BackGroundFname	= ":/EXT/ext/serial.png";
		
		pTIMER		= new Ctimer(this);
		Pc_DX	= 195;
		Pc_DY	= 145;
//		Pc_DX	= 85;
//		Pc_DY	= 173;
//		SnapPts = QPoint(672,150);
		stackBehind = true;
 	};
	
	virtual ~Csio(){
		delete(pSIOCONNECTOR);
	};
public:
	void paintEvent(QPaintEvent *);

protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void ShowConsole(void);
    void HideConsole(void);
signals:
    void valueChanged(int v);
    void newData(qint8);

private:
	int		inBitNb;
	int		Sii_ndx;
	int		Sii_wait;
	bool	Sii_startbitsent;
	bool	Sii_stopbitsent;
	bool	Sii_TransferStarted;
	int		Sii_TextLength;
	int		Sii_Bit_Nb;
	int		Sii_LfWait;	
    int     baudrate;
};



#endif  // _SIO_H
