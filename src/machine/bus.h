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
    UINT8 getData() { return data;}
    void setData(UINT8 val) { data = val;}
    UINT32 getAddr() { return addr;}
    void setAddr(UINT32 val) { addr=val;}
    UINT8 getDest() { return dest;}
    void setDest(UINT8 val) { dest = val;}
    UINT8 getFunc() { return func;}
    void setFunc(UINT8 val) { func = val;}

private:
    UINT32 addr;
    UINT8 data;
    UINT8 dest;
    UINT8 func;


};

#endif // BUS_H
