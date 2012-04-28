#ifndef CPB2000_H
#define CPB2000_H

#include "pb1000.h"

class CHD44352;

class Cpb2000 : public Cpb1000
{
    Q_OBJECT

public:
    Cpb2000(CPObject *parent = 0);

    bool Chk_Adr(DWORD *d, DWORD data);
    bool Chk_Adr_R(DWORD *d, DWORD data);

    CHD44352 *pHD44352;
    UINT16 getKey();
    void setKey(UINT8 data);
    UINT8 m_kb_matrix;

    virtual void paintEvent(QPaintEvent *event);
    virtual void TurnCLOSE();
};

#endif // CPB2000_H
