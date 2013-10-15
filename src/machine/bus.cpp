#include "bus.h"

quint64 Cbus::toUInt64()
{
    quint64 serialized = 0;

    serialized |= (addr & 0xffff);
    serialized |= (data << 16);
    serialized |= (dest << 24);
    quint64 _val = func;
    serialized |= (_val << 32);

    return serialized;
}

void Cbus::fromUInt64(quint64 val)
{
    addr = val & 0xffff;
    data = (val >>16) & 0xff;
    dest = (val >>24) & 0xff;
    func = (val >>32) & 0xff;
}
