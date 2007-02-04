

#include	<stdlib.h>


#include	"common.h"
#include	"pc1403.h"
#include "Log.h"

#include	"Inter.h"
#include	"Keyb.h"
//#include	"ce126.h"
//#include	"sound.h"

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

/*
MT2		------------------------------------------------------------------------------------------------------------- MT_OUT2	1
 		------------------------------------------------------------------------------------------------------------- GND		2
 		------------------------------------------------------------------------------------------------------------- VGG		3
 		------------------------------------------------------------------------------------------------------------- BUSY		4
 		------------------------------------->|-----+---------------------------------------------------------------- D_OUT		5
 		--------------------------------------------)---------------------------------------------------------------- MT_IN		6
 		--------------------------------------------)---------------------------------------------------------------- MT_OUT1	7
 		--------------------------------------------)-----------+---------------------------------------------------- D_IN		8
 		--------------------------------------------)-----------)---------------------------------------------------- ACK		9
 		----------------------------+---------------)-----------)---------------------------------------------------- SEL2		10
 		----------------+-----------)---------------)-----------)---------------------------------------------------- SEL1		11
						|			|				|			|
IB5 --------------|<-----			|				|			|
									|				|			|
IB6 ---------------------------|<----				|			|
													|			|
IB7 ------------------------------------------|<-----			|
																|	
IB8 ----------------------------------------------------|<-------
*/


bool Cpc1403::Set_Connector(void)
{
	int port1 = Get_8(0x3A00);
	
	pCONNECTOR->Set_pin(PIN_MT_OUT2	,0);
	pCONNECTOR->Set_pin(PIN_VGG		,1);
	pCONNECTOR->Set_pin(PIN_BUSY	,GET_PORT_BIT(PORT_F,3));		// F03
	pCONNECTOR->Set_pin(PIN_D_OUT	,READ_BIT(port1,2));		// F02
	pCONNECTOR->Set_pin(PIN_D_IN	,READ_BIT(port1,3));		// F02
	pCONNECTOR->Set_pin(PIN_MT_OUT1	,pCPU->Get_Xout());
	pCONNECTOR->Set_pin(PIN_SEL2	,READ_BIT(port1,1));		// B06
	pCONNECTOR->Set_pin(PIN_SEL1	,READ_BIT(port1,0));		// B05

	return(1);
}

bool Cpc1403::Get_Connector(void)
{
	int port;

	port = Get_8(0x3A00);
	PUT_BIT(port,3,pCONNECTOR->Get_pin(PIN_D_IN));
	Set_8(0x3A00,port);

	port = Get_8(0x3C00);
	PUT_BIT(port,7,pCONNECTOR->Get_pin(PIN_ACK));
	Set_8(0x3C00,port);
	
	pCPU->Set_Xin(pCONNECTOR->Get_pin(PIN_MT_IN));

	port = Get_8(0x3A00);
	PUT_BIT(port,1,pCONNECTOR->Get_pin(PIN_SEL2));
	Set_8(0x3A00,port);

	port = Get_8(0x3A00);
	PUT_BIT(port,0,pCONNECTOR->Get_pin(PIN_SEL1));
	Set_8(0x3A00,port);

	return(1);
}


/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:BOOL (1=RAM,0=ROM)												 */
/*****************************************************************************/
// Virtual Fonction

bool Cpc1403::Chk_Adr(DWORD *d,DWORD data)
{

	if ( (*d>=0x0000) && (*d<=0x1FFF) )	{ return(0); }		// CPU ROM
	if ( (*d>=0x2000) && (*d<=0x2FFF) )	{ return(0); }		// unused
	if ( (*d>=0x3000) && (*d<=0x30FF) )	{ pLCDC->SetDirtyBuf(*d-0x3000);return(1); }
	if ( (*d>=0x3100) && (*d<=0x3BFF) )	{ return(1); }
	if ( (*d>=0x3C00) && (*d<=0x3DFF) )
	{
		int NewBank = data; 
//--		AddLog(LOG_ROM,tr("Change ROM Bank to %02X").arg(,NewBank);
		if (NewBank != RomBank)
		{
//--			AddLog(LOG_ROM,"Change ROM Bank from %02X to %02X",RomBank,NewBank);
			RomBank = NewBank;
			switch (RomBank)
			{
				case 0x00:
				case 0x08:	memcpy(&mem[SlotList[SLOT3].getAdr()],&mem[SlotList[SLOT5].getAdr()],SlotList[SLOT3].getSize()*1024); break;
				case 0x09:	memcpy(&mem[SlotList[SLOT3].getAdr()],&mem[SlotList[SLOT6].getAdr()],SlotList[SLOT3].getSize()*1024); break;
				case 0x0A:	memcpy(&mem[SlotList[SLOT3].getAdr()],&mem[SlotList[SLOT7].getAdr()],SlotList[SLOT3].getSize()*1024); break;
				case 0x0B:	memcpy(&mem[SlotList[SLOT3].getAdr()],&mem[SlotList[SLOT8].getAdr()],SlotList[SLOT3].getSize()*1024); break;
			}

		}
		*d=0x3C00;
		return(1);
	}
	if ( (*d>=0x3E00) && (*d<=0x3FFF) )	{ pKEYB->Set_KS( (BYTE) data & 0x7F );/*ShowPortsAuto(0);*/	return(1);}
	if ( (*d>=0x4000) && (*d<=0x7FFF) )	{ return(0); }
	if ( (*d>=0xE000) && (*d<=0xFFFF) )	{ return(1); }

	return(0);

}

bool Cpc1403::Chk_Adr_R(DWORD *d,DWORD data)
{
	if ( (*d>=0x3C00) && (*d<=0x3DFF) ) *d=0x3C00;
	return(1);	
}

bool Cpc1403H::Chk_Adr(DWORD *d,DWORD data)
{
	if ( (*d>=0x8000) && (*d<=0xdFFF) )	{ return(1); }
	return (Cpc1403::Chk_Adr(d,data));
}

bool Cpc1403H::Chk_Adr_R(DWORD *d,DWORD data)
{
	return(Cpc1403::Chk_Adr_R(d,data));	
}


