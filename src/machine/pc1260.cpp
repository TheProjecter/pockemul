#include	"common.h"
 
#include "pc1260.h"

BYTE	Cpc1260::Get_PortB(void)
{
	BYTE loc;
	loc = Cpc1250::Get_PortB();
	
	if (! Japan){
		if (IO_B & 4) loc = loc | 8;
	}
		
	return loc;
}

bool Cpc1260::Chk_Adr(DWORD *d,DWORD data)
{
	if ( (*d>=0x0000) && (*d<=0x1FFF) )	return(0);				// ROM area(0000-1fff) 
	if ( (*d>=0x5800) && (*d<=0x6800) )	return(1);				// ROM area(8000-ffff) 
	if ( (*d>=0x8000) && (*d<=0xFFFF) )	return(0);				// ROM area(8000-ffff) 
	if ( (*d>=0x2000) && (*d<=0x207D) )	{ pLCDC->SetDirtyBuf(*d-0x2000); return(1); }	/* LCDC (0200x) */
	if ( (*d>=0x2800) && (*d<=0x287D) )	{ pLCDC->SetDirtyBuf(*d-0x2000); return(1);	}	/* LCDC (0200x) */
	if ( (*d>=0x2000) && (*d<=0x20FF) )	return(1);				// ROM area(8000-ffff) 
	if ( (*d>=0x2800) && (*d<=0x28FF) )	return(1);				// ROM area(8000-ffff) 

	return(0);
}

bool Cpc1261::Chk_Adr(DWORD *d,DWORD data)
{
	if ( (*d>=0x4000) && (*d<=0x6800) )	return(1);				// RAM area(4000-6800) 

	return(Cpc1260::Chk_Adr(d,data));
}



