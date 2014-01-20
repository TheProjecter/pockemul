#ifndef BUS_H
#define BUS_H




#include <QtCore>

enum BUS_FUNC{BUS_SLEEP,BUS_WRITEDATA,BUS_READDATA,BUS_QUERY,BUS_SELECT,BUS_INTREQUEST};

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
    BUS_FUNC getFunc() { return func;}
    void setFunc(BUS_FUNC val) { func = val;}
    bool getINT() { return interrupt; }
    void setINT(bool val) { interrupt = val; }

private:
    quint32 addr;
    quint8 data;
    quint8 dest;
    BUS_FUNC func;
    bool    interrupt;


};

#endif // BUS_H
