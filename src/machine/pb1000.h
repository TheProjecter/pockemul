#ifndef CPB1000_H
#define CPB1000_H

#include "pcxxxx.h"

class CHD44352;

class Cpb1000 : public CpcXXXX
{
public:
    Cpb1000(CPObject *parent = 0);
    bool init();
    bool run();
    bool Chk_Adr(DWORD *d, DWORD data);
    bool Chk_Adr_R(DWORD *d, DWORD data);
    UINT8 in(UINT8 Port);
    UINT8 out(UINT8 Port, UINT8 Value);
    void TurnOFF();
    void TurnON();
    void Reset();
    bool LoadConfig(QXmlStreamReader *xmlIn);
    bool SaveConfig(QXmlStreamWriter *xmlOut);

    CHD44352 *pHD44352;
    UINT16 getKey();
    void setKey(UINT8 data);
    UINT8 m_kb_matrix;
    WORD Get_16rPC(DWORD adr);
};

#endif // CPB1000_H
