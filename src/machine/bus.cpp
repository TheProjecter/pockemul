#include "bus.h"

quint64 Cbus::toUInt64()
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
