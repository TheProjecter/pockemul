
//#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "Connect.h"
#include "pcxxxx.h"
#include "Log.h"

Cconnector::Cconnector(CPObject *parent , int nb, QString desc, bool newGender = false)
{
	Parent	= parent;
	nbpins = nb;
	gender = newGender;
	Desc = desc;
	pins=(bool *)malloc((nbpins+1)*sizeof(bool));		/* alloc main ram */
	for (int i=0;i<nbpins;i++)
	{
		pins[i] = 0;
	}
}

void Cconnector::ConnectTo(Cconnector * source)
{
	if (!source) return;

	memcpy(pins,source->pins,MIN(nbpins,source->nbpins)*sizeof(bool));

//	AddLog(LOG_TEMP,tr("%1  ->  %2").arg(source->pPC->getName()).arg(pPC->getName()));
}

void Cconnector::Dump_pin(void)
{
//	dump = "00000000000";
	for (int i=0;i<nbpins;i++)
	{
		if (pins[i])	dump[i] = '1';
		else			dump[i] = '0';
	}
	dump[nbpins]='\0';
}

qint64 Cconnector::Get_values(void)
{
	qint64 tempvalues=0;
	for (int i=0;i<nbpins;i++)
	{
		tempvalues+=(pins[i] <<(i));
	}

	return tempvalues;	
}

bool Cconnector::Get_pin(qint8 PinId)
{
	if (PinId > nbpins) return 0;
		
	return ( pins[PinId-1] ? 1 : 0 );
}

void Cconnector::Set_pin(qint8 PinId,bool Value)
{
	if (PinId > nbpins) return;
	pins[PinId-1] = (Value ? 1 : 0 ) ;
}

bool Cconnector::getGender()
{
	return gender;
}

void Cconnector::setGender(bool newgender)
{
	gender = newgender;
}


bool Cconnector11::init(void)
{
	if((pins=(bool *)malloc((nbpins+1)*sizeof(bool)))==NULL) return(0);		/* alloc main ram */
	for (int i=0;i<nbpins;i++)
	{
		pins[i] = 0;
	}
	
	return(1);
}

bool Cconnector11::exit(void)
{
	return(1);
}

bool Cconnector15::init(void)
{
	if((pins=(bool *)malloc((nbpins+1)*sizeof(bool)))==NULL) return(0);		/* alloc main ram */
	for (int i=0;i<nbpins;i++)
	{
		pins[i] = 0;
	}	
	return(1);
}

bool Cconnector15::exit(void)
{
	return(1);
}
bool Cconnector60::init(void)
{
	if((pins=(bool *)malloc((nbpins+1)*sizeof(bool)))==NULL) return(0);		/* alloc main ram */
	for (int i=0;i<nbpins;i++)
	{
		pins[i] = 0;
	}	
	return(1);
}

bool Cconnector60::exit(void)
{
	return(1);
}
