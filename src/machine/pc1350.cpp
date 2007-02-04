/*** POCKEMUL [PC1350.CPP] ********************************/
/* PC1350 emulator main class                             */
/**********************************************************/
 
#include	<string.h>
#include	<stdlib.h>
#include	<QMessageBox>
#include	"common.h"
#include	"pc1350.h"

#include	"Inter.h"
#include	"Keyb.h"
#include	"Log.h"
#include	"dialoganalog.h"

bool Cpc13XX::init(void)
{
	CpcXXXX::init();
	WatchPoint.add(&pSIOCONNECTOR_value,64,15,this,"Serial 15pins connector");
	
	return true;
}

 
void	Cpc1350::initExtension(void)
{
	// initialise ext_MemSlot1
	ext_MemSlot1 = new CExtensionArray("Memory Slot 1","Add memory credit card");
	ext_MemSlot1->setAvailable(ID_CE201M,true);		ext_MemSlot1->setChecked(ID_CE202M,true);
	ext_MemSlot1->setAvailable(ID_CE202M,true);
	ext_MemSlot1->setAvailable(ID_CE203M,true);
	
	addExtMenu(ext_MemSlot1);
	
}

bool Cpc13XX::CheckUpdateExtension(CExtension *ext)
{
		if (ext)
			if (ext->Id == "ce-125")
			{
				switch(QMessageBox::warning(mainwindow, "PockEmul",
	                                    "This model does not fit correctly into the CE-125.\n"
	                                    "You risk connector damage !!!\n "
	                                    "Are you sure ????\n",
	                                    "Yes",
	                                    "Abort", 0, 0, 1)) { 
	        	case 0: // The user clicked the Continue again button or pressed Enter
	            // try again
	            	return true;
	        	    break;
	        	case 1: // The user clicked the Cancel or pressed Escape
	        	    // exit
	        	    return false;
	        	    break;
	        	}
	       	}
       	return true;
}



BYTE	Cpc13XX::Get_PortA(void)
{
	IO_A = pKEYB->Read(IO_A);

	return (IO_A);
}

BYTE	Cpc1350::Get_PortA(void)
{
	IO_A = Cpc13XX::Get_PortA();

	if (! Japan) PUT_BIT(IO_A,7,pKEYB->Get_KS() & 0x40);

	return (IO_A);
}

BYTE	Cpc1350::Get_PortB(void)
{
	return (IO_B);
}


void	Cpc13XX::Set_PortA(BYTE data)
{
	static BYTE cnt=0;

	if ((IO_A != 0) && (data == 0))
	{
		++cnt;
		if (cnt > 2)
		{
			pKEYB->keyscan();
			cnt = 0;
		}
	}
	IO_A = data;
}

void	Cpc1350::Set_PortB(BYTE data)
{
	IO_B = data;
}

void	Cpc1350::Set_PortF(BYTE data)
{
	IO_F = data;
}

// PIN_MT_OUT2	1
// PIN_GND		2
// PIN_VGG		3
// PIN_BUSY		4
// PIN_D_IN		5
// PIN_MT_IN	6
// PIN_MT_OUT1	7
// PIN_D_OUT	8
// PIN_ACK		9
// PIN_SEL2		10
// PIN_SEL1		11

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

bool Cpc1350::Set_Connector(void)
{
#if 1
	// MANAGE STANDARD CONNECTOR
	pCONNECTOR->Set_pin(PIN_MT_OUT2	,0);
	pCONNECTOR->Set_pin(PIN_GND		,0);
	pCONNECTOR->Set_pin(PIN_VGG		,1);
	pCONNECTOR->Set_pin(PIN_BUSY	,GET_PORT_BIT(PORT_F,3));		// F03
	pCONNECTOR->Set_pin(PIN_D_OUT	,GET_PORT_BIT(PORT_F,2));		// F02
	pCONNECTOR->Set_pin(PIN_MT_OUT1	,pCPU->Get_Xout());
	pCONNECTOR->Set_pin(PIN_SEL2	,0);
	pCONNECTOR->Set_pin(PIN_SEL1	,0);
#endif

	// MANAGE SERIAL CONNECTOR
	// TO DO 
	pSIOCONNECTOR->Set_pin(SIO_SD	,GET_PORT_BIT(PORT_F,4));	//	pSIO->Set_SD((IO_F & 0x08) ? 1 : 0);
	pSIOCONNECTOR->Set_pin(SIO_RR	,GET_PORT_BIT(PORT_B,3));	//( READ_BIT(IO_B,2) );
	pSIOCONNECTOR->Set_pin(SIO_RS	,GET_PORT_BIT(PORT_B,2));	//( READ_BIT(IO_B,1) );
	pSIOCONNECTOR->Set_pin(SIO_ER	,GET_PORT_BIT(PORT_B,1));	//( READ_BIT(IO_B,0) );
	
	return(1);
}

