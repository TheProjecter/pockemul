#include <QPainter>
#include <QPen>
#include <QFile>

#include <limits.h>
#include "common.h"
#include "global.h"
#include "Log.h"
#include "analog.h"

//TODO  Add the capability to record several marker at the same time or introduce a n steps latency between connected objects
//TODO only use a single List of TAnalog_Data
//FIXME datastream input / Output

void CData::Clear(void)
{
    dataset.clear();
}
void CData::Write(TAnalog_Data tmp_value)
{
    // if values unchanged , simple remove last entry and add the new one

    int size = dataset.size();
    if (size >3) {
        TAnalog_Data last = dataset.last();
        if ((last.values == tmp_value.values) &&
            (last.values == dataset.at(size-2).values) &&
            (last.marker == Marker) &&
            (last.marker == dataset.at(size-2).marker)
            )
        {
            dataset.removeLast();
        }
    }
    else
    {
        AddLog(LOG_ANALOG,tr("[%1]  state:%2  - value %3").arg(dataset.size()).arg(tmp_value.state).arg(tmp_value.values));
    }

    dataset.append(tmp_value);

    Marker = 0;

    if (dataset.size() > maxSize){
        dataset.removeFirst();
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
    return dataset.at(indice);
}

qint64 CData::Read_values(int indice)
{
    return dataset.at(indice).values;
}
qint64 CData::Read_state(int indice)
{
    return dataset.at(indice).state;
}

qint8 CData::Read_marker(int indice)
{
    return dataset.at(indice).marker;
}

int CData::Save(void){ return 1;}
int CData::Load(void){ return 1;}

QDataStream &operator<<(QDataStream &out, const CData &dataplot)
{
#if 0
	out << dataplot.timeUnit;
    for (int i=0;i < dataplot.size();i++) {
        TAnalog_Data d = dataplot.Read(i);
        out << d.values << d.state << d.marker;
    }
#endif
    return out;
}

QDataStream &operator>>(QDataStream &in, CData &dataplot)
{
#if 0
    in >> dataplot.timeUnit;
    in >> dataplot.dataset;
#endif
    return in;
}




