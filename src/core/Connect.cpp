
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
	values = 0;
}

// Link current connector to Source connector
void Cconnector::ConnectTo(Cconnector * source)
{
	if (!source) return;
	values = source->values;
}

void Cconnector::Dump_pin(void)
{
	for (int i=0;i<nbpins;i++)
	{
		if ((values >>i) & 0x01)	dump[i] = '1';
		else						dump[i] = '0';
	}
	dump[nbpins]='\0';

}

qint64 Cconnector::Get_values(void)
{
	return values;
}
void Cconnector::Set_values(qint64 val)
{
    values = val;
}
// Return Pin value : pin number from 1 to N
bool Cconnector::Get_pin(qint8 PinId)
{
	return ((values >>(PinId-1)) & 0x01);
}

// Set Pin value : pin from 1 to N
void Cconnector::Set_pin(qint8 PinId,bool Value)
{
	PUT_BIT(values, (PinId-1), Value);
}

bool Cconnector::getGender()
{
	return gender;
}

void Cconnector::setGender(bool newgender)
{
	gender = newgender;
}


