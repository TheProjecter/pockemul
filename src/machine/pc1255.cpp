#include	"common.h"

#include "pc1255.h"

bool Cpc1255::Chk_Adr(DWORD *d,DWORD data)
{
	Cpc1250::Mem_Mirror(d);
	
	if ( (*d>=0xA000) && (*d<=0xC7FF) )	return(1);			// RAM area(C800-C7ff) 

	return (Cpc1250::Chk_Adr(d,data));
}

