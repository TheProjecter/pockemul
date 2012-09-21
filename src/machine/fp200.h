#ifndef FP200_H
#define FP200_H

class CPObject;
class Ci8085;

#include "pcxxxx.h"

class Cfp200 : public CpcXXXX
{
    Q_OBJECT

public:
    Cfp200(CPObject *parent = 0);
    virtual ~Cfp200();

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    virtual UINT8 in(UINT8 address);
    virtual UINT8 out(UINT8 address,UINT8 value);
    virtual UINT8 in8(UINT16 Port);
    virtual UINT8 out8(UINT16 Port,UINT8 x);
    virtual UINT16 in16(UINT16 Port);
    virtual UINT16 out16(UINT16 Port, UINT16 x);
    bool init();

    virtual bool run();

    virtual void Reset();

    void TurnON();
    void TurnOFF();
    bool SaveConfig(QXmlStreamWriter *xmlOut);
    bool LoadConfig(QXmlStreamReader *xmlIn);

//    UINT16 getKey();

    Ci8085 *i85cpu;

    quint8 ks;
    BYTE getKey();

    int lastKeyBufSize;
    bool newKey;
    void keyReleaseEvent(QKeyEvent *event);
    void keyPressEvent(QKeyEvent *event);
    UINT8 matrixResult;
};


#endif // FP200_H