bool Cpc1350::Get_Connector(void)
{
	// MANAGE STANDARD CONNECTOR
	Set_Port_Bit(PORT_B,8,pCONNECTOR->Get_pin(PIN_D_IN));	// DIN	:	IB8
	Set_Port_Bit(PORT_B,7,pCONNECTOR->Get_pin(PIN_ACK));	// ACK	:	IB7
	pCPU->Set_Xin(pCONNECTOR->Get_pin(PIN_MT_IN));

	// MANAGE SERIAL CONNECTOR
	// TO DO 
	Set_Port_Bit(PORT_B,4,pSIOCONNECTOR->Get_pin(SIO_RD));
	Set_Port_Bit(PORT_B,5,pSIOCONNECTOR->Get_pin(SIO_CS));
	Set_Port_Bit(PORT_B,6,pSIOCONNECTOR->Get_pin(SIO_CD));
	// PAK
	return(1);
}




/*****************************************************************************/
/* RETURN: 0=error, 1=success												 */
/*****************************************************************************/

bool Cpc1350::run(void) 
{ 
	pSIOCONNECTOR_value = pSIOCONNECTOR->Get_values();

	CpcXXXX::run();

#if 0
	// HACK Program Counter
#define FUNC_CALL(ADDR,LIB) case ADDR: AddLog(LOG_ROM,tr(LIB).arg(pCPU->get_PC(),5,16,QChar('0')));

	switch (pCPU->get_PC())
	{
	FUNC_CALL(0xF22A,QT_TR_NOOP("Function Call [%1] - SIO Input 1 byte"));			break;
	//FUNC_CALL(0xF343,QT_TR_NOOP("Function Call [%1] - LOAD"));						break;
	//FUNC_CALL(0xF267,QT_TR_NOOP("Function Call [%1] - SAVE"));						break;

	FUNC_CALL(0x9D1D,QT_TR_NOOP("Function Call [%1] - Header Input"));				;
//											pCPU->logsw=1;
//											pCPU->Check_Log();
											break;
	}
#endif
	return(1); 
}

/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/
// Virtual Fonction

bool Cpc1350::Chk_Adr(DWORD *d,DWORD data)
{

	if ( (*d>=0x7000) && (*d<=0x79FF) )	{pLCDC->SetDirtyBuf(*d-0x7000);return(1);}
	if ( (*d>=0x7E00) && (*d<=0x7FFF) )	
	{
		pKEYB->Set_KS( (BYTE) data & 0x7F );
		return(1);
	}

	if ( (*d>=0x2000) && (*d<=0x3FFF) && EXTENSION_CE201M_CHECK ) { *d+=0x2000;	return(1);}										// 8Kb Ram Card Image
	if ( (*d>=0x2000) && (*d<=0x3FFF) )	{ return( EXTENSION_CE202M_CHECK | EXTENSION_CE203M_CHECK); }							// 16K
	if ( (*d>=0x4000) && (*d<=0x5FFF) )	{ return( EXTENSION_CE201M_CHECK | EXTENSION_CE202M_CHECK | EXTENSION_CE203M_CHECK); }	// 32K
	if ( (*d>=0x6000) && (*d<=0x7FFF) ) return(1);																				// Internal RAM area(6000-8000)

	return(0);

}

bool Cpc1350::Chk_Adr_R(DWORD *d,DWORD data) { return(1); }



