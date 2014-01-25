#include "bus.h"

quint64 Cbus::toUInt64() const
{
    quint64 serialized = 0;

    serialized |= (addr & 0xffff);
    serialized |= (data << 16);
    serialized |= (dest << 24);
    quint64 _val = func;
    serialized |= ((_val&0xff) << 32);
    serialized |=( (quint64)(interrupt?1:0) << 40);
//    if (interrupt) qWarning()<<"test:"<<((serialized >> 40) &0x01);

    return serialized;
}

void Cbus::fromUInt64(quint64 val)
{
    addr = val & 0xffff;
    data = (val >>16) & 0xff;
    dest = (val >>24) & 0xff;
    func = static_cast<BUS_FUNC>((val >>32) & 0xff);
    interrupt = (val >> 40) &0x01;
    //    if (interrupt) qWarning()<<"INTERRUPT";
}

QString Cbus::toLog() const
{
    QString ret;
    ret = QString("Dest:%1  - %2").arg(getDest()).arg(getFunc());
    switch (getFunc()) {
    case BUS_QUERY: ret += "BUS_QUERY"; break;
    case BUS_SELECT: ret += "BUS_SELECT"; break;
    case BUS_READDATA: ret += "BUS_READDATA";
        ret += QString(" - addr=%1").arg(getAddr(),4,16,QChar('0'));
        break;
    case BUS_WRITEDATA: ret += "BUS_WRITEDATA";
        ret += QString(" - addr=%1").arg(getAddr(),4,16,QChar('0'));
        break;
    }
    ret += QString(" - data=%1").arg(getData(),2,16,QChar('0'));

    return ret;
}

QDebug operator<<(QDebug dbg, const Cbus &bus)
{
    dbg.nospace() << bus.toLog();
    return dbg.maybeSpace();
}

