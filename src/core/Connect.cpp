

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


