#include "buspc1500.h"

quint64 CbusPc1500::toUInt64() const
{
    quint64 serialized = 0;

    serialized |= (interrupt?1:0) ;
    serialized |= ((writeMode?1:0) << 1);
    serialized |= ((enable?1:0) << 2);
    serialized |= ((me1?1:0) << 3);
    serialized |= ((pu?1:0) << 4);
    serialized |= ((pv?1:0) << 5);
    serialized |= ((quint64)addr & 0xffff)<<6;
    serialized |= ((quint64)data & 0xff) << 22;

    return serialized;
}

void CbusPc1500::fromUInt64(quint64 val)
{
    interrupt = val &0x01;
    writeMode = (val >> 1) &0x01;
    enable    = (val >> 2) &0x01;
    me1       = (val >> 3) &0x01;
    pu        = (val >> 4) &0x01;
    pv        = (val >> 5) &0x01;

    addr = (val>>6) & 0xffff;     // 16 bits
    data = (val >>22) & 0xff;     // 8 bits
}

void CbusPc1500::setAddr(quint32 val)
{
    me1=false;
    addr = val & 0xffff;
    if (val >> 16) {
        me1=true;
    }
}

QString CbusPc1500::toLog() const
{
    QString ret;
    ret = QString("addr=%1").arg(getAddr(),4,16,QChar('0'));
    ret += QString(" - data=%1").arg(getData(),2,16,QChar('0'));
    ret += QString(" Write:%1").arg(writeMode);
    ret += QString(" INT:%1").arg(interrupt);
    ret += QString(" ME1:%1").arg(me1);
    return ret;
}





