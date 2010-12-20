/*** POCKEMUL [PC1401.CPP] ********************************/
/* PC1401 emulator main class                             */
/**********************************************************/
#include	<string.h>
#include	<stdlib.h>
 
#include	"common.h"
#include	"pc1401.h"

#include 	"Log.h"
#include	"Inter.h"
#include	"Keyb.h"
#include	"extension.h"

bool Cpc1401::CheckUpdateExtension(CExtension *ext)
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


void Cpc1401::TurnON(void)
{
	CpcXXXX::TurnON();


}


BYTE	Cpc1401::Get_PortA(void)
{
	IO_A = pKEYB->Read(IO_A);
	return(IO_A);
}

void	Cpc1401::Set_PortA(BYTE data)
{


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
			
void	Cpc1401::Set_PortB(BYTE data)
{
	IO_B = data;
	pKEYB->Set_KS(data);
}

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

bool Cpc1401::Set_Connector(void)
{
#if 1
	pCONNECTOR->Set_pin(PIN_MT_OUT2	,0);
	pCONNECTOR->Set_pin(PIN_VGG		,1);
	pCONNECTOR->Set_pin(PIN_BUSY	,GET_PORT_BIT(PORT_F,1));		// F01
	pCONNECTOR->Set_pin(PIN_D_OUT	,GET_PORT_BIT(PORT_F,2));		// F02
	pCONNECTOR->Set_pin(PIN_MT_OUT1	,pCPU->Get_Xout());
	pCONNECTOR->Set_pin(PIN_SEL2	,GET_PORT_BIT(PORT_B,6));		// B06
	pCONNECTOR->Set_pin(PIN_SEL1	,GET_PORT_BIT(PORT_B,5));		// B05
#endif
	return(1);
}

bool Cpc1401::Get_Connector(void)
{
#if 1
	Set_Port_Bit(PORT_B,8,pCONNECTOR->Get_pin(PIN_D_IN));	// DIN	:	IB7
	Set_Port_Bit(PORT_B,7,pCONNECTOR->Get_pin(PIN_ACK));	// ACK	:	IB8
	pCPU->Set_Xin(pCONNECTOR->Get_pin(PIN_MT_IN));
#endif
	return(1);
}




/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/
// Virtual Fonction

bool Cpc1401::Chk_Adr(DWORD *d,DWORD data)
{

	if ( (*d>=0x0000) && (*d<=0x1FFF) )	return(0);			// ROM area(0000-1fff) 
	if ( (*d>=0x8000) && (*d<=0xFFFF) )	return(0);			// ROM area(8000-ffff) 
	if ( (*d>=0x6000) && (*d<=0x67FF) ) {	pLCDC->SetDirtyBuf(*d-0x6000);	return(1);	}
	if ( (*d>=0x2800) && (*d<=0x47FF) )	return(1);
	if ( (*d>=0x6800) && (*d<=0x685F) )	return(1); 

	return(0);
}

bool Cpc1401::Chk_Adr_R(DWORD *d,DWORD data)
{
	return(1);
}



