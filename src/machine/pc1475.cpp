#include "common.h"
#include "Log.h"

#include "pc1475.h"

bool Cpc1475::Chk_Adr(DWORD *d,DWORD data)
{

	if ( (*d>=0x0000) && (*d<=0x1FFF) )	return(0);	// ROM area(0000-1fff) 
	if ( (*d>=0x2000) && (*d<=0x27FF) )	return(0);
	if ( (*d>=0x2800) && (*d<=0x2B7B) ) { pLCDC->SetDirtyBuf(*d-0x2800);return(1);			/* LCDC (0200x) */	}
	if ( (*d>=0x2800) && (*d<=0x33FF) ) return(1);
	if ( (*d>=0x3400) && (*d<=0x35FF) )	{ RomBank = data &0x07;	return(1); }
	if ( (*d>=0x3E00) && (*d<=0x3FFF) )
	{
		int K=0;
		switch (data & 0x0F)
		{
			case 0x00: K=0x00;	break;
			case 0x01: K=0x01;	break;
			case 0x02: K=0x02;	break;
			case 0x03: K=0x04;	break;
			case 0x04: K=0x08;	break;
			case 0x05: K=0x10;	break;
			case 0x06: K=0x20;	break;
			case 0x07: K=0x40;	break;
			case 0x08: K=0x80;	break;
			case 0x09: K=0x00;	break;
			case 0x0A: K=0x00;	break;
			case 0x0B: K=0x00;	break;
			case 0x0C: K=0x00;	break;
			case 0x0D: K=0x00;	break;
			case 0x0E: K=0x00;	break;
			case 0x0F: K=0x7F;	break;
		}
		pKEYB->Set_KS( K );
		return(1);
	}

#if 1
	if ( (*d>=0x3C00) && (*d<=0x3DFF) )
	{
//		AddLog(LOG_MASTER,"Write Slot Register %04X=%02X",*d,mem[*d]);	

		RamBank = (data == 0x04 ? 0 : 1);
		return(1);
	}
#endif

	if ( (*d>=0x2800) && (*d<=0x3FFF) )	return(1);
	if ( (*d>=0x4000) && (*d<=0x7FFF) )	{ *d += 0xC000 + ( RomBank * 0x4000 );	return(0); }


	if ( (*d>=0x8000) && (*d<=0xFFFF) )
	{
		*d += 0x28000 + (RamBank * 0x8000);
		if ( (*d>=0x30000) && (*d<=0x33FFF) )	return(S1_EXTENSION_CE2H32M_CHECK);
		if ( (*d>=0x34000) && (*d<=0x35FFF) )	return(S1_EXTENSION_CE2H32M_CHECK | S1_EXTENSION_CE2H16M_CHECK);
		if ( (*d>=0x36000) && (*d<=0x36FFF) )	return(S1_EXTENSION_CE2H32M_CHECK | S1_EXTENSION_CE2H16M_CHECK | S1_EXTENSION_CE212M_CHECK);
		if ( (*d>=0x37000) && (*d<=0x377FF) )	return(S1_EXTENSION_CE2H32M_CHECK | S1_EXTENSION_CE2H16M_CHECK | S1_EXTENSION_CE212M_CHECK | S1_EXTENSION_CE211M_CHECK);
		if ( (*d>=0x37800) && (*d<=0x37FFF) )	return(S1_EXTENSION_CE2H32M_CHECK | S1_EXTENSION_CE2H16M_CHECK | S1_EXTENSION_CE212M_CHECK | S1_EXTENSION_CE211M_CHECK | S1_EXTENSION_CE210M_CHECK);

		if ( (*d>=0x38000) && (*d<=0x3BFFF) )	return(S2_EXTENSION_CE2H32M_CHECK);
		if ( (*d>=0x3C000) && (*d<=0x3DFFF) )	return(S2_EXTENSION_CE2H32M_CHECK | S2_EXTENSION_CE2H16M_CHECK);
		if ( (*d>=0x3E000) && (*d<=0x3EFFF) )	return(S2_EXTENSION_CE2H32M_CHECK | S2_EXTENSION_CE2H16M_CHECK | S2_EXTENSION_CE212M_CHECK);
		if ( (*d>=0x3F000) && (*d<=0x3F7FF) )	return(S2_EXTENSION_CE2H32M_CHECK | S2_EXTENSION_CE2H16M_CHECK | S2_EXTENSION_CE212M_CHECK | S2_EXTENSION_CE211M_CHECK);
		if ( (*d>=0x3F800) && (*d<=0x3FFFF) )	return(S2_EXTENSION_CE2H32M_CHECK | S2_EXTENSION_CE2H16M_CHECK | S2_EXTENSION_CE212M_CHECK | S2_EXTENSION_CE211M_CHECK | S2_EXTENSION_CE210M_CHECK);
		
	}

	return(0);
}

bool Cpc1475::Chk_Adr_R(DWORD *d,DWORD data)
{
	if ( (*d>=0x4000) && (*d<=0x7FFF) )	{ *d += 0xC000 + ( RomBank * 0x4000 ); }	// Manage ROM Bank
	if ( (*d>=0x8000) && (*d<=0xFFFF) )	{ *d += 0x28000 + ( RamBank * 0x8000 );}	// Manage ram bank

	return(1); 
}
