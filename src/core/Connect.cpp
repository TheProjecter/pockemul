
//#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "Connect.h"

class CPObject;

Cconnector::Cconnector(CPObject *parent , QString desc, bool newGender)
{
    Parent	= parent;
    Desc = desc;
    gender = newGender;
    values = 0;
}

Cconnector::Cconnector(CPObject *parent , int nb, int id,ConnectorType type,QString desc, bool newGender,QPoint snap)
{
    this->Parent = parent;
    this->Desc   = desc;
    this->gender = newGender;
    this->values = 0;
    this->nbpins = nb;
    this->Id     = id;
    this->Type   = type;
    this->snap   = snap;
}

bool Cconnector::arePluggable(Cconnector *a,Cconnector *b) {
    return ( (a->nbpins==b->nbpins) && (a->gender != b->gender) && (a->Type == b->Type) );
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

Cconnector::ConnectorType Cconnector::getType()
{
    return this->Type;
}

void Cconnector::setType(Cconnector::ConnectorType type)
{
    this->Type = type;
}


