#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDebug>

#include "breakpoint.h"


Cbreakpoint::Cbreakpoint(Cbreakpoint::TYPE _type, UINT32 _from, UINT32 _to, int _val, bool _enabled)
{
    type = _type;
    adrFrom = _from;
    adrTo   = _to;
    enabled = _enabled;
    val = _val;
}

QString Cbreakpoint::toText() {
    QString ret;
    switch (type) {
    case Cbreakpoint::EXEC: ret = "(EXEC)  "; break;
    case Cbreakpoint::READ: ret = "(READ)  "; break;
    case Cbreakpoint::WRITE:ret = "(WRITE) "; break;
    case Cbreakpoint::READWRITE:ret = "(R/W) "; break;
    case Cbreakpoint::REG:  ret = "(REG)   "; break;
    }

    ret += QString("%1").arg(adrFrom,6,16,QChar('0'));
    if (adrFrom != adrTo)
        ret += QString(" - %1").arg(adrTo,6,16,QChar('0'));

    return QString(ret);
}

void Cbreakpoint::serialize(QXmlStreamWriter *xmlOut)
{
    xmlOut->writeStartElement("breakpoint");
    xmlOut->writeAttribute("type",typeToText(type));
    xmlOut->writeAttribute("adrFrom",QString("%1").arg(adrFrom,6,16,QChar('0')));
    xmlOut->writeAttribute("adrTo",QString("%1").arg(adrTo,6,16,QChar('0')));
    xmlOut->writeAttribute("enabled",enabled?"true":"false");
    xmlOut->writeAttribute("val",val==-1? "-1" : QString("%1").arg(val,6,16,QChar('0')));
    xmlOut->writeEndElement();  // breakpoints
}

Cbreakpoint* Cbreakpoint::unserialize(QXmlStreamReader *xmlIn)
{
    QString eltname = xmlIn->name().toString();
    if (eltname=="breakpoint") {
        TYPE _type = textToType(xmlIn->attributes().value("type").toString());
        UINT32 _adrFrom = xmlIn->attributes().value("adrFrom").toUInt(0,16);
        UINT32 _adrTo = xmlIn->attributes().value("adrTo").toUInt(0,16);
        bool _enabled = xmlIn->attributes().value("enabled").toString()=="true" ? true : false;
        int _val = xmlIn->attributes().value("val").toInt(0,16);
        Cbreakpoint *_bpt = new Cbreakpoint(_type,_adrFrom,_adrTo,_val,_enabled);

        return _bpt;
    }
    return 0;
}

QString Cbreakpoint::typeToText(Cbreakpoint::TYPE _type)
{
    switch (_type) {
    case EXEC: return QString("EXEC");
    case READ: return QString("READ");
    case WRITE: return QString("WRITE");
    case READWRITE: return QString("R/W");
    case REG: return QString("REG");
    default: return "UNDEFINED";
    }

    return "UNDEFINED";
}

Cbreakpoint::TYPE Cbreakpoint::textToType(QString _type)
{
    if (_type=="EXEC") return EXEC;
    if (_type=="READ") return READ;
    if (_type=="WRITE") return WRITE;
    if (_type=="R/W") return READWRITE;
    if (_type=="REG") return REG;

    return UNDEFINED;
}

bool CbreakpointManager::isBreak(Cbreakpoint::TYPE _type,UINT32 _adr, int _val )
{
    switch (_type) {
    case Cbreakpoint::REG:
        break;

    case Cbreakpoint::EXEC:
        for (int i=0; i<breakList.count();i++) {
            Cbreakpoint *_bpt = breakList[i];
            if ( _bpt->isEnabled() &&
                 _bpt->isType(Cbreakpoint::EXEC) &&
                 (_adr >= _bpt->From()) &&
                 (_adr <= _bpt->To())) {

                breakMsg = _bpt->toText();
                qWarning()<<breakMsg;
                return true;
            }
        }
        break;

    case Cbreakpoint::READ:
        for (int i=0; i<breakList.count();i++) {
            Cbreakpoint *_bpt = breakList[i];
            if ( _bpt->isEnabled() &&
                 _bpt->isType(Cbreakpoint::READ) &&
                 (_adr >= _bpt->From()) &&
                 (_adr <= _bpt->To()) &&
                 ( (_bpt->Val()==-1) || (_bpt->Val()==_val) ) ) {

                breakMsg = _bpt->toText();
                qWarning()<<breakMsg;
                return true;
            }
        }
        break;
    case Cbreakpoint::WRITE:
        for (int i=0; i<breakList.count();i++) {
            Cbreakpoint *_bpt = breakList[i];
            if ( _bpt->isEnabled() &&
                 _bpt->isType(Cbreakpoint::WRITE) &&
                 (_adr >= _bpt->From()) &&
                 (_adr <= _bpt->To()) &&
                 ( (_bpt->Val()==-1) || (_bpt->Val()==_val) ) ) {
                qWarning()<<"break write";
                breakMsg = _bpt->toText();
                qWarning()<<breakMsg;
                return true;
            }
        }
        break;

    default: return false;
    }
    return false;
}

void CbreakpointManager::addBreakPoint(Cbreakpoint *_b)
{
    breakList.append(_b);
}

void CbreakpointManager::serialize(QXmlStreamWriter *xmlOut) {
    xmlOut->writeStartElement("breakpoints");
        for (int i=0; i < breakList.count(); i++) {
            breakList.at(i)->serialize(xmlOut);
        }
    xmlOut->writeEndElement();  // breakpoints
}

void CbreakpointManager::unserialize(QXmlStreamReader *xmlIn) {
    if (xmlIn->readNextStartElement() &&
            (xmlIn->name() == "breakpoints")) {
        while (xmlIn->readNextStartElement()) {
            Cbreakpoint *_bpt = Cbreakpoint::unserialize(xmlIn);
            if (_bpt) breakList.append(_bpt);
            xmlIn->skipCurrentElement();
        }
    }
}
