 
#include	"common.h"

#include "pc1402.h"

bool Cpc1402::Chk_Adr(DWORD *d,DWORD data)
{

	if ( (*d>=0x0000) && (*d<=0x1FFF) )	return(0);			// ROM area(0000-1fff) 
	if ( (*d>=0x8000) && (*d<=0xFFFF) )	return(0);			// ROM area(8000-ffff) 
	if ( (*d>=0x6000) && (*d<=0x67FF) )	{pLCDC->SetDirtyBuf(*d-0x6000);return(1);}
	if ( (*d>=0x2000) && (*d<=0x7FFF) )	return(1);			// RAM area(2000-7fff) 

	return(0);
}

bool Cpc1402::Chk_Adr_R(DWORD *d,DWORD data)
{
	return(1);
}
