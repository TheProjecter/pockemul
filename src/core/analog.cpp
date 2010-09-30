#include <QPainter>
#include <QPen>
#include <QFile>

#include <limits.h>
#include "common.h"
#include "global.h"
#include "Log.h"
#include "analog.h"




void CData::Clear(void)
{
	values.clear();
	state.clear();
	marker.clear();
}
void CData::Write(TAnalog_Data tmp_value)
{

// if values unchanged , simple remove last entry and add the new one
	if ((values.size() >3) &&
		(values.last() == tmp_value.values) &&
		(values.last() == values.at(values.size()-2)) &&
		(marker.last() == Marker) &&
		(marker.last() == marker.at(marker.size()-2))
		)
	{
		values.removeLast();
		state.removeLast();
		marker.removeLast();
	}
	else
	{
		AddLog(LOG_ANALOG,tr("[%1]  state:%2  - value %3").arg(values.size()).arg(tmp_value.state).arg(tmp_value.values));
	}
	 	
	values.append(tmp_value.values);
	state.append(tmp_value.state);
	marker.append(Marker);
	Marker = 0;
	
	if (values.size() > maxSize){
		values.removeFirst();
		state.removeFirst();
		marker.removeFirst();
	} 
}

void CData::Write(qint64 val,qint64 state)
{
	TAnalog_Data loc;
	
	loc.values = val;
	loc.state = state;
	loc.marker = Marker;
	Write(loc);
}

TAnalog_Data CData::Read(int indice)
{
	TAnalog_Data ret;
	ret.values = values.at(indice);
	ret.state = state.at(indice);
	ret.marker = marker.at(indice);
	return ret;
}

qint64 CData::Read_values(int indice)
{
	return values.at(indice);
}
qint64 CData::Read_state(int indice)
{
	return state.at(indice);
}

qint8 CData::Read_marker(int indice)
{
	return marker.at(indice);
}

int CData::Save(void){ return 1;}
int CData::Load(void){ return 1;}

QDataStream &operator<<(QDataStream &out, const CData &dataplot)
{
	out << dataplot.timeUnit;
	out << dataplot.values;
	out << dataplot.state;
	out << dataplot.marker;

    return out;
}

QDataStream &operator>>(QDataStream &in, CData &dataplot)
{
	in >> dataplot.timeUnit;
    in >> dataplot.values;
    in >> dataplot.state;
    in >> dataplot.marker;

    return in;
}




