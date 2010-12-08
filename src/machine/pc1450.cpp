/*** POCKEMUL [PC1450.CPP] ********************************/
/* PC1450 emulator main class                             */
/**********************************************************/

#include	<stdlib.h>

#include	"common.h"
#include	"pc1450.h"

#include	"Inter.h"
#include	"Keyb.h"
#include	"Log.h"

//#include	"ce126.h"
//#include	"sound.h"

void	Cpc1450::initExtension(void)
{
    ext_MemSlot1 = new CExtensionArray("Memory Slot 1","Add memory credit card");
	// Add possible memory card to ext_MemSlot1
    ext_MemSlot1->setAvailable(ID_CE210M,true); ext_MemSlot1->setChecked(ID_CE202M,true);
	ext_MemSlot1->setAvailable(ID_CE211M,true);
	ext_MemSlot1->setAvailable(ID_CE212M,true);
	ext_MemSlot1->setAvailable(ID_CE2H16M,true);
	ext_MemSlot1->setAvailable(ID_CE2H32M,true);
	ext_MemSlot1->setAvailable(ID_CE2H64M,true);
	
	addExtMenu(ext_MemSlot1);

    extensionArray[0] = ext_11pins;
    extensionArray[1] = ext_MemSlot1;
    extensionArray[2] = ext_Serial;
}

BYTE	Cpc1450::Get_PortA(void)
{
    BYTE data = pKEYB->Read(IO_A);

//	if (! Japan) PUT_BIT(IO_A,7,IO_F & 0x01);

    return (data);
}



/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:BOOL (1=RAM,0=ROM)												 */
/*****************************************************************************/
// Virtual Fonction

#if 1
bool Cpc1450::Chk_Adr(DWORD *d,DWORD data)
{
	if ( (*d>=0x7000) && (*d<=0x708F) ) {pLCDC->SetDirtyBuf(*d-0x7000);return(1);	}
	if ( (*d>=0x7E00) && (*d<=0x7FFF) )	{pKEYB->Set_KS( (BYTE) data & 0x7F );return(1);}
	if ( (*d>=0x0000) && (*d<=0x1FFF) ) return(0);			// ROM area(0000-1fff) 

	if ( (*d>=0x2000) && (*d<=0x3FFF) && EXTENSION_CE201M_CHECK ) { 
				*d+=0x2000;
				return(1);}	// 8Kb Ram Card Image
	if ( (*d>=0x2000) && (*d<=0x3FFF) )	{ 
				return(EXTENSION_CE202M_CHECK | EXTENSION_CE203M_CHECK); }						// ROM area(0000-3FFF) 16K
	if ( (*d>=0x4000) && (*d<=0x5FFF) )	{ 
				return( EXTENSION_CE201M_CHECK | EXTENSION_CE202M_CHECK | EXTENSION_CE203M_CHECK); }	// ROM area(0000-3FFF) 16K

	if ( (*d>=0x6000) && (*d<=0x7FFF) ) return(1);			// Internal RAM area(6000-8000)

	if ( (*d>=0x8000) && (*d<=0xFFFF) ) return(0);			// ROM area(8000-ffff) 

	return(0);
}

bool Cpc1450::Chk_Adr_R(DWORD *d,DWORD data){ return(1); }
#endif



