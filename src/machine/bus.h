#ifndef BUS_H
#define BUS_H




#include <QtCore>
#include <QMetaType>


enum BUS_FUNC{BUS_SLEEP,BUS_WRITEDATA,BUS_READDATA,BUS_QUERY,BUS_SELECT,BUS_INTREQUEST,BUS_ACK};

class Cbus {


public:
    quint64 toUInt64() const;
    void fromUInt64(quint64 val);
    quint8 getData() const { return data;}
    void setData(quint8 val) { data = val;}
    quint32 getAddr() const { return addr;}
    void setAddr(quint32 val) { addr=val;}
    quint8 getDest() const { return dest;}
    void setDest(quint8 val) { dest = val;}
    BUS_FUNC getFunc() const  { return func;}
    void setFunc(BUS_FUNC val) { func = val;}
    bool getINT() const { return interrupt; }
    void setINT(bool val) { interrupt = val; }
    QString toLog() const;

private:
    quint32 addr;
    quint8 data;
    quint8 dest;
    BUS_FUNC func;
    bool    interrupt;


};

Q_DECLARE_METATYPE(Cbus);

QDebug operator<<(QDebug dbg, const Cbus &bus);

#endif // BUS_H
