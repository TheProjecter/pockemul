#ifndef BUS_H
#define BUS_H


#define BUS_SLEEP 0
#define BUS_WRITEDATA 1
#define BUS_READDATA 2
#define BUS_QUERY 3
#define BUS_SELECT 4
#define BUS_READROM 5

#include <QtCore>

class Cbus {

public:
    quint64 toUInt64();
    void fromUInt64(quint64 val);
    quint8 getData() { return data;}
    void setData(quint8 val) { data = val;}
    quint32 getAddr() { return addr;}
    void setAddr(quint32 val) { addr=val;}
    quint8 getDest() { return dest;}
    void setDest(quint8 val) { dest = val;}
    quint8 getFunc() { return func;}
    void setFunc(quint8 val) { func = val;}

private:
    quint32 addr;
    quint8 data;
    quint8 dest;
    quint8 func;


};

#endif // BUS_H
