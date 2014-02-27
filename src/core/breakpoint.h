#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "common.h"

class QXmlStreamWriter;
class QXmlStreamReader;

class Cbreakpoint {

public:
enum TYPE {
    EXEC,
    READ,
    WRITE,
    READWRITE,
    REG,
    UNDEFINED
};
    Cbreakpoint() {}
    Cbreakpoint(TYPE _type, UINT32 _from, UINT32 _to, int _val = -1 ,bool _enabled=true);

    bool isEnabled() { return enabled;}
    void setEnabled(bool _val) { enabled = _val;}
    UINT32 From() { return adrFrom;}
    UINT32 To() { return adrTo;}
    bool isType(TYPE _val) { return (type == _val); }
    int Val() { return val; }
    QString toText();

    void serialize(QXmlStreamWriter *xmlOut);
    static Cbreakpoint* unserialize(QXmlStreamReader *xmlIn);
    static QString typeToText(TYPE _type);
    static TYPE textToType(QString _type);

private:
    UINT32 adrFrom;
    UINT32 adrTo;
    TYPE type;
    int val;
    bool enabled;
};

class CbreakpointManager {
public:
    bool isBreak(Cbreakpoint::TYPE _type, UINT32 _adr, int _val=-1);
    void addBreakPoint(Cbreakpoint *_b);


    QList<Cbreakpoint*> breakList;


    void serialize(QXmlStreamWriter *xmlOut);
    void unserialize(QXmlStreamReader *xmlIn);

    QString breakMsg;
};

#endif // BREAKPOINT_H
